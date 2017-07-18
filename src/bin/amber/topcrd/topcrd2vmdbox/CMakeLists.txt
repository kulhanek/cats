# ==============================================================================
# CATs CMake File
# ==============================================================================

# program objects --------------------------------------------------------------
SET(TOPCRD2VMDBOX_SRC
        main.cpp
        TopCrd2VMDBox.cpp
        TopCrd2VMDBoxOptions.cpp
        )

# final build ------------------------------------------------------------------
ADD_EXECUTABLE(topcrd2vmdbox ${TOPCRD2VMDBOX_SRC})
QT5_USE_MODULES(topcrd2vmdbox Core)
TARGET_LINK_LIBRARIES(topcrd2vmdbox
        ${CATS_LIBS})

INSTALL(TARGETS
            topcrd2vmdbox
        DESTINATION
            bin
        )
