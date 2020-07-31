idf_component_register(
	SRCS 
		"src/Bt/Core/Time.cpp"
		"src/Bt/Events/DefaultEventLoop.cpp"
		"src/Bt/Events/EventLoopSubscription.cpp"
		"src/Bt/Network/WiFiController.cpp"
		"src/Bt/Peripherals/DigitalOut.cpp"
		"src/Bt/Protocols/MqttController.cpp"
		"src/Bt/Protocols/SntpController.cpp"
		"src/Bt/Storage/NvsRepository.cpp"
		"src/Bt/Storage/VirtualFilesystem.cpp"
	INCLUDE_DIRS "src"
	REQUIRES driver nvs_flash mqtt spiffs BtCore
)