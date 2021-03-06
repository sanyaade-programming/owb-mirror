execute_process(
    COMMAND mkdir -p include
    WORKING_DIRECTORY ${OWB_BINARY_DIR}
    ERROR_QUIET
)
set(API_INCLUDE_DIRS ${OWB_BINARY_DIR}/include)
foreach(files ${API_HEADER})
    get_filename_component(filename ${files} NAME)
    configure_file(
        ${files}
        ${API_INCLUDE_DIRS}/${filename}
        COPYONLY
    )
endforeach(files)

include_directories(
    ${API_INCLUDE_DIRS}
    ${EXTERNAL_DEPS_INCLUDE_DIRS}
)
#FIXME
link_directories(${LIBRARY_OUTPUT_PATH})
if(USE_GRAPHICS_GTK)
    add_executable(owb EXCLUDE_FROM_ALL gtk/main.cpp)
endif(USE_GRAPHICS_GTK)

if(USE_GRAPHICS_QT)
    list(APPEND OWB_SRC
        Qt/main.cpp
    )
    owb_generate_moc(OWB_SRC ${OWB_SOURCE_DIR}/WebKitTools/OWBLauncher/Qt/main.cpp main.moc)
    add_executable(owb EXCLUDE_FROM_ALL ${OWB_SRC})
endif(USE_GRAPHICS_QT)

if(USE_GRAPHICS_SDL)
    add_executable(owb EXCLUDE_FROM_ALL SDL/main.cpp)
endif(USE_GRAPHICS_SDL)

include(Customer/CMakeLists.txt OPTIONAL)

target_link_libraries(owb
    webkit-owb
    ${GRAPHICS_LIBRARIES}
)

if (ENABLE_ORIGYNSUITE)
    target_link_libraries(owb
        origynsuite
    )
endif (ENABLE_ORIGYNSUITE)


if(ENABLE_TESTS)
    add_executable(WebViewTests Tests/WebViewTests.cpp)
    target_link_libraries(WebViewTests
        webkit-owb
        ${GRAPHICS_LIBRARIES}
        ${CPPUNIT_LIBRARIES}
    )
    ADD_TEST(WebViewTests ${OWB_BINARY_DIR}/bin/WebViewTests)

    add_executable(WebFrameTests Tests/WebFrameTests.cpp)
    target_link_libraries(WebFrameTests
        webkit-owb
        ${GRAPHICS_LIBRARIES}
        ${CPPUNIT_LIBRARIES}
    )
    ADD_TEST(WebFrameTests ${OWB_BINARY_DIR}/bin/WebFrameTests)

    if(USE_GRAPHICS_SDL)
        add_executable(WebViewPlatformTestsCommon Tests/SDL/WebViewPlatformTests.cpp Tests/WebViewPlatformTestsCommon.cpp)
    endif(USE_GRAPHICS_SDL)

    if(USE_GRAPHICS_GTK)
        add_executable(WebViewPlatformTestsCommon Tests/Gtk/WebViewPlatformTests.cpp Tests/WebViewPlatformTestsCommon.cpp)
    endif(USE_GRAPHICS_GTK)

    target_link_libraries(WebViewPlatformTestsCommon
        webkit-owb
        ${GRAPHICS_LIBRARIES}
        ${CPPUNIT_LIBRARIES}
    )
    ADD_TEST(WebViewPlatformTestsCommon ${OWB_BINARY_DIR}/bin/WebViewPlatformTestsCommon)

endif(ENABLE_TESTS)

# IF (WEBKIT_USE_HTML_EXTENSION)
#     TARGET_LINK_LIBRARIES (owb
#         htmlext
#     )
# ENDIF (WEBKIT_USE_HTML_EXTENSION)
# 
# INSTALL (TARGETS owb 
#     RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin
# )
