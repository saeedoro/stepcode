set(FEDEX_COMMON_SRCS
    classes_misc.c
    ../express/fedex.c
)

set(fedex_plus_SOURCES
    ${FEDEX_COMMON_SRCS}
    fedex_main.c
    classes_wrapper.cc
    classes.c
    selects.c
    multpass.c
    ../clstepcore/collect.cc
    complexlist.cc
    ../clstepcore/entlist.cc
    ../clstepcore/multlist.cc
    ../clstepcore/orlist.cc
    ../clstepcore/entnode.cc
    expressbuild.cc
    ../clstepcore/non-ors.cc
    ../clstepcore/match-ors.cc
    ../clstepcore/trynext.cc
    write.cc
    ../clstepcore/print.cc
)

include_directories(
    ${SC_SOURCE_DIR}/include
    ${SC_SOURCE_DIR}/include/exppp
    ${SC_SOURCE_DIR}/include/express
    ${SC_SOURCE_DIR}/src/base
    ${SC_SOURCE_DIR}/src/clutils
)

SC_ADDEXEC(fedex_plus "${fedex_plus_SOURCES}" "libexppp;express;base")

add_dependencies( fedex_plus version_string )