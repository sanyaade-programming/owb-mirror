list(APPEND WEBCORE_SRC
    bridge/runtime_array.cpp
    bridge/runtime_method.cpp
    bridge/runtime_object.cpp
    bridge/runtime_root.cpp
    bridge/runtime.cpp
    bridge/NP_jsobject.cpp
    bridge/c/c_class.cpp
    bridge/c/c_instance.cpp
    bridge/c/c_runtime.cpp
    bridge/c/c_utility.cpp
    bridge/npruntime.cpp
    bridge/bal/bal_class.cpp
    bridge/bal/bal_instance.cpp
    bridge/bal/bal_runtime.cpp)

#     ADD_EXECUTABLE (testbalbindings
#         bridge/testbalbindings.cpp)
#     TARGET_LINK_LIBRARIES (testbalbindings
#         jscore
#         generated-sources
#         webcore-owb
#         bal-events
#         bal-facilities
#         bal-fonts
#         bal-graphics
#         bal-imagedecoder
#         bal-internationalization
#         bal-media
#         bal-memory
#         bal-network
#         bal-types
#         bal-widgets
# 	${EXTRA_LDFLAGS}
#     )
# 
# TARGET_LINK_LIBRARIES (testbalbindings
#     ${XML2_LIBRARIES}
#     ${XSLT_LIBRARIES}
# )
# 
# IF (WEBKIT_USE_HTML_EXTENSION)
#     TARGET_LINK_LIBRARIES (testbalbindings
#         htmlext
#     )
# ENDIF (WEBKIT_USE_HTML_EXTENSION)

