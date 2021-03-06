set(srcs
	"src/Bt/Core/Time.cpp"
	"src/Bt/Events/DefaultEventLoop.cpp"
	"src/Bt/Events/EventLoopSubscription.cpp"
	"src/Bt/Network/WiFiController.cpp"
	"src/Bt/Peripherals/DigitalIn.cpp"
	"src/Bt/Peripherals/DigitalOut.cpp"
	"src/Bt/Peripherals/InterruptPushButton.cpp"
	"src/Bt/Peripherals/PwmOut.cpp"
	"src/Bt/Peripherals/PwmTimer.cpp"
	"src/Bt/Peripherals/SpiDevice.cpp"
	"src/Bt/Peripherals/SpiMaster.cpp"
	"src/Bt/Protocols/MqttController.cpp"
	"src/Bt/Protocols/SntpController.cpp"
	"src/Bt/Storage/NvsRepository.cpp"
	"src/Bt/Storage/VirtualFilesystem.cpp"
	"src/Bt/System/OtaUpdate.cpp"
	"src/Bt/System/System.cpp"
	"src/Bt/System/Vitals.cpp"
	"src/Bt/Ui/LedStrip.cpp"
)

if(CONFIG_BT_NIMBLE_ENABLED)
	list(APPEND srcs 
		"src/Bt/Bluetooth/BleCharacteristic.cpp"
		"src/Bt/Bluetooth/BleClient.cpp"
		"src/Bt/Bluetooth/BleController.cpp"
		"src/Bt/Bluetooth/BleDescriptor.cpp"
		"src/Bt/Bluetooth/BleDeviceDiscoveryAgent.cpp"
		"src/Bt/Bluetooth/BleService.cpp"
		"src/Bt/Bluetooth/Utilities.cpp"
	)
endif()

idf_component_register(
	SRCS	"${srcs}" 
	INCLUDE_DIRS "src"
	REQUIRES driver nvs_flash mqtt spiffs bt app_update BtCore ArduinoJson
)