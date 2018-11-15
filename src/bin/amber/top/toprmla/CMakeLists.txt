# ==============================================================================
# CATs CMake File
# ==============================================================================

# program objects --------------------------------------------------------------
SET(TOPRMLA_SRC
        main.cpp
        TopRMLA.cpp
        TopRMLAOptions.cpp
        )

# final build ------------------------------------------------------------------
ADD_EXECUTABLE(toprmla ${TOPRMLA_SRC})
ADD_DEPENDENCIES(toprmla cats_shared)

QT5_USE_MODULES(toprmla Core)
TARGET_LINK_LIBRARIES(toprmla
        ${CATS_LIBS})

INSTALL(TARGETS
            toprmla
        DESTINATION
            bin
        )

