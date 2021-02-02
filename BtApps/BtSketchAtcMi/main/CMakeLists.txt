idf_component_register(
	SRCS 
		"Main.cpp" 
    INCLUDE_DIRS "." 
    REQUIRES epaper BtCore BtCoreEspIdf ArduinoJson BtXiaomi
)

# spiffs_create_partition_image(default ../spiffs/default FLASH_IN_PROJECT)

