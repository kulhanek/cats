# ==============================================================================
# CATs CMake File
# ==============================================================================

# program objects --------------------------------------------------------------
SET(TOPMASK_SRC
        main.cpp
        TopMask.cpp
        TopMaskOptions.cpp
        )

# final build ------------------------------------------------------------------
ADD_EXECUTABLE(topmask ${TOPMASK_SRC})
ADD_DEPENDENCIES(topmask cats_shared)

QT5_USE_MODULES(topmask Core)
TARGET_LINK_LIBRARIES(topmask
        ${CATS_LIBS})

INSTALL(TARGETS
            topmask
        DESTINATION
            bin
        )
