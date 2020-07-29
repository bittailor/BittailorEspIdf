idf_component_register(
	SRCS 
		"mock/Bt/Core/TimeMock.cpp"
		"mock/Bt/Core/TimeStub.cpp"
	INCLUDE_DIRS "mock"
	REQUIRES Trompeloeil BtCore 
)