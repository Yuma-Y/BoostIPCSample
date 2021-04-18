set(CMAKE_BUILD_TYPE Debug)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")

add_executable(test_client client_test_code.cpp)

target_include_directories(test_client PUBLIC
	${CMAKE_SOURCE_DIR}/src
	)
	
target_link_libraries(test_client
	gtest gtest_main rt)
	
add_executable(test_server server_main.cpp)

target_include_directories(test_server PUBLIC
	${CMAKE_SOURCE_DIR}/src
	)

target_link_libraries(test_server rt)