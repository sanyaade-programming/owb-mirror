##################################################
# Types support part.                            #
##################################################

set(TYPES_DIR ${BAL_DIR}/Types/WebCore)

if(USE_GRAPHICS_SDL)
    create_include_link(${TYPES_DIR}/SDL/BCIconSDL.h BAL/Icon.h)
endif(USE_GRAPHICS_SDL)

create_include_link(${TYPES_DIR}/WK/BCDeprecatedPtrListWK.h BAL/DeprecatedPtrList.h)
create_include_link(${TYPES_DIR}/WK/BCDeprecatedPtrListImplWK.h BAL/DeprecatedPtrListImpl.h)
create_include_link(${TYPES_DIR}/WK/BCHTTPHeaderMapWK.h BAL/HTTPHeaderMap.h)
create_include_link(${TYPES_DIR}/WK/BCIconWK.h BAL/Icon.h)
create_include_link(${TYPES_DIR}/WK/BCKURLWK.h BAL/KURL.h)
create_include_link(${TYPES_DIR}/WK/BCKURLHashWK.h BAL/KURLHash.h)
create_include_link(${TYPES_DIR}/WK/BCLengthWK.h BAL/Length.h)
create_include_link(${TYPES_DIR}/WK/BCLengthBoxWK.h BAL/LengthBox.h)
create_include_link(${TYPES_DIR}/WK/BCLengthSizeWK.h BAL/LengthSize.h)
create_include_link(${TYPES_DIR}/WK/BCLinkHashWK.h BAL/LinkHash.h)
create_include_link(${TYPES_DIR}/WK/BCPurgeableBufferWK.h BAL/PurgeableBuffer.h)
create_include_link(${TYPES_DIR}/WK/BCScrollTypesWK.h BAL/ScrollTypes.h)
create_include_link(${TYPES_DIR}/WK/BCSharedBufferWK.h BAL/SharedBuffer.h)
create_include_link(${TYPES_DIR}/WK/BCThreadTimersWK.h BAL/ThreadTimers.h)
create_include_link(${TYPES_DIR}/WK/BCTreeSharedWK.h BAL/TreeShared.h)

aux_source_directory(${TYPES_DIR}/WK WEBCORE_SRC)
aux_source_directory(${TYPES_DIR}/Common WEBCORE_SRC)

if(USE_GRAPHICS_GTK)
    aux_source_directory(${TYPES_DIR}/Gtk WEBCORE_SRC)
endif(USE_GRAPHICS_GTK)

if(USE_GRAPHICS_QT)
    aux_source_directory(${TYPES_DIR}/Qt WEBCORE_SRC)
endif(USE_GRAPHICS_QT)

if(USE_GRAPHICS_SDL)
    create_include_link(${TYPES_DIR}/SDL/BCIconSDL.h BAL/Icon.h)
    list(APPEND WEBCORE_SRC
        ${TYPES_DIR}/SDL/BCIconSDL.cpp
        ${TYPES_DIR}/SDL/BCSharedBufferSDL.cpp
    )
endif(USE_GRAPHICS_SDL)
