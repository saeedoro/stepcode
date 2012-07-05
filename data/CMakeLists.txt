# To build one or more schemas, configure with
# 'cmake -DSCL_BUILD_SCHEMAS="path/to/schema.exp;path/to/schema2.exp"

# This function runs fedex on one express file. The generated source goes in a subdir of
# the build dir, along with a CMakeLists.txt that fedex_plus writes. We do this because
# the file names are not known until compile time. As a result, each sdai lib is treated
# as a separate project. In this subproject, the source is compiled into a library. A
# p21read executable is also compiled and linked to the lib, and is then used to test the lib.
FUNCTION(BUILD_A_SCHEMA SCHEMA_FILE)
    if( EXISTS "${CMAKE_BINARY_DIR}/${SCHEMA_FILE}" )  #try absolute path. if that fails, must already be absolute.
        set( SCHEMA_FILE "${CMAKE_BINARY_DIR}/${SCHEMA_FILE}" )
    else()
        if( NOT EXISTS ${SCHEMA_FILE} )
            message( FATAL_ERROR "Cannot find ${CMAKE_BINARY_DIR}/${SCHEMA_FILE} or ${SCHEMA_FILE}")
        endif()
    endif()

    if( IS_DIRECTORY ${SCHEMA_FILE} ) #if it is a dir, look for one .exp file inside
        file(GLOB SCHEMA_FILE ${SCHEMA_FILE}/*.exp )
    endif()

    if( NOT EXISTS ${SCHEMA_FILE} )
        message(FATAL_ERROR "Expected one express file. Found '${SCHEMA_FILE}' instead.")
    endif()

    # read the schema name from a line like 'SCHEMA AUTOMOTIVE_DESIGN;'
    file(STRINGS ${SCHEMA_FILE} SCHEMA_STATEMENT LIMIT_COUNT 1 REGEX "^SCHEMA .*")
    string(REGEX REPLACE "^SCHEMA \(.*\)\;$" "\\1" SCHEMA_N ${SCHEMA_STATEMENT} )
    string(TOUPPER ${SCHEMA_N} SCHEMA_LONG_NAME) #fedex_plus always uses upper case for file names
    get_filename_component( SCHEMA_SN ${SCHEMA_FILE} NAME )
    string( REGEX REPLACE "\(.*\).[Ee][Xx][Pp]" "\\1" SCHEMA_SHORT_NAME ${SCHEMA_SN} )

    project( sdai_${SCHEMA_SHORT_NAME} )
    message( STATUS "Generating code for ${SCHEMA_SHORT_NAME}.")
    set( SCHEMA_OUTPUT_DIR ${CMAKE_BINARY_DIR}/${SCHEMA_SHORT_NAME} )

    set( ${PROJECT_NAME}_CMAKELISTS ${SCHEMA_OUTPUT_DIR}/CMakeLists.txt )

    #ALL option for add_custom_target() - don't include these targets in 'all' if testing is enabled.
    # this is the equivalent of the inverse of EXCLUDE_FROM_ALL
    if( SCL_ENABLE_TESTING )
        unset( BUILD_IN_TARGET_ALL )
    else()
        set( BUILD_IN_TARGET_ALL ALL )
    endif()

    # SC_SDAI_EXES_SRC: list of independent c++ files, which can be individually passed to SCL_ADDEXEC() and linked to an sdai lib
    if( NOT DEFINED SC_SDAI_EXES_SRC )
        set( SC_SDAI_EXES_SRC "${SCL_SOURCE_DIR}/src/test/p21read/p21read.cc" CACHE STRING "Semicolon-delimited list of c++ files; each will become an executable that is linked to each schema built" )
    endif( NOT DEFINED SC_SDAI_EXES_SRC )


    #creates the directory where fedex_plus will run
    add_custom_command( OUTPUT ${SCHEMA_OUTPUT_DIR}
                        COMMAND cmake ARGS -E make_directory ${SCHEMA_OUTPUT_DIR})
    #run fedex_plus, which writes the source as well as a CMakeLists.txt
    add_custom_command( OUTPUT ${SCHEMA_OUTPUT_DIR}/CMakeLists.txt
                        COMMAND fedex_plus ARGS ${SCHEMA_FILE}
                        DEPENDS ${SCHEMA_FILE} ${SCHEMA_OUTPUT_DIR} fedex_plus ${SCHEMA_SHORT_NAME}_cmakelists_in
                        WORKING_DIRECTORY ${SCHEMA_OUTPUT_DIR}
                        COMMENT "Running fedex_plus for ${SCHEMA_SHORT_NAME}..."
                        VERBATIM )
    add_custom_command( OUTPUT ${SCHEMA_SHORT_NAME}_cmakelists_in COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/CMakeLists.in ${SCHEMA_OUTPUT_DIR}/CMakeLists.txt )
    #building this target triggers execution of the above commands
    add_custom_target( generate_cpp_${PROJECT_NAME} ${BUILD_IN_TARGET_ALL}
                       DEPENDS ${SCHEMA_OUTPUT_DIR}/CMakeLists.txt version_string )
    #run cmake to configure the subproject. ${PROJECT_NAME}_CONFIGURED is a dummy file that doesn't actually exist.
    #this passes several variables to the subproject; other vars can be found in the cache.
    add_custom_command( OUTPUT ${PROJECT_NAME}_CONFIGURED
                        COMMAND ${CMAKE_COMMAND} ARGS ${SCHEMA_OUTPUT_DIR}
                                                      -DSDAI_LIB=${PROJECT_NAME} -DSDAI_LONG=${SCHEMA_LONG_NAME} -DSCL_CMAKE_DIR=${SCL_CMAKE_DIR}
                                                      -DBUILD_ROOT=${CMAKE_BINARY_DIR} -DSC_SDAI_EXES_SRC=${SC_SDAI_EXES_SRC}
                        WORKING_DIRECTORY ${SCHEMA_OUTPUT_DIR}
                        COMMENT "Configuring subproject for ${SCHEMA_SHORT_NAME}..."
                        VERBATIM )
    set_source_files_properties( ${PROJECT_NAME}_CONFIGURED PROPERTIES SYMBOLIC TRUE )
    #build the sdai library and p21read, using make or equivalent
    add_custom_command( OUTPUT ${PROJECT_NAME} DEPENDS ${PROJECT_NAME}_CONFIGURED stepdai stepcore express stepeditor steputils base
                        COMMAND "$(MAKE)" # ${CMAKE_BUILD_TOOL} #the latter may work on more platforms, but it produces a warning about parallel building
                        WORKING_DIRECTORY ${SCHEMA_OUTPUT_DIR}
                        COMMENT "Building subproject for ${SCHEMA_SHORT_NAME}..."
                        VERBATIM )
    #building this target triggers execution of the above commands, configuring and building the sdai lib and p21read
    add_custom_target( build_cpp_${PROJECT_NAME} ${BUILD_IN_TARGET_ALL} DEPENDS ${PROJECT_NAME} )

    # if testing is enabled, "TESTABLE" sets property EXCLUDE_FROM_ALL and prevents installation
    add_dependencies( build_cpp_${PROJECT_NAME} generate_cpp_${PROJECT_NAME} )

    add_test( NAME test_generate_cpp_${PROJECT_NAME}
              WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
              COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR}
                                       --target generate_cpp_${PROJECT_NAME}
                                       --config $<CONFIGURATION> )
    add_test( NAME test_build_cpp_${PROJECT_NAME}
              WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
              COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR}
                                       --target build_cpp_${PROJECT_NAME}
                                       --config $<CONFIGURATION> )

    # label the tests and set dependencies
    set_tests_properties( test_generate_cpp_${PROJECT_NAME} PROPERTIES LABELS cpp_schema_gen )
    set_tests_properties( test_build_cpp_${PROJECT_NAME} PROPERTIES DEPENDS generate_cpp_${PROJECT_NAME} LABELS cpp_schema_build )

    #find all part 21 files in schema dir, add a test for each one
    get_filename_component( SCHEMA_DIR ${SCHEMA_FILE} PATH )
    file( GLOB_RECURSE P21_FILES ${SCHEMA_DIR}/*.stp ${SCHEMA_DIR}/*.step ${SCHEMA_DIR}/*.p21 ${SCHEMA_DIR}/*.ifc )
    foreach( TEST_FILE ${P21_FILES} )
        get_filename_component( FNAME ${TEST_FILE} NAME_WE )
        add_test( NAME read_write_cpp_${SCHEMA_SHORT_NAME}_${FNAME}
                  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
                  COMMAND ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/p21read_${PROJECT_NAME} ${TEST_FILE} )
        set_tests_properties( read_write_cpp_${SCHEMA_SHORT_NAME}_${FNAME} PROPERTIES DEPENDS build_cpp_${PROJECT_NAME} LABELS cpp_schema_rw )
    endforeach()

ENDFUNCTION(BUILD_A_SCHEMA)

if( DEFINED SCL_BUILD_SCHEMAS )
    if( SCL_BUILD_SCHEMAS STREQUAL "ALL" )
        file( GLOB_RECURSE SCL_BUILD_SCHEMAS ${SCL_SOURCE_DIR}/data/*.exp )
    endif()
    foreach( ap ${SCL_BUILD_SCHEMAS} )
        BUILD_A_SCHEMA( ${ap} )
    endforeach()
endif()
