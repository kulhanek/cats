# ==============================================================================
# CATs CMake File
# ==============================================================================

# program objects --------------------------------------------------------------
SET(TOPMANIP_SRC
        main.cpp
        TopManip.cpp
        TopManipOptions.cpp
        )

# final build ------------------------------------------------------------------
ADD_EXECUTABLE(topcharges ${TOPMANIP_SRC})
ADD_DEPENDENCIES(topcharges cats_shared)

TARGET_LINK_LIBRARIES(topcharges
        Qt5::Core
        ${CATS_LIBS})

INSTALL(TARGETS
            topcharges
        DESTINATION
            bin
        )
