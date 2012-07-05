# SC_SDAI_HEADER.cmake - included at the beginning of the CMakeLists.txt written by fedex_plus
#import the following variables, without prefix
load_cache( ${BUILD_ROOT} READ_WITH_PREFIX ""
            CMAKE_BUILD_TYPE
            CMAKE_LIBRARY_OUTPUT_DIRECTORY
            CMAKE_ARCHIVE_OUTPUT_DIRECTORY
            CMAKE_RUNTIME_OUTPUT_DIRECTORY
            SCL_ABI_SOVERSION
            SCL_ABI_VERSION
            SCL_BINARY_DIR
            SCL_SOURCE_DIR
            SCL_BUILD_SHARED_LIBS
            SCL_BUILD_STATIC_LIBS )

INCLUDE(${SCL_CMAKE_DIR}/SCL_Utils.cmake)

include_directories( ${BUILD_ROOT}/include ${SCL_SOURCE_DIR}/include ${CMAKE_CURRENT_SOURCE_DIR} ${SCL_SOURCE_DIR}/data
                     ${SCL_SOURCE_DIR}/src/cldai ${SCL_SOURCE_DIR}/src/cleditor ${SCL_SOURCE_DIR}/src/clutils
                     ${SCL_SOURCE_DIR}/src/clstepcore ${SCL_SOURCE_DIR}/src/base )

#set full path to each of the libs we link with. necessary because this is a separate project, so the info about each target is lost.
set(libs stepdai stepcore express stepeditor steputils base )
if( APPLE )
    foreach( lib ${libs} )
        list( APPEND LINK_LIBS "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/lib${lib}.dylib" )
    endforeach()
elseif(WIN32) #windows
    foreach( lib ${libs} )
        list( APPEND LINK_LIBS "${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}/${lib}.lib" )
    endforeach()
else() #linux
    foreach( lib ${libs} )
        list( APPEND LINK_LIBS "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/lib${lib}.so" )
    endforeach()
endif()

#copied from the main CMakeLists.txt
if(MSVC)
  add_definitions( -D__MSVC__ -D__WIN32__ )
  # Disable warning for preferred usage of secure functions (example strcpy should be strcpy_s, ...)
  add_definitions( -D_CRT_SECURE_NO_WARNINGS -D_CRT_NONSTDC_NO_WARNINGS )
elseif(BORLAND)
  add_definitions( -D__BORLAND__ -D__WIN32__ )
else() #gcc
  add_definitions( -pedantic -W -Wall -Wundef -Wfloat-equal -Wshadow -Winline -Wno-long-long )
endif()
