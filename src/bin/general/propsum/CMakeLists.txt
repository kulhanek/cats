# ==============================================================================
# CATs CMake File
# ==============================================================================

# program objects --------------------------------------------------------------
SET(PROPSUM_SRC
        propsum.cpp
        propsum-opts.cpp
        main.cpp
        )

# final build ------------------------------------------------------------------
ADD_EXECUTABLE(propsum ${PROPSUM_SRC})
ADD_DEPENDENCIES(propsum cats_shared)

TARGET_LINK_LIBRARIES(propsum Qt5::Core
        ${CATS_LIBS})

INSTALL(TARGETS
            propsum
        DESTINATION
            bin
        )
