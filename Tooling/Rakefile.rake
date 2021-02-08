require 'rubygems'
begin
    require 'serialport'
rescue LoadError
    puts 'gem serialport not found'
end

testig_port = '/dev/cu.usbserial-1410'
flash_port = '/dev/cu.usbserial-1410'

# ota_device = '30:AE:A4:CC:47:68'
ota_device = 'D8:A0:1D:5E:2E:00'

apps = Rake::FileList["BtApps/*"].pathmap("%f")
cmps = ['BtCmps']
hosts = ['BtHostTests']

def idf_sh(cmd)
    return sh(". $HOME/esp/esp-idf/export.sh && " + cmd)
end

hosts.each do |host|
    # build 
    namespace :build do
        namespace :host do
            task host do
                Dir.chdir("#{host}") do
                    puts
                    puts "#####################################################"
                    puts "## BUILD ==> #{host}"
                    puts "#####################################################"
                    sh 'mkdir -p build'
                    Dir.chdir('build') do 
                        sh 'cmake .. -G Ninja -DCMAKE_BUILD_TYPE=Debug'
                        sh 'ninja'
                    end 
                end 
            end
            task :all => "build:host:#{host}"
        end 
        task :all => 'build:host:all'
    end 

    #test 
    namespace :test do
        namespace :host do
            task host => "build:host:#{host}" do
                Dir.chdir("#{host}") do
                    puts
                    puts "#####################################################"
                    puts "## TEST ==> #{host}"
                    puts "#####################################################"
                    Dir.chdir('build') do 
                        sh './BtHostTests'
                    end 
                end 
            end
            task :all => "test:host:#{host}"
        end 
        task :all => 'test:host:all'
    end 
    
    # clean 
    namespace :clean do
        namespace :host do
            task host do
                Dir.chdir("#{host}") do
                    puts
                    puts "#####################################################"
                    puts "## CLEAN ==> #{host}"
                    puts "#####################################################"
                    sh "rm -rf build"
                end 
            end
            task :all => "clean:host:#{host}"
        end 
        task :all => 'clean:host:all'
    end  
end

cmps.each do |cmp|
    # build 
    namespace :build do
        namespace :cmp do
            task cmp do
                Dir.chdir("#{cmp}") do
                    puts
                    puts "#####################################################"
                    puts "## BUILD ==> #{cmp}"
                    puts "#####################################################"
                    idf_sh "idf.py build"
                end
                if(File.exists?("#{cmp}/test"))
                    Dir.chdir("#{cmp}/test") do
                        puts
                        puts "#####################################################"
                        puts "## BUILD ==> #{cmp} - test"
                        puts "#####################################################"
                        idf_sh "idf.py build"
                    end
                end 
            end
            task :all => "build:cmp:#{cmp}"
        end 
        task :all => 'build:cmp:all'
    end 

    if File.exists?("#{cmp}/test")
        #test 
        namespace :test do
            namespace :cmp do
                task cmp => "build:cmp:#{cmp}" do
                    Dir.chdir("#{cmp}/test") do
                        puts
                        puts "#####################################################"
                        puts "## TEST ==> #{cmp}"
                        puts "#####################################################"
                        puts "=> flash:" 
                        idf_sh "idf.py -p #{testig_port} flash"
                        puts "=> start watching serial:"
                        
                        failed = false;
                        SerialPort.open(testig_port, 115200) do |serial|
                            loop do
                                line = serial.readline()
                                puts "s> " + line
                                begin
                                    string = line.strip
                                    #puts ">#{string}< #{string == '!! FAILED !!'}"
                                    break if string == '<!-*END-OF-TEST-RUN*-!>'
                                    failed = true if string == '!! ERROR  !!'
                                    failed = true if string == '!! FAILED !!'
                                rescue                
                                end    
                            end
                        end 
                        fail if failed
                    end 
                end
                task :all => "test:cmp:#{cmp}"
            end 
            task :all => 'test:cmp:all'
        end 
    end
    
    # clean 
    namespace :clean do
        namespace :cmp do
            task cmp do
                Dir.chdir("#{cmp}") do
                    puts
                    puts "#####################################################"
                    puts "## CLEAN ==> #{cmp}"
                    puts "#####################################################"
                    sh "rm -rf build"
                end
                if(File.exists?("#{cmp}/test"))
                    Dir.chdir("#{cmp}/test") do
                        puts
                        puts "#####################################################"
                        puts "## CLEAN ==> #{cmp} - test"
                        puts "#####################################################"
                        sh "rm -rf build"
                    end
                end  
            end
            task :all => "clean:cmp:#{cmp}"
        end 
        task :all => 'clean:cmp:all'
    end  
end

apps.each do |app|
    # build 
    namespace :build do
        namespace :app do
            task app do
                Dir.chdir("BtApps/#{app}") do
                    puts
                    puts "#####################################################"
                    puts "## BUILD ==> #{app}"
                    puts "#####################################################"
                    idf_sh "idf.py build"
                end 
            end
            task :all => "build:app:#{app}"
        end 
        task :all => 'build:app:all'
    end 

    # flash 
    namespace :flash do
        namespace :app do
            task app do
                Dir.chdir("BtApps/#{app}") do
                    puts
                    puts "#####################################################"
                    puts "## FLASH ==> #{app}"
                    puts "#####################################################"
                    idf_sh "idf.py flash"
                end 
            end
            task :all => "flash:app:#{app}"
        end 
        task :all => 'flash:app:all'
    end 

    # ota 
    namespace :ota do
        namespace :app do
            task app => "build:app:#{app}" do
                Dir.chdir("BtApps/#{app}") do
                    puts
                    puts "#####################################################"
                    puts "## OTA ==> #{app}"
                    puts "#####################################################"
                    sh "mosquitto_pub -h piOne.local -u ***REMOVED*** -P ***REMOVED*** -p 1883 -t bittailor/ota/#{ota_device}/data -q 2 -f build/#{app}.bin"
                end 
            end
            task :all => "ota:app:#{app}"
        end 
        task :all => 'ota:app:all'
    end
    
    # restart 
    namespace :restart do
        namespace :app do
            task app do
                Dir.chdir("BtApps/#{app}") do
                    puts
                    puts "#####################################################"
                    puts "## RESTART ==> #{app}"
                    puts "#####################################################"
                    sh "mosquitto_pub -h piOne.local -u ***REMOVED*** -P ***REMOVED*** -p 1883 -t bittailor/ota/#{ota_device}/restart -q 2 -m \"restart\" "
                end 
            end
            task :all => "restart:app:#{app}"
        end 
        task :all => 'restart:app:all'
    end
    
    # clean 
    namespace :clean do
        namespace :app do
            task app do
                Dir.chdir("BtApps/#{app}") do
                    puts
                    puts "#####################################################"
                    puts "## CLEAN ==> #{app}"
                    puts "#####################################################"
                    sh "rm -rf build"
                end 
            end
            task :all => "clean:app:#{app}"
        end 
        task :all => 'clean:app:all'
    end  
end

task :default => ['build:all', 'test:host:all']
task :ci => ['build:all', 'test:host:all']

task :build => 'build:all'
task :test => ['build:all', 'test::all']