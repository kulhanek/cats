# ==============================================================================
# CATs CMake File
# ==============================================================================

# program objects --------------------------------------------------------------
SET(INTEGRATE_SRC
        gauss-integrate.cpp
        gauss-integrate-opts.cpp
        main.cpp
        )

# final build ------------------------------------------------------------------
ADD_EXECUTABLE(gauss-integrate ${INTEGRATE_SRC})
ADD_DEPENDENCIES(gauss-integrate cats_shared)

TARGET_LINK_LIBRARIES(gauss-integrate Qt5::Core
        ${CATS_LIBS})

INSTALL(TARGETS
            gauss-integrate
        DESTINATION
            bin
        )
