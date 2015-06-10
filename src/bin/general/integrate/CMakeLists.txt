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
QT5_USE_MODULES(integrate Core)
TARGET_LINK_LIBRARIES(integrate ${HIPOLY_LIB_NAME} ${SYSTEM_LIBS})

INSTALL(TARGETS
            integrate
        DESTINATION
            bin
        )
