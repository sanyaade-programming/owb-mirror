include(${OWB_SOURCE_DIR}/BAL/Graphics/WebKit/CMakeLists.txt)
include(${OWB_SOURCE_DIR}/BAL/Widgets/WebKit/CMakeLists.txt)

if(ENABLE_CEHTML)
    include(${OWB_SOURCE_DIR}/BAL/CEHTML/WebKit/CMakeLists.txt)
    aux_source_directory(Api/CEHTML WEBKIT_SRC)
endif(ENABLE_CEHTML)

if(ENABLE_DAE)
    include(${OWB_SOURCE_DIR}/BAL/DAE/WebKit/CMakeLists.txt)
    add_custom_command(
        OUTPUT ${CMAKE_BINARY_DIR}/generated_sources/WebKit/VKValuesJavaScript.cpp
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        COMMAND mkdir -p generated_sources/WebKit
        COMMAND
        ${CMAKE_SOURCE_DIR}/WebKit/OrigynWebBrowser/DAE/VKParserJavaScript.py ${CMAKE_SOURCE_DIR}/BAL/CEHTML/WebCore/Generic/BCKeyboardCodesCEHTMLGeneric.h
 > ${CMAKE_BINARY_DIR}/generated_sources/WebKit/VKValuesJavaScript.cpp
        DEPENDS ${CMAKE_SOURCE_DIR}/WebKit/OrigynWebBrowser/DAE/VKParserJavaScript.py ${CMAKE_SOURCE_DIR}/BAL/CEHTML/WebCore/Generic/BCKeyboardCodesCEHTMLGeneric.h
    )

    list(APPEND WEBKIT_SRC
        ${CMAKE_BINARY_DIR}/generated_sources/WebKit/VKValuesJavaScript.cpp
    )
    aux_source_directory(Api/DAE WEBKIT_SRC)

    if(ENABLE_DAE_TUNER AND ENABLE_DAE_APPLICATION)
        aux_source_directory(DAE/Video WEBKIT_SRC)
    endif(ENABLE_DAE_TUNER AND ENABLE_DAE_APPLICATION)

    if(ENABLE_DAE_APPLICATION)
        aux_source_directory(DAE/Application WEBKIT_SRC)
    endif(ENABLE_DAE_APPLICATION)

    if(ENABLE_DAE_DOWNLOAD)
        aux_source_directory(DAE/Download WEBKIT_SRC)
    endif(ENABLE_DAE_DOWNLOAD)

endif(ENABLE_DAE)

aux_source_directory(Api WEBKIT_SRC)
aux_source_directory(WebCoreSupport WEBKIT_SRC)

# Generate WebKitVersion.h
set(WEBKIT_GENERATED_DIR ${OWB_BINARY_DIR}/generated_sources/WebKit)
if(NOT EXISTS ${WEBKIT_GENERATED_DIR})
    execute_process(COMMAND mkdir ${WEBKIT_GENERATED_DIR})
endif(NOT EXISTS ${WEBKIT_GENERATED_DIR})

list(APPEND WEBKIT_SRC ${WEBKIT_GENERATED_DIR}/WebKitVersion.h)

set(WEBKIT_VERSION_SCRIPT ${OWB_SOURCE_DIR}/WebKit/scripts/generate-webkitversion.pl)
set(WEBKIT_VERSION_CONFIG ${OWB_SOURCE_DIR}/WebKit/mac/Configurations/Version.xcconfig)
add_custom_command(
    OUTPUT ${WEBKIT_GENERATED_DIR}/WebKitVersion.h
    COMMAND ${PERL_EXECUTABLE} ${WEBKIT_VERSION_SCRIPT} --config ${WEBKIT_VERSION_CONFIG} --outputDir ${WEBKIT_GENERATED_DIR}
    WORKING_DIRECTORY ${WEBKIT_GENERATED_DIR}
    DEPENDS ${WEBKIT_VERSION_SCRIPT} ${WEBKIT_VERSION_CONFIG}
)

if(NOT ENABLE_INSPECTOR)
    list(REMOVE_ITEM WEBKIT_SRC
        Api/WebInspector.cpp
        Api/WebScriptCallFrame.cpp
        Api/WebScriptDebugServer.cpp
        WebCoreSupport/WebInspectorClient.cpp
    )
endif(NOT ENABLE_INSPECTOR)

include_directories(
    ${BASE_INCLUDE_DIRS}
    ${WTF_INCLUDE_DIRS}
    ${BAL_INCLUDE_DIRS}
    ${JAVASCRIPTCORE_INCLUDE_DIRS}
    ${WEBCORE_INCLUDE_DIRS}
    ${WEBKIT_INCLUDE_DIRS}
    ${WEBKIT_GENERATED_DIR}
    ${OWB_BINARY_DIR}/generated_sources/WebCore
    ${OWB_BINARY_DIR}/generated_sources/WebKit
    ${OWB_BINARY_DIR}/generated_sources/WebCore/html
    ${EXTERNAL_DEPS_INCLUDE_DIRS}
)

IF(NOT WIN32)
    include(AddWebKitCompilerFlags)
ELSE(NOT WIN32)
    ## We had an issues with link under windows so we put directly the target of it (AddWebCoreCompilerFlags)
    include(AddWebCoreCompilerFlags)
ENDIF(NOT WIN32)

set_source_files_properties(${WEBKIT_SRC}
    PROPERTIES COMPILE_FLAGS "-fno-rtti -fno-exceptions"
)
add_library(webkit-owb SHARED ${WEBKIT_SRC})
add_dependencies(webkit-owb webcore)

target_link_libraries(webkit-owb
    ${WEBKKIT_LD_FLAGS}
    ${EXTERNAL_DEPS_LIBRARIES}
)

target_link_libraries(webkit-owb LINK_INTERFACE_LIBRARIES)

##################################################
# webkit-owb installation.                       #
##################################################

set_target_properties(webkit-owb
    PROPERTIES SOVERSION ${WEBKIT_VERSION}
)
configure_file(
    ${OWB_SOURCE_DIR}/WebKit/OrigynWebBrowser/webkit-owb.pc.in
    ${OWB_SOURCE_DIR}/WebKit/OrigynWebBrowser/webkit-owb.pc
    @ONLY
)

IF(NOT WIN32)
    ## LIBRARY DESTINATION is useless with vcproj
    install(TARGETS webkit-owb
        LIBRARY DESTINATION lib
    )
ENDIF(NOT WIN32)

install(FILES ${API_HEADER}
    DESTINATION include/webkit-owb
    PERMISSIONS OWNER_READ GROUP_READ WORLD_READ
)
install(FILES ${OWB_SOURCE_DIR}/WebKit/OrigynWebBrowser/webkit-owb.pc
    DESTINATION lib/pkgconfig
    PERMISSIONS OWNER_READ GROUP_READ WORLD_READ
)
install(DIRECTORY ${OWB_SOURCE_DIR}/Resources/
    DESTINATION share/webkit-owb/resources
    PATTERN ".svn" EXCLUDE
    PERMISSIONS OWNER_READ GROUP_READ WORLD_READ
)
if(ENABLE_DAE_TUNER)
    install(FILES ${OWB_SOURCE_DIR}/WebKit/OrigynWebBrowser/DAE/Video/ChannelConfig.dtd
        DESTINATION ${WITH_OWB_CONFIG_DIR}
        PERMISSIONS OWNER_READ GROUP_READ WORLD_READ
    )
endif(ENABLE_DAE_TUNER)
