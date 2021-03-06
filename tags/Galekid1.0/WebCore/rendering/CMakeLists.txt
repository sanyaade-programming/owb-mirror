list(APPEND WEBCORE_SRC
    rendering/AutoTableLayout.cpp
    rendering/bidi.cpp
    rendering/break_lines.cpp
    rendering/CounterNode.cpp
    rendering/EllipsisBox.cpp
    rendering/FixedTableLayout.cpp
    rendering/HitTestResult.cpp
    rendering/InlineBox.cpp
    rendering/InlineFlowBox.cpp
    rendering/InlineTextBox.cpp
    rendering/LayoutState.cpp
    rendering/ListMarkerBox.cpp
    rendering/RenderApplet.cpp
    rendering/RenderArena.cpp
    rendering/RenderBR.cpp
    rendering/RenderBlock.cpp
    rendering/RenderBox.cpp
    rendering/RenderButton.cpp
    rendering/RenderContainer.cpp
    rendering/RenderCounter.cpp
    rendering/RenderFieldset.cpp
    rendering/RenderFileUploadControl.cpp
    rendering/RenderFlexibleBox.cpp
    rendering/RenderFlow.cpp
    rendering/RenderFrame.cpp
    rendering/RenderFrameSet.cpp
    rendering/RenderHTMLCanvas.cpp
    rendering/RenderImage.cpp
    rendering/RenderImageGeneratedContent.cpp
    rendering/RenderInline.cpp
    rendering/RenderLayer.cpp
    rendering/RenderLegend.cpp
    rendering/RenderListBox.cpp
    rendering/RenderListItem.cpp
    rendering/RenderListMarker.cpp
    rendering/RenderMarquee.cpp
    rendering/RenderMenuList.cpp
    rendering/RenderObject.cpp
    rendering/RenderPart.cpp
    rendering/RenderPartObject.cpp
    rendering/RenderReplaced.cpp
    rendering/RenderReplica.cpp
    rendering/RenderScrollbar.cpp
    rendering/RenderScrollbarPart.cpp
    rendering/RenderScrollbarTheme.cpp
    rendering/RenderSlider.cpp
    rendering/RenderTable.cpp
    rendering/RenderTableCell.cpp
    rendering/RenderTableCol.cpp
    rendering/RenderTableRow.cpp
    rendering/RenderTableSection.cpp
    rendering/RenderText.cpp
    rendering/RenderTextControl.cpp
    rendering/RenderTextFragment.cpp
    rendering/RenderTheme.cpp
    rendering/RenderTreeAsText.cpp
    rendering/RenderView.cpp
    rendering/RenderWidget.cpp
    rendering/RenderWordBreak.cpp
    rendering/RootInlineBox.cpp
    rendering/TextControlInnerElements.cpp
)

if(ENABLE_SVG)
    list(APPEND WEBCORE_SRC
        rendering/PointerEventsHitRules.cpp
        rendering/RenderForeignObject.cpp
        rendering/RenderPath.cpp
        rendering/RenderSVGBlock.cpp
        rendering/RenderSVGContainer.cpp
        rendering/RenderSVGGradientStop.cpp
        rendering/RenderSVGHiddenContainer.cpp
        rendering/RenderSVGImage.cpp
        rendering/RenderSVGInline.cpp
        rendering/RenderSVGInlineText.cpp
        rendering/RenderSVGRoot.cpp
        rendering/RenderSVGTSpan.cpp
        rendering/RenderSVGText.cpp
        rendering/RenderSVGTextPath.cpp
        rendering/RenderSVGTransformableContainer.cpp
        rendering/RenderSVGViewportContainer.cpp
        rendering/SVGCharacterLayoutInfo.cpp
        rendering/SVGInlineFlowBox.cpp
        rendering/SVGInlineTextBox.cpp
        rendering/SVGRenderSupport.cpp
        rendering/SVGRenderTreeAsText.cpp
        rendering/SVGRootInlineBox.cpp
    )
endif(ENABLE_SVG)

if(ENABLE_VIDEO)
    list(APPEND WEBCORE_SRC
        rendering/MediaControlElements.cpp
        rendering/RenderMedia.cpp
        rendering/RenderVideo.cpp
    )
endif(ENABLE_VIDEO)
