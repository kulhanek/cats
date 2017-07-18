# ==============================================================================
# CATs CMake File
# ==============================================================================

# program objects --------------------------------------------------------------
SET(INTEGRATE_SRC
        integrate.cpp
        integrate-opts.cpp
        main.cpp
        )

# final build ------------------------------------------------------------------
ADD_EXECUTABLE(integrate ${INTEGRATE_SRC})
ADD_DEPENDENCIES(integrate cats_shared)

QT5_USE_MODULES(integrate Core)
TARGET_LINK_LIBRARIES(integrate
        ${CATS_LIBS})

INSTALL(TARGETS
            integrate
        DESTINATION
            bin
        )
