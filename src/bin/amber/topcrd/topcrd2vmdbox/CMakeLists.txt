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
                cats
                ${ASL_LIB_NAME}
                ${SCIMAFIC_CLIB_NAME}
                ${HIPOLY_LIB_NAME}
                ${SYSTEM_LIBS})

INSTALL(TARGETS
            topcrd2vmdbox
        DESTINATION
            bin
        )
