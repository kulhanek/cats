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

QT5_USE_MODULES(propsum Core)
TARGET_LINK_LIBRARIES(propsum
        ${CATS_LIBS})

INSTALL(TARGETS
            propsum
        DESTINATION
            bin
        )
