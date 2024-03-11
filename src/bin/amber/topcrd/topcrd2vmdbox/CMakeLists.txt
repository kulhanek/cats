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
ADD_DEPENDENCIES(topcrd2vmdbox cats_shared)

TARGET_LINK_LIBRARIES(topcrd2vmdbox Qt5::Core
        ${CATS_LIBS})

INSTALL(TARGETS
            topcrd2vmdbox
        DESTINATION
            bin
        )
