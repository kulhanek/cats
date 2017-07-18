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

QT5_USE_MODULES(histogram Core)
TARGET_LINK_LIBRARIES(histogram
        ${CATS_LIBS})

INSTALL(TARGETS
            histogram
        DESTINATION
            bin
        )
