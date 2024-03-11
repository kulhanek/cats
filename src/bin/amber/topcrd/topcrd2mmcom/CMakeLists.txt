# ==============================================================================
# CATs CMake File
# ==============================================================================

# program objects --------------------------------------------------------------
SET(TOPCRD2MMCOM_SRC
        main.cpp
        TopCrd2MMCom.cpp
        TopCrd2MMComOptions.cpp
        )

# final build ------------------------------------------------------------------
ADD_EXECUTABLE(topcrd2mmcom ${TOPCRD2MMCOM_SRC})
ADD_DEPENDENCIES(topcrd2mmcom cats_shared)

TARGET_LINK_LIBRARIES(topcrd2mmcom Qt5::Core
        ${CATS_LIBS})

INSTALL(TARGETS
            topcrd2mmcom
        DESTINATION
            bin
        )

