LIST (APPEND XML_SRC
    xml/DOMParser.cpp
    xml/XMLHttpRequest.cpp
    xml/XMLHttpRequestUpload.cpp
    xml/XMLSerializer.cpp
)

IF (WEBKIT_ENABLE_XPATH)
    LIST (APPEND XML_SRC
        xml/NativeXPathNSResolver.cpp
        xml/XPathEvaluator.cpp
        xml/XPathExpression.cpp
        xml/XPathExpressionNode.cpp
        xml/XPathFunctions.cpp
        xml/XPathNamespace.cpp
        xml/XPathNodeSet.cpp
        xml/XPathNSResolver.cpp
        xml/XPathParser.cpp
        xml/XPathPath.cpp
        xml/XPathPredicate.cpp
        xml/XPathResult.cpp
        xml/XPathStep.cpp
        xml/XPathUtil.cpp
        xml/XPathValue.cpp
        xml/XPathVariableReference.cpp
    )
ENDIF (WEBKIT_ENABLE_XPATH)

IF (WEBKIT_ENABLE_XSLT)
    LIST (APPEND XML_SRC
        xml/XSLImportRule.cpp
        xml/XSLStyleSheet.cpp
        xml/XSLTExtensions.cpp
        xml/XSLTProcessor.cpp
        xml/XSLTUnicodeSort.cpp
    )
ENDIF (WEBKIT_ENABLE_XSLT)

