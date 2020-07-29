idf_component_register(
	SRCS "AlarmClockMain.cpp"
    INCLUDE_DIRS "." 
    REQUIRES BtCore BtCoreEspIdf BtAlarmClock  
)

spiffs_create_partition_image(default ../spiffs/default FLASH_IN_PROJECT)

# REQUIRES nvs_flash mqtt spiffs BtCore BtCoreEspIdf BtAlarmClock
