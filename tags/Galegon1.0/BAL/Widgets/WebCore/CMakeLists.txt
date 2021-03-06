##################################################
# Widgets support part.                          #
##################################################

set(WIDGETS_DIR ${BAL_DIR}/Widgets/WebCore)

create_include_link(${WIDGETS_DIR}/WK/BCCursorWK.h BAL/Cursor.h)
create_include_link(${WIDGETS_DIR}/WK/BCContextMenuWK.h BAL/ContextMenu.h)
create_include_link(${WIDGETS_DIR}/WK/BCContextMenuItemWK.h BAL/ContextMenuItem.h)
create_include_link(${WIDGETS_DIR}/WK/BCHostWindowWK.h BAL/HostWindow.h)
create_include_link(${WIDGETS_DIR}/WK/BCPasteboardWK.h BAL/Pasteboard.h)
create_include_link(${WIDGETS_DIR}/WK/BCPlatformMenuDescriptionWK.h BAL/PlatformMenuDescription.h)
create_include_link(${WIDGETS_DIR}/WK/BCPlatformScreenWK.h BAL/PlatformScreen.h)
create_include_link(${WIDGETS_DIR}/WK/BCPopupMenuWK.h BAL/PopupMenu.h)
create_include_link(${WIDGETS_DIR}/WK/BCPopupMenuClientWK.h BAL/PopupMenuClient.h)
create_include_link(${WIDGETS_DIR}/WK/BCPopupMenuStyleWK.h BAL/PopupMenuStyle.h)
create_include_link(${WIDGETS_DIR}/WK/BCScrollbarWK.h BAL/Scrollbar.h)
create_include_link(${WIDGETS_DIR}/WK/BCScrollbarClientWK.h BAL/ScrollbarClient.h)
create_include_link(${WIDGETS_DIR}/WK/BCScrollbarThemeWK.h BAL/ScrollbarTheme.h)
create_include_link(${WIDGETS_DIR}/WK/BCScrollbarThemeCompositeWK.h BAL/ScrollbarThemeComposite.h)
create_include_link(${WIDGETS_DIR}/WK/BCScrollViewWK.h BAL/ScrollView.h)
create_include_link(${WIDGETS_DIR}/WK/BCSearchPopupMenuWK.h BAL/SearchPopupMenu.h)
create_include_link(${WIDGETS_DIR}/WK/BCThemeWK.h BAL/Theme.h)
create_include_link(${WIDGETS_DIR}/WK/BCThemeTypesWK.h BAL/ThemeTypes.h)
create_include_link(${WIDGETS_DIR}/WK/BCWidgetWK.h BAL/Widget.h)

if(USE_GRAPHICS_GTK)
    create_include_link(${WIDGETS_DIR}/Gtk/BCClipboardGtk.h BAL/ClipboardGtk.h)
    create_include_link(${WIDGETS_DIR}/Gtk/BCgtkdrawingGtk.h BAL/gtkdrawing.h)
    create_include_link(${WIDGETS_DIR}/Gtk/PasteboardHelperGtk.h BAL/PasteboardHelperGtk.h)
    create_include_link(${WIDGETS_DIR}/Gtk/BCPasteboardHelperGtk.h BAL/PasteboardHelper.h)
    create_include_link(${WIDGETS_DIR}/Gtk/BCRenderThemeGtk.h BAL/RenderThemeGtk.h)
    create_include_link(${WIDGETS_DIR}/Gtk/BCScrollbarGtk.h BAL/ScrollbarGtk.h)
    create_include_link(${WIDGETS_DIR}/Gtk/BCScrollbarThemeGtk.h BAL/ScrollbarThemeGtk.h)
    
    aux_source_directory(${WIDGETS_DIR}/Gtk WEBCORE_SRC)
endif(USE_GRAPHICS_GTK)

if(USE_GRAPHICS_QT)
    create_include_link(${WIDGETS_DIR}/Qt/BCClipboardQt.h BAL/ClipboardQt.h)
    create_include_link(${WIDGETS_DIR}/Qt/BCRenderThemeQt.h BAL/RenderThemeQt.h)
    create_include_link(${WIDGETS_DIR}/Qt/BCScrollbarThemeQt.h BAL/ScrollbarThemeQt.h)

    aux_source_directory(${WIDGETS_DIR}/Qt WEBCORE_SRC)
    owb_generate_moc(WEBCORE_SRC ${OWB_SOURCE_DIR}/BAL/Widgets/WebCore/Qt/QWebPopup.h moc_QWebPopup.cpp)
endif(USE_GRAPHICS_QT)

if(USE_GRAPHICS_SDL)
    create_include_link(${WIDGETS_DIR}/SDL/BCContextMenuSDL.h BAL/ContextMenu.h)

    create_include_link(${WIDGETS_DIR}/SDL/BCClipboardSDL.h BAL/ClipboardSDL.h)
    create_include_link(${WIDGETS_DIR}/SDL/BCContextMenuItemSDL.h BAL/ContextMenuItem.h)
    create_include_link(${WIDGETS_DIR}/SDL/BCCursorSDL.h BAL/Cursor.h)
    create_include_link(${WIDGETS_DIR}/SDL/BCPasteboardSDL.h BAL/Pasteboard.h)
    create_include_link(${WIDGETS_DIR}/SDL/PasteboardHelperSDL.h BAL/PasteboardHelperSDL.h)
    create_include_link(${WIDGETS_DIR}/SDL/BCPasteboardHelperSDL.h BAL/PasteboardHelper.h)
    create_include_link(${WIDGETS_DIR}/SDL/BCPlatformMenuDescriptionSDL.h BAL/PlatformMenuDescription.h)
    create_include_link(${WIDGETS_DIR}/SDL/BCPlatformScreenSDL.h BAL/PlatformScreen.h)
    create_include_link(${WIDGETS_DIR}/SDL/BCPopupMenuSDL.h BAL/PopupMenu.h)
    create_include_link(${WIDGETS_DIR}/SDL/BCRenderThemeSDL.h BAL/RenderThemeSDL.h)
    create_include_link(${WIDGETS_DIR}/SDL/BCScrollbarThemeSDL.h BAL/ScrollbarThemeSDL.h)
    create_include_link(${WIDGETS_DIR}/SDL/BCScrollViewSDL.h BAL/ScrollView.h)
    create_include_link(${WIDGETS_DIR}/SDL/BCSearchPopupMenuSDL.h BAL/SearchPopupMenu.h)

    aux_source_directory(${WIDGETS_DIR}/SDL WEBCORE_SRC)
endif(USE_GRAPHICS_SDL)

aux_source_directory(${WIDGETS_DIR}/WK WEBCORE_SRC)
