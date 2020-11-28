require 'rubygems'
begin
    require 'serialport'
rescue LoadError
    puts 'gem serialport not found'
end


port = '/dev/cu.usbserial-1410'
# port = '/dev/cu.usbserial-1420'
# port = '/dev/cu.usbserial-14101'
# port = 'ls -0001'
baudrate = 921600


build_target_folders = ['BtApps/BtAppAlarmClock', 'BtApps/BtSketchEPaper', 'BtCmps/test', 'BtCmps']
build_host_folders = ['BtHostTests']
build_folders = build_host_folders + build_target_folders 

test_host_folders = build_host_folders
test_target_folders = ['BtCmps/test']

#flash_folder = 'BtApps/BtAppAlarmClock'
flash_folder = 'BtApps/BtMijiaBleSensorGateway'
 
def idf_sh(cmd)
    return sh(". $HOME/esp/esp-idf/export.sh && " + cmd)
end

namespace :test do

    task :target do
        test_target_folders.each do |folder| 
            Dir.chdir(folder) do  
                sh "pwd"  
                #idf_sh "idf.py -p #{port} flash monitor"
                #idf_sh "idf.py -p #{port} -b   #{115200*8} flash"
                idf_sh "idf.py -p #{port} -b #{baudrate} flash"
                puts "start watching serial"
                failed = false;
                SerialPort.open(port, 115200) do |serial|
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
    end

    task :host do 
        test_host_folders.each do |folder|
            Dir.chdir(folder) do
                sh "pwd"    
                sh 'mkdir -p build'
                Dir.chdir('build') do 
                    sh 'cmake .. -G Ninja'
                    sh 'ninja'
                    sh './BtHostTests'
                end 
            end
        end
    end

    task :all => ['test:host' , 'test:target']
end

namespace :build do

    task :target do
        build_target_folders.each do |folder| 
            Dir.chdir(folder) do  
                sh "pwd"  
                idf_sh "idf.py build"
            end
        end
    end

    task :host do 
        build_host_folders.each do |folder|
            Dir.chdir(folder) do
                sh "pwd"    
                sh 'mkdir -p build'
                Dir.chdir('build') do 
                    sh 'cmake .. -G Ninja -DCMAKE_BUILD_TYPE=Debug'
                    sh 'ninja'
                end 
            end
        end
    end

    task :all => ['build:host' , 'build:target']
end

task :clean do 
    build_folders.each do |f|
        Dir.chdir(f) do
            sh "rm -rf build"
        end
    end

end

task :conf do 
    Dir.chdir(flash_folder) do    
        idf_sh "idf.py menuconfig"
    end
end

task :flash do 
    Dir.chdir(flash_folder) do    
        idf_sh "idf.py -p #{port} -b #{baudrate} flash monitor"
    end
end

task :test => 'test:all'
task :build => 'build:all'

task :default => [ 'test:all' ]

