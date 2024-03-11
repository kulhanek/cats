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

TARGET_LINK_LIBRARIES(topmask Qt5::Core
        ${CATS_LIBS})

INSTALL(TARGETS
            topmask
        DESTINATION
            bin
        )
