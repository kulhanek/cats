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

TARGET_LINK_LIBRARIES(toprmla Qt5::Core
        ${CATS_LIBS})

INSTALL(TARGETS
            toprmla
        DESTINATION
            bin
        )

