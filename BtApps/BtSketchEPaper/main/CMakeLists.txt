idf_component_register(
	SRCS 
		"BtSketchEPaperMain.cpp" 
		"imagedata.cpp" 
		"epdpaint.cpp" 
		"font8.cpp"
		"font12.cpp"
		"font16.cpp"
		"font20.cpp"
		"font24.cpp"
    INCLUDE_DIRS "." 
    REQUIRES epaper BtCore BtCoreEspIdf 
)
