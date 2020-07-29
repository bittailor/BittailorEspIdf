idf_component_register(
	SRCS 
		"Bt/AlarmClock/RepeatingAlarmTest.cpp"
		"Bt/AlarmClock/AlarmControllerTest.cpp"
	INCLUDE_DIRS "."
	REQUIRES Catch2 BtCore BtCoreMocks BtAlarmClock BtAlarmClockMocks
)