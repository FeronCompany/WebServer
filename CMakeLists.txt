cmake_minimum_required (VERSION 3.16)

if(NOT SET_MY_SERVER_NAME)
	SET(SET_MY_SERVER_NAME cream_web_server)
endif()

if(NOT SET_MY_CLIENT_NAME)
	SET(SET_MY_CLIENT_NAME cream_web_client)
endif()

project (${SET_MY_SERVER_NAME} VERSION 0.1.0 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_compile_options(-std=c++11 -g)

LIST(APPEND SRC_DIR commlib frame)
foreach(cur_dir ${SRC_DIR})
	file(GLOB_RECURSE SRCS ${cur_dir}/*.cpp)
	file(GLOB_RECURSE HDRS ${cur_dir}/*.h)
	file(GLOB_RECURSE HPPS ${cur_dir}/*.hpp)
	LIST(APPEND SRC_LIST ${SRCS})
	LIST(APPEND HDR_LIST ${HDRS} ${HPPS})
endforeach()
add_library(cws_frame STATIC ${SRC_LIST} ${HDR_LIST})
add_executable(${SET_MY_SERVER_NAME} src/main.cpp)
install (TARGETS ${SET_MY_SERVER_NAME} DESTINATION bin)

file(GLOB_RECURSE TEST_HPPS test/*.hpp)
file(GLOB_RECURSE TEST_HDRS test/*.h)
file(GLOB_RECURSE TEST_CPPS test/*.cpp)
LIST(APPEND TEST_HDR ${TEST_HPPS} ${TEST_HDRS})
LIST(APPEND TEST_SRC ${TEST_CPPS})
add_executable(${SET_MY_CLIENT_NAME} ${TEST_HDR} ${TEST_SRC})

SET(PROJECT_ROOT_PATH ".")   
INCLUDE_DIRECTORIES("${PROJECT_ROOT_PATH}")
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Protobuf REQUIRED)
find_package(Threads REQUIRED)

SET(PROTO_PATH "${CMAKE_CURRENT_SOURCE_DIR}/src")
SET(PROJECT_BUILD_DIR "${CMAKE_CURRENT_SOURCE_DIR}/build")
SET(PROTO_META_BASE_DIR "${PROJECT_BUILD_DIR}/stubs")
file(GLOB protobuf_files
	${PROTO_PATH}/*.proto
)
LIST(APPEND PROTO_FLAGS -I${PROTO_PATH})
if(NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/build/stubs)
	file(MAKE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/build/stubs)
endif()
FOREACH(FIL ${protobuf_files})
	message("Dealing with ${FIL}")
	string(REGEX MATCH "^/.*/([^ ]*)/([^ ]*)\.proto$" proto_name ${FIL})
	list(APPEND BUSI_HDR ${PROTO_META_BASE_DIR}/${CMAKE_MATCH_2}.pb.h)
	list(APPEND BUSI_SRC ${PROTO_META_BASE_DIR}/${CMAKE_MATCH_2}.pb.cc)
	EXECUTE_PROCESS(
		COMMAND ${PROTOBUF_PROTOC_EXECUTABLE} ${PROTO_FLAGS} --cpp_out=${PROTO_META_BASE_DIR} ${FIL}
    )
ENDFOREACH()
file(GLOB proj_dirs src/*)
foreach(child ${proj_dirs})
	if(IS_DIRECTORY ${child})
		file(GLOB_RECURSE BUSI_HPPS ${child}/*.hpp)
		file(GLOB_RECURSE BUSI_HDRS ${child}/*.h)
		file(GLOB_RECURSE BUSI_CPPS ${child}/*.cpp)
		list(APPEND BUSI_HDR ${BUSI_HPPS} ${BUSI_HDRS})
		list(APPEND BUSI_SRC ${BUSI_CPPS})
	endif()
endforeach()
add_library(cws_business STATIC ${BUSI_HDR} ${BUSI_SRC})
target_include_directories(cws_business
	PUBLIC cws_frame
)
target_link_libraries(cws_business
	PUBLIC cws_frame
	PRIVATE ${PROTOBUF_LIBRARY}
)

target_include_directories(${SET_MY_SERVER_NAME}
	PRIVATE ${PROTOBUF_INCLUDE_DIR}
	PUBLIC cws_frame
)
target_link_libraries(${SET_MY_SERVER_NAME}
	PRIVATE ${PROTOBUF_LIBRARY}
	PRIVATE Threads::Threads
	PUBLIC cws_business
)

target_include_directories(${SET_MY_CLIENT_NAME}
	PRIVATE ${PROTOBUF_INCLUDE_DIR}
	PUBLIC cws_frame
)
target_link_libraries(${SET_MY_CLIENT_NAME}
	PRIVATE ${PROTOBUF_LIBRARY}
	PRIVATE Threads::Threads
	PUBLIC cws_business
)
