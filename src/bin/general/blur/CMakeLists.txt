# ==============================================================================
# CATs CMake File
# ==============================================================================

# program objects --------------------------------------------------------------
SET(BLUR_SRC
        main.cpp
        blur.cpp
        blur-opts.cpp
        )

# final build ------------------------------------------------------------------
ADD_EXECUTABLE(blur ${BLUR_SRC})
ADD_DEPENDENCIES(blur cats_shared)

QT5_USE_MODULES(blur Core)
TARGET_LINK_LIBRARIES(blur
        ${CATS_LIBS})

INSTALL(TARGETS
            blur
        DESTINATION
            bin
        )
