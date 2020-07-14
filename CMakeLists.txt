cmake_minimum_required(VERSION 3.10)

project(TestTask)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS_DEBUG   "${CMAKE_CXX_FLAGS_DEBUG}   -g3")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -o3")

set(PRJ_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(IDL_DIR ${PRJ_DIR}/idl)
set(STUB_DIR ${PRJ_DIR}/stub)
set(SRC_DIR ${PRJ_DIR}/src)

set(CLIENT clt.cc)
set(SERVER impl.cc)
set(CLIENT_OUT client.out)
set(SERVER_OUT server.out)

set(SERVER_IDL speaker.idl)

find_library(omniORB_FOUND NAMES omniORB4)
find_library(omnithread_FOUND NAMES omnithread)
find_library(COS_FOUND NAMES COS4)
find_library(omniDynamic_FOUND NAMES omniDynamic4)
find_library(omniZIOP_FOUND NAMES omniZIOP4)

if (omniORB_FOUND AND omnithread_FOUND AND COS_FOUND AND omniDynamic_FOUND)
    message("OK!")

    file(MAKE_DIRECTORY ${STUB_DIR})
    execute_process(COMMAND omniidl -C${STUB_DIR} -bcxx ${IDL_DIR}/${SERVER_IDL}) 

    add_executable(${CLIENT_OUT} ${SRC_DIR}/${CLIENT} ${STUB_DIR}/speakerSK.cc)
    add_executable(${SERVER_OUT} ${SRC_DIR}/${SERVER} ${STUB_DIR}/speakerSK.cc)

    target_link_libraries(${CLIENT_OUT} PUBLIC omniORB4 omnithread COS4
                                               omniDynamic4 omniZIOP4)
    target_link_libraries(${SERVER_OUT} PUBLIC omniORB4 omnithread COS4
                                               omniDynamic4 omniZIOP4)
    target_include_directories(${CLIENT_OUT}
                               PUBLIC ${STUB_DIR})
    target_include_directories(${SERVER_OUT}
                               PUBLIC ${STUB_DIR})
else ()
    message(FATAL_ERROR "Can't find omniORB or omnithread")
endif ()
