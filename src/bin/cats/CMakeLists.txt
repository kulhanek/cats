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

QT5_USE_MODULES(cats_inter Core Script)
SET_TARGET_PROPERTIES(cats_inter PROPERTIES
                        OUTPUT_NAME cats
                        )

TARGET_LINK_LIBRARIES(cats_inter
        readline
        ${CATS_LIBS}
        )

INSTALL(TARGETS
            cats_inter
        DESTINATION
            bin
        )

