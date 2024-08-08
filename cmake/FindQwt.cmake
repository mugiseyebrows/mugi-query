
find_library(QWT_LIBRARY NAMES qwt)

find_path(QWT_INCLUDE_DIR NAMES qwt.h)

#message(STATUS "QWT_LIBRARY ${QWT_LIBRARY}")
#message(STATUS "QWT_INCLUDE_DIR ${QWT_INCLUDE_DIR}")

include(FindPackageHandleStandardArgs)

find_package_handle_standard_args(
    qwt
    FOUND_VAR QWT_FOUND
    REQUIRED_VARS QWT_LIBRARY QWT_INCLUDE_DIR
)

if (QWT_FOUND)
    add_library(qwt UNKNOWN IMPORTED)
    set_target_properties(qwt PROPERTIES
        IMPORTED_LOCATION ${QWT_LIBRARY}
        INTERFACE_INCLUDE_DIRECTORIES ${QWT_INCLUDE_DIR}
    )
endif()
