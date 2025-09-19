set(APP_NAME multiplot-lib)

set(CMAKE_AUTOMOC ON)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(Qt5 REQUIRED Core Gui Widgets LinguistTools)
# find_package(Qt5 REQUIRED Core Gui Widgets)

set(HEADERS
    multiplot.hpp
    multiplotdatamodel.hpp
    comtraderecord.hpp
    src/comtradeconfig.hpp
    src/multiplotpainter.hpp
    src/multiplotwidget.hpp
    src/multiplotcontrols.hpp
    src/multiplotmodel.hpp
    src/multiplotcontroller.hpp
    src/phases.hpp
        )
set(SOURCES
    src/multiplot.cpp
    src/comtradeconfig.cpp
    src/comtraderecord.cpp
    src/multiplotpainter.cpp
    src/multiplotwidget.cpp
    src/multiplotcontrols.cpp
    src/multiplotmodel.cpp
    src/multiplotcontroller.cpp
    src/phases.cpp
        )

set(TRANSLATIONS_TS res/translation/multiplot-lib_ru.ts)

# QT5_ADD_TRANSLATION(TRANSLATIONS ${TRANSLATIONS_TS})
qt5_add_translation(TRANSLATIONS ${TRANSLATIONS_TS})

add_custom_target(multiplot-lib_translations ALL DEPENDS ${TRANSLATIONS})
add_custom_target(multiplot-lib_resources ALL DEPENDS ${RESOURCES})

add_dependencies(multiplot-lib_translations multiplot-lib_resources)

configure_file(res/translation.qrc multiplot-lib_translation.qrc COPYONLY)

set(RESOURCES multiplot-lib_translation.qrc
)

# add_library(${APP_NAME} SHARED ${SOURCES} ${HEADERS} ${RESOURCES} ${TRANSLATIONS} )

add_library(${APP_NAME} SHARED ${SOURCES} ${HEADERS})

#target_include_directories(${APP_NAME} INTERFACE ../.. PRIVATE ../../libs ./src ../../external)
target_include_directories(${APP_NAME} PRIVATE ${QWT_INCLUDE_DIR})
target_include_directories(${APP_NAME} PRIVATE ./src)
# target_include_directories(${APP_NAME} PRIVATE  ../qwt/src)

target_link_libraries(${APP_NAME} LINK_PRIVATE qwt)
target_link_libraries(${APP_NAME} LINK_PRIVATE Qt5::Core)
target_link_libraries(${APP_NAME} LINK_PRIVATE Qt5::Gui)
target_link_libraries(${APP_NAME} LINK_PRIVATE Qt5::Widgets)

if (WIN32)
    install(TARGETS ${APP_NAME} DESTINATION ${INSTALL_DIRECTORY})
    install(CODE "execute_process(COMMAND cmd /c ${CMAKE_SOURCE_DIR}/windeployqt.bat)")
else ()
    install(TARGETS ${APP_NAME} DESTINATION ${INSTALL_DIRECTORY})
endif ()
