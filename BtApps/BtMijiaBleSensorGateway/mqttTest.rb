require 'rubygems'
require 'mqtt'

MQTT::Client.connect('mqtt://***REMOVED***:***REMOVED***@piOne.local') do |c|
    c.get('bittailor/#') do |topic,message|
      puts "#{topic}: #{message.size()}"
    end
end