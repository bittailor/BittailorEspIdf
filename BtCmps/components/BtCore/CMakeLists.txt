idf_component_register(
	SRCS 
		"src/Bt/Concurrency/CountingLatch.cpp"
		"src/Bt/Core/Logging.cpp"
		"src/Bt/Core/StringBuilder.cpp"
		"src/Bt/Core/Timezone.cpp"
		"src/Bt/Concurrency/SchedulingExecutionContext.cpp"
		"src/Bt/Concurrency/SchedulingExecutionContextBase.cpp"
		"src/Bt/Concurrency/CountdownLatch.cpp"
		"src/Bt/Concurrency/LoopExecutionContext.cpp"
		"src/Bt/Concurrency/CountingLatch.cpp"
		"src/Bt/Concurrency/I_ExecutionContext.cpp"
		"src/Bt/Concurrency/WorkerExecutionContext.cpp"
		"src/Bt/Events/Events.cpp"
		"src/Bt/Peripherals/RgbLed.cpp"
		"src/Bt/Protocols/MqttSubscription.cpp"
		"src/Bt/Protocols/Mqtt/Mqtt.cpp"
	INCLUDE_DIRS "src"
	REQUIRES Date
)