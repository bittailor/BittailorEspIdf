
port = '/dev/cu.usbserial-0001'
flashAppFolder = 'BtApps/BtAppAlarmClock'
flashTestFolder = 'BtCmps/test'
hostFolder = 'BtHostTests'

def idf_sh(cmd)
    return sh(". $HOME/esp/esp-idf/export.sh && " + cmd)
end

task :flash do 
    Dir.chdir(flashAppFolder) do    
        sh "idf.py -p #{port} flash monitor"
    end
end

namespace :test do

    task :target do 
        Dir.chdir(flashTestFolder) do  
            sh "pwd"  
            idf_sh "idf.py -p #{port} flash monitor"
        end
    end

    task :host do 
        Dir.chdir(hostFolder) do
            sh "pwd"    
            sh 'mkdir -p build'
            Dir.chdir('build') do 
                sh 'cmake .. -G Ninja'
                sh 'ninja'
                sh './BtHostTests'
            end 
        end
    end

    task :all => [:host , :target]

end

task :test => 'test:all'

task :default => [ :flash ]


