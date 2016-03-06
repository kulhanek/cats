# ==============================================================================
# CATs CMake File
# ==============================================================================

# program objects --------------------------------------------------------------
SET(CATS_IDE_SRC
        main.cpp
        )

# final build ------------------------------------------------------------------
ADD_EXECUTABLE(cats_ide_inter ${CATS_IDE_SRC} ${MOC_LIST})
QT5_USE_MODULES(cats_ide_inter Core Script)
SET_TARGET_PROPERTIES(cats_ide_inter PROPERTIES
                        OUTPUT_NAME cats-ide
                        )

TARGET_LINK_LIBRARIES(cats_ide_inter
        readline
        ${CATS_LIBS}
        )

INSTALL(TARGETS
            cats_ide_inter
        DESTINATION
            bin
        )

