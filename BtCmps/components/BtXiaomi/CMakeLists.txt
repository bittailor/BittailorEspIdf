idf_component_register(
	SRCS 
		"src/Bt/Xiaomi/Gateway.cpp"
		"src/Bt/Xiaomi/AdvertisementsGateway.cpp"
	INCLUDE_DIRS "src"
	REQUIRES BtCore ArduinoJson
)