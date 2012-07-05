# SC_SDAI_FOOTER.cmake - included at the end of the CMakeLists.txt written by fedex_plus
# fedex_plus sets two variables, ${SDAI_HEADERS} and ${SDAI_SOURCES}
# everything else comes from SC_SDAI_HEADER.cmake
SCL_ADDLIB( ${SDAI_LIB} "${SDAI_HEADERS} ${SDAI_SOURCES}" ${LINK_LIBS} "" ) # "libs stepdai stepcore express stepeditor steputils base" "" )

foreach( src ${SC_SDAI_EXES_SRC} )
    get_filename_component( name ${src} NAME_WE )
    SCL_ADDEXEC( "${name}_${SDAI_LIB}" "${src}" "${SDAI_LIB};${LINK_LIBS}" "" )
endforeach()
