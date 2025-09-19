set(APP_NAME multiplot-exec)

set(CMAKE_AUTOMOC ON)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(Qt5 REQUIRED Core Gui Widgets LinguistTools)

set(HEADERS
    localfilemodel.hpp
    src/simleoscreader.hpp

        )
set(SOURCES
    main.cpp
    localfilemodel.cpp
    src/simleoscreader.cpp
        )

add_executable(${APP_NAME} ${SOURCES} ${HEADERS})

target_include_directories(${APP_NAME} PRIVATE ${QWT_INCLUDE_DIR})
target_include_directories(${APP_NAME} PRIVATE ../multiplot-lib)

target_link_libraries(${APP_NAME} LINK_PRIVATE qwt)
target_link_libraries(${APP_NAME} LINK_PRIVATE Qt5::Core)
target_link_libraries(${APP_NAME} LINK_PRIVATE Qt5::Gui)
target_link_libraries(${APP_NAME} LINK_PRIVATE Qt5::Widgets)

target_link_libraries(${APP_NAME} LINK_PRIVATE multiplot-lib)

if (WIN32)
    install(TARGETS ${APP_NAME} DESTINATION ${INSTALL_DIRECTORY})
    install(CODE "execute_process(COMMAND cmd /c ${CMAKE_SOURCE_DIR}/windeployqt.bat)")
else ()
    install(TARGETS ${APP_NAME} DESTINATION ${INSTALL_DIRECTORY})
endif ()
