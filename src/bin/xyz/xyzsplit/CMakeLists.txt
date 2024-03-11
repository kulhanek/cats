# ==============================================================================
# CATs CMake File
# ==============================================================================

# program objects --------------------------------------------------------------
SET(XYZSPLIT_SRC
        XYZSplit.cpp
        XYZSplitOptions.cpp
        )

# final build ------------------------------------------------------------------
ADD_EXECUTABLE(xyzsplit ${XYZSPLIT_SRC})
ADD_DEPENDENCIES(xyzsplit cats_shared)

TARGET_LINK_LIBRARIES(xyzsplit Qt5::Core
        ${CATS_LIBS}
        )

INSTALL(TARGETS
            xyzsplit
        RUNTIME DESTINATION
            bin
        )

