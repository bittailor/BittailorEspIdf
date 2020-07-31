
port = '/dev/cu.usbserial-0001'


build_target_folders = ['BtApps/BtAppAlarmClock', 'BtCmps/test', 'BtCmps']
build_host_folders = ['BtHostTests']
build_folders = build_host_folders + build_target_folders 

test_host_folders = build_host_folders
test_target_folders = ['BtCmps/test']

flash_folder = 'BtApps/BtAppAlarmClock'
 
def idf_sh(cmd)
    return sh(". $HOME/esp/esp-idf/export.sh && " + cmd)
end

namespace :test do

    task :target do
        test_target_folders.each do |folder| 
            Dir.chdir(folder) do  
                sh "pwd"  
                idf_sh "idf.py -p #{port} flash monitor"
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

    task :all => [:host , :target]
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
                    sh 'cmake .. -G Ninja'
                    sh 'ninja'
                end 
            end
        end
    end

    task :all => [:host , :target]
end

task :clean do 
    build_folders.each do |f|
        Dir.chdir(f) do
            sh "rm -rf build"
        end
    end

end

task :flash do 
    Dir.chdir(flash_folder) do    
        idf_sh "idf.py -p #{port} flash monitor"
    end
end

task :test => 'test:all'
task :build => 'build:all'

task :default => [ :flash ]


