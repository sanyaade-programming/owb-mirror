##################################################
# Generate lut for JavaScriptCore and for        #
# bindings/js in WebCore.                        #
##################################################

set(CREATE_HASH_TABLE ${OWB_SOURCE_DIR}/JavaScriptCore/create_hash_table)
macro(create_lut_jsc _srcs_LIST _in_FILE _out_FILE _dep_FILE)
    get_filename_component(_dirname ${_out_FILE} PATH)
    add_custom_command(
        OUTPUT ${OWB_BINARY_DIR}/${_out_FILE}
        COMMAND mkdir -p ${OWB_BINARY_DIR}/${_dirname}
        COMMAND ${PERL_EXECUTABLE} ${CREATE_HASH_TABLE} ${OWB_SOURCE_DIR}/${_in_FILE} -i > ${OWB_BINARY_DIR}/${_out_FILE}
        DEPENDS ${OWB_SOURCE_DIR}/${_in_FILE} ${OWB_SOURCE_DIR}/${_dep_FILE}
    )
    list(APPEND ${_srcs_LIST}
        ${OWB_BINARY_DIR}/${_out_FILE}
    )
endmacro(create_lut_jsc)

macro(create_lut_webcore _srcs_LIST _in_FILE _out_FILE _dep_FILE)
    get_filename_component(_dirname ${_out_FILE} PATH)
    add_custom_command(
        OUTPUT ${OWB_BINARY_DIR}/${_out_FILE}
        COMMAND mkdir -p ${OWB_BINARY_DIR}/${_dirname}
        COMMAND ${PERL_EXECUTABLE} ${CREATE_HASH_TABLE} ${OWB_SOURCE_DIR}/${_in_FILE} -n WebCore > ${OWB_BINARY_DIR}/${_out_FILE}
        DEPENDS ${OWB_SOURCE_DIR}/${_in_FILE} ${OWB_SOURCE_DIR}/${_dep_FILE}
    )
    list(APPEND ${_srcs_LIST}
        ${OWB_BINARY_DIR}/${_out_FILE}
    )
endmacro(create_lut_webcore)

macro(create_lexer _srcs_LIST _in_FILE _out_FILE _dep_FILE)
    get_filename_component(_dirname ${_out_FILE} PATH)
    add_custom_command(
        OUTPUT ${OWB_BINARY_DIR}/${_out_FILE}
        COMMAND mkdir -p ${OWB_BINARY_DIR}/${_dirname}
        COMMAND ${PERL_EXECUTABLE} ${CREATE_HASH_TABLE} ${OWB_SOURCE_DIR}/${_in_FILE} > ${OWB_BINARY_DIR}/${_out_FILE}
        DEPENDS ${OWB_SOURCE_DIR}/${_in_FILE} ${OWB_SOURCE_DIR}/${_dep_FILE}
    )
    list(APPEND ${_srcs_LIST}
        ${OWB_BINARY_DIR}/${_out_FILE}
    )
endmacro(create_lexer)

macro(create_cpp_lut _in_FILE _out_FILE _dep_FILE)
    get_filename_component(_basename ${_out_FILE} NAME_WE)
    add_custom_command(
        OUTPUT ${OWB_BINARY_DIR}/${_out_FILE}
        COMMAND mkdir -p ${OWB_BINARY_DIR}/${_dirname}
        COMMAND ${PERL_EXECUTABLE} ${CREATE_HASH_TABLE} ${OWB_SOURCE_DIR}/${_in_FILE} > ${OWB_SOURCE_DIR}/${_out_FILE}
        DEPENDS ${OWB_SOURCE_DIR}/${_in_FILE} ${OWB_SOURCE_DIR}/${_dep_FILE}
    )
endmacro(create_cpp_lut)

##################################################
# Create cpp file from idl.                      #
##################################################

list(APPEND SCRIPTS_BINDINGS
    ${OWB_SOURCE_DIR}/WebCore/bindings/scripts/CodeGeneratorJS.pm
    ${OWB_SOURCE_DIR}/WebCore/bindings/scripts/CodeGenerator.pm
    ${OWB_SOURCE_DIR}/WebCore/bindings/scripts/IDLParser.pm
    ${OWB_SOURCE_DIR}/WebCore/bindings/scripts/IDLStructure.pm
    ${OWB_SOURCE_DIR}/WebCore/bindings/scripts/InFilesParser.pm
    ${OWB_SOURCE_DIR}/WebCore/bindings/scripts/generate-bindings.pl)
macro(generate_sources_from_idl _target)
    foreach(idl ${IDL_SRC})
        get_filename_component(_basename ${idl} NAME_WE)
        add_custom_command(
            OUTPUT ${OWB_BINARY_DIR}/generated_sources/WebCore/JS${_basename}.cpp ${OWB_BINARY_DIR}/generated_sources/WebCore/JS${_basename}.h
            COMMAND mkdir -p ${OWB_BINARY_DIR}/generated_sources/WebCore/
            COMMAND ${PERL_EXECUTABLE} -I${OWB_SOURCE_DIR}/WebCore/bindings/scripts ${OWB_SOURCE_DIR}/WebCore/bindings/scripts/generate-bindings.pl  --defines \"${FEATURE_DEFINES_JAVASCRIPT}\" --generator JS --include ${OWB_SOURCE_DIR}/WebCore/dom --include ${OWB_SOURCE_DIR}/WebCore/html --include ${OWB_SOURCE_DIR}/WebCore/xml --include ${OWB_SOURCE_DIR}/WebCore/svg --outputdir ${OWB_BINARY_DIR}/generated_sources/WebCore ${OWB_SOURCE_DIR}/WebCore/${idl}
            DEPENDS ${SCRIPTS_BINDINGS}
            )
        list(APPEND ${_target} 
            ${OWB_BINARY_DIR}/generated_sources/WebCore/JS${_basename}.cpp
            ${OWB_BINARY_DIR}/generated_sources/WebCore/JS${_basename}.h)
    endforeach(idl)
endmacro(generate_sources_from_idl)


##################################################
# Create cpp file from idl.                      #
##################################################

macro(create_generated_cpp_helper _inlName)
    get_filename_component(_baseName ${_inlName} NAME_WE)
    if(NOT EXISTS ${OWB_BINARY_DIR}/generated_sources/WebCore/${_baseName}.cpp)
        file(WRITE ${OWB_BINARY_DIR}/generated_sources/WebCore/${_baseName}.cpp
"/* This file is autogenerated by CMake.                                       *
 * Do not edit, changes will be lost.                                         *
 * With the CMake buildsystem giving generated-but-not-compiled-on-their-own  *
 * files the suffix \".cpp\" is not recommended, because it breaks the          *
 * dependency handling. So the actual generated file has the suffix \".inl\"    *
 * and this file here only exists as a helper.                                */
#include \"${OWB_BINARY_DIR}/${_inlName}\"\n")
    endif(NOT EXISTS ${OWB_BINARY_DIR}/generated_sources/WebCore/${_baseName}.cpp)
    list(APPEND WEBCORE_SRC 
        ${OWB_BINARY_DIR}/${_inlName})
endmacro(create_generated_cpp_helper _inlName)


##################################################
# Create symlink to ease include with OWBAL and  #
# WKAL.                                          #
##################################################

macro(create_include_link _src_dir _dst_dir)
    execute_process(
        COMMAND ${OWB_SOURCE_DIR}/BAL/Scripts/createLink.py ${CMAKE_CURRENT_SOURCE_DIR}/${_src_dir} generated_link/${_dst_dir}
        WORKING_DIRECTORY ${OWB_BINARY_DIR})
endmacro(create_include_link)

##################################################
# Create moc file                                #
##################################################

macro(owb_generate_moc _srcs_LIST _in_FILE _out_FILE)
    execute_process(
        COMMAND mkdir -p ${OWB_BINARY_DIR}/generated_sources/moc/
    )
    qt4_generate_moc(${_in_FILE} ${OWB_BINARY_DIR}/generated_sources/moc/${_out_FILE})
    list(APPEND ${_srcs_LIST} ${OWB_BINARY_DIR}/generated_sources/moc/${_out_FILE})
endmacro(owb_generate_moc)
