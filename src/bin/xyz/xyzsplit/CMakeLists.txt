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
QT5_USE_MODULES(xyzsplit Core)
ADD_DEPENDENCIES(svs-server cats_shared)

TARGET_LINK_LIBRARIES(xyzsplit
        ${CATS_LIBS}
        )

INSTALL(TARGETS
            xyzsplit
        RUNTIME DESTINATION
            bin
        )

