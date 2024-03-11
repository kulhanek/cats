# ==============================================================================
# CATs CMake File
# ==============================================================================

# program objects --------------------------------------------------------------
SET(CATS_SRC
        main.cpp
        CATs.cpp
        CATsOptions.cpp
        )

# final build ------------------------------------------------------------------
ADD_EXECUTABLE(cats_inter ${CATS_SRC} ${MOC_LIST})
ADD_DEPENDENCIES(cats_inter cats_shared)

SET_TARGET_PROPERTIES(cats_inter PROPERTIES
                        OUTPUT_NAME cats
                        )

TARGET_LINK_LIBRARIES(cats_inter Qt5::Core Qt5::Script
        ${ReadLine_LIBRARY_NAME}
        ${CATS_LIBS}
        )

INSTALL(TARGETS
            cats_inter
        DESTINATION
            bin
        )

