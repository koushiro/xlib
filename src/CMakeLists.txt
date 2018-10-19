set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/target/lib)

set(XLIB
        xalloc.h
        xalloc.c
        xstr.h
        xstr.c
        xlist.h
        xlist.c
        xdict.h
        xdict.c
        xskiplist.h
        xskiplist.c
        )

add_library(xlib ${XLIB})
set_target_properties(xlib PROPERTIES OUTPUT_NAME "xlib")
set_target_properties(xlib PROPERTIES LINKER_LANGUAGE C)
