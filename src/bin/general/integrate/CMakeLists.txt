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

TARGET_LINK_LIBRARIES(integrate Qt5::Core
        ${CATS_LIBS})

INSTALL(TARGETS
            integrate
        DESTINATION
            bin
        )
