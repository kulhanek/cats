# ==============================================================================
# CATs CMake File
# ==============================================================================

# program objects --------------------------------------------------------------
SET(HISTOGRAM_SRC
        main.cpp
        histogram.cpp
        histogram-opts.cpp
        )

# final build ------------------------------------------------------------------
ADD_EXECUTABLE(histogram ${HISTOGRAM_SRC})
ADD_DEPENDENCIES(histogram cats_shared)

TARGET_LINK_LIBRARIES(histogram Qt5::Core
        ${CATS_LIBS})

INSTALL(TARGETS
            histogram
        DESTINATION
            bin
        )
