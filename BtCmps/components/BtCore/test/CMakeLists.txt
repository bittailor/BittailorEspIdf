idf_component_register(
	SRCS 
		"Bt/Concurrency/AtomicTest.cpp"
		"Bt/Bluetooth/BleUuidTest.cpp"
		"Bt/Concurrency/CountingLatchTest.cpp"
		"Bt/Core/TimezoneTest.cpp"
		"Bt/Concurrency/ExecutionContextTest.cpp"
		"Bt/Concurrency/CountdownLatchTest.cpp"
		"Bt/Concurrency/LoopExecutionContextTest.cpp"
		"Bt/Concurrency/BlockingQueueTest.cpp"
		"Bt/Concurrency/WorkerExecutionContextTest.cpp"
		"Bt/Concurrency/CountingLatchTest.cpp"
		"Bt/Concurrency/SchedulingExecutionContextBaseTest.cpp"
		"Bt/Protocols/Mqtt/MqttTest.cpp"
		"Bt/Core/SingletonTest.cpp"
	INCLUDE_DIRS "."
	REQUIRES Catch2 BtCore BtCoreMocks
)