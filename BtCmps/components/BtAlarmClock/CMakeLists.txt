idf_component_register(
	SRCS 
		"src/Bt/AlarmClock/RepeatingAlarm.cpp"
		"src/Bt/AlarmClock/AlarmActor.cpp"
		"src/Bt/AlarmClock/Clock.cpp"
		"src/Bt/AlarmClock/Buzzer.cpp"
		"src/Bt/AlarmClock/AlarmController.cpp"
		"src/Bt/AlarmClock/ConfigurationController.cpp"
		"src/Bt/AlarmClock/MqttConfigurationConnector.cpp"
		"src/Bt/AlarmClock/DisplayBacklight.cpp"
		"src/Bt/AlarmClock/OnceAlarm.cpp"
	INCLUDE_DIRS "src"
	REQUIRES BtCore ArduinoJson
)