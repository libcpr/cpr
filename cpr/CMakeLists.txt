include_directories(
    ${CPR_INCLUDE_DIRS}
    ${CURL_INCLUDE_DIRS})

add_library(${CPR_LIBRARIES}
    api.cpp
    auth.cpp
    session.cpp
    util.cpp)

target_link_libraries(${CPR_LIBRARIES}
    ${CURL_LIBRARIES})

add_executable(cprmain
    main.cpp)

target_link_libraries(cprmain
    ${CPR_LIBRARIES})
