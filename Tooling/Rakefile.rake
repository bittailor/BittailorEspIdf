require 'rubygems'
begin
    require 'serialport'
rescue LoadError
    puts 'gem serialport not found'
end


#port = '/dev/cu.usbserial-1410'
# port = '/dev/cu.usbserial-1420'
# port = '/dev/cu.usbserial-14101'
port = '/dev/cu.usbserial-14110'
# port = 'ls -0001'

baudrate = 921600
#baudrate = 2764800

device = "30:AE:A4:CC:47:68"

build_target_folders = ['BtApps/BtAppAlarmClock', 'BtApps/BtSketchEPaper', 'BtCmps/test', 'BtCmps']
build_host_folders = ['BtHostTests']
build_folders = build_host_folders + build_target_folders 

test_host_folders = build_host_folders
test_target_folders = ['BtCmps/test']

#flash_folder = 'BtApps/BtAppAlarmClock'
# flash_folder = 'BtApps/BtMijiaBleSensorGateway'
flash_folder = 'BtApps/BtBleAdvertisementsGateway'
 
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
                puts 
                puts "** #{folder} **"
                sh "pwd"  
                idf_sh "idf.py build"
            end
        end
    end

    task :host do 
        build_host_folders.each do |folder|
            Dir.chdir(folder) do
                puts 
                puts "** #{folder} **"
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

task :ota do
    Dir.chdir(flash_folder) do    
        sh "mosquitto_pub -h piOne.local -u ***REMOVED*** -P ***REMOVED*** -p 1883 -t bittailor/ota/#{device}/data -q 2 -f build/BtBleAdvertisementsGateway.bin"
        
        # sh 'mosquitto_pub -h piOne.local -u ***REMOVED*** -P ***REMOVED*** -t bittailor/ota/D8:A0:1D:5E:2E:00/data -q 2 -f build/BtMijiaBleSensorGateway.bin '
        # sh 'mosquitto_pub -h broker.hivemq.com -t bittailor/ota/D8:A0:1D:5E:2E:00/data -q 2 -f build/BtMijiaBleSensorGateway.bin'
        # sh 'mosquitto_pub -h piOne.local -u ***REMOVED*** -P ***REMOVED*** -p 1883 -t bittailor/ota/3C:71:BF:47:DA:94/data -q 2 -f build/BtMijiaBleSensorGateway.bin'
        # sh 'mosquitto_pub -h piOne.local -p 1883 -t bittailor/ota/3C:71:BF:47:DA:94/data -q 2 -f ../BtSketchOta/build/BtSketchOta.bin'
        # sh 'mosquitto_pub -h broker.emqx.io -t bittailor/ota/D8:A0:1D:5E:2E:00/data -q 2 -f build/BtMijiaBleSensorGateway.bin'
        # mosquitto_sub -h piOne.local -u ***REMOVED*** -P ***REMOVED*** -t bittailor/develop/uartgateway/370020000a47343232363230/uart/raw
    end
end


task :restart do
    Dir.chdir(flash_folder) do    
        sh "mosquitto_pub -h piOne.local -u ***REMOVED*** -P ***REMOVED*** -p 1883 -t bittailor/ota/#{device}/restart -q 2 -m \"restart\" "
    end
end

task :btota do
    Dir.chdir(flash_folder) do
        fragement = 1024
        contents = nil
        File.open("build/BtMijiaBleSensorGateway.bin", "rb") do |file| 
            contents = file.read
            puts contents.size
        end
        puts contents.size
        File.open("build/BtMijiaBleSensorGateway.bin.out", "wb") do |file| 
            sent = 0
            while sent < contents.size
                slice_size = [1024,(contents.size-sent)].min
                puts "sent #{sent} slice_size #{slice_size}"
                file.write(contents[sent..(sent+slice_size)])
                sent = sent + slice_sizecmp
            end
        end
    end
end

task :test => 'test:all'
task :build => 'build:all'

# task :default => [ 'test:all' ]
task :default => [ 'flash' ]


