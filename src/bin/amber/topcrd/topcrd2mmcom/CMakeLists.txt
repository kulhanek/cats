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

QT5_USE_MODULES(topcrd2mmcom Core)
TARGET_LINK_LIBRARIES(topcrd2mmcom
        ${CATS_LIBS})

INSTALL(TARGETS
            topcrd2mmcom
        DESTINATION
            bin
        )

