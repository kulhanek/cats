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

QT5_USE_MODULES(topcharges Core)
TARGET_LINK_LIBRARIES(topcharges
        ${CATS_LIBS})

INSTALL(TARGETS
            topcharges
        DESTINATION
            bin
        )
