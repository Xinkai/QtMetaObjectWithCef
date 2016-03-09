cmake_minimum_required(VERSION 3.2)
#set(CMAKE_CXX_COMPILER /usr/bin/clang++)
#set(CMAKE_C_COMPILER /usr/bin/clang)

project(QtMetaObjectWithCef)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Werror")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror")
set(CMAKE_CXX_LINK_EXECUTABLE "${CMAKE_CXX_LINK_EXECUTABLE} -ldl")

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(Qt5Core REQUIRED)
find_package(Qt5Widgets REQUIRED)

set(SOURCE_FILES
    cef/Shell.h cef/Shell.cpp
    cef/ClientApp.h cef/ClientApp.cpp
    cef/ClientHandler.h cef/ClientHandler.cpp
    cef/RenderProcessHandler.h cef/RenderProcessHandler.cpp
    cef/SchemeHandlerFactory.h cef/SchemeHandlerFactory.cpp
    cef/AsyncMethodHandler.h cef/AsyncMethodHandler.cpp
)

set(BRIDGE_FILES
    bridge/Bridge.h bridge/Bridge.cpp
    bridge/SubModule.cpp bridge/SubModule.h
)

set(WEB_FILES
    wwwroot/wwwroot.qrc
    cef/cef.qrc
)

set(LINK_LIBS
    Qt5::Widgets
    Qt5::Gui
)


add_executable(Application
    main.cpp
    ${SOURCE_FILES}
    ${BRIDGE_FILES}
    ${WEB_FILES}
)

add_executable(QtMoExtractor
    cef/QtMoExtractor.cpp
    ${BRIDGE_FILES}
)

target_link_libraries(QtMoExtractor ${LINK_LIBS})


set(CEF_TARGET "Application")

# Logical target used to link the libcef library.
ADD_LOGICAL_TARGET("libcef_lib" "${CEF_LIB_DEBUG}" "${CEF_LIB_RELEASE}")

set(CEF_TARGET_OUT_DIR "${CMAKE_CURRENT_BINARY_DIR}")

# Determine the target output directory.
#SET_CEF_TARGET_OUT_DIR()

if(OS_LINUX)
    # Executable target.
    add_dependencies(${CEF_TARGET} libcef_dll_wrapper)
    target_link_libraries(${CEF_TARGET}
        libcef_lib
        libcef_dll_wrapper
        ${CEF_STANDARD_LIBS}
        ${LINK_LIBS}
    )

    # Set rpath so that libraries can be placed next to the executable.
    set_target_properties(${CEF_TARGET} PROPERTIES INSTALL_RPATH "$ORIGIN")
    set_target_properties(${CEF_TARGET} PROPERTIES BUILD_WITH_INSTALL_RPATH TRUE)
    set_target_properties(${CEF_TARGET} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CEF_TARGET_OUT_DIR})

    # Copy binary and resource files to the target output directory.
    COPY_FILES("${CEF_TARGET}" "${CEF_BINARY_FILES}" "${CEF_BINARY_DIR}" "${CEF_TARGET_OUT_DIR}")
    COPY_FILES("${CEF_TARGET}" "${CEF_RESOURCE_FILES}" "${CEF_RESOURCE_DIR}" "${CEF_TARGET_OUT_DIR}")

    # Set SUID permissions on the chrome-sandbox target.
    SET_LINUX_SUID_PERMISSIONS("${CEF_TARGET}" "${CEF_TARGET_OUT_DIR}/chrome-sandbox")
endif()
