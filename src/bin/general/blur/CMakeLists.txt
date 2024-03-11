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

TARGET_LINK_LIBRARIES(blur Qt5::Core
        ${FFTW3_LIBRARY_NAME}
        ${CATS_LIBS})

INSTALL(TARGETS
            blur
        DESTINATION
            bin
        )
