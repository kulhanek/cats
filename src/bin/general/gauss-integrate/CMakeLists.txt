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
QT5_USE_MODULES(gauss-integrate Core)
TARGET_LINK_LIBRARIES(gauss-integrate ${HIPOLY_LIB_NAME} ${SYSTEM_LIBS})

INSTALL(TARGETS
            gauss-integrate
        DESTINATION
            bin
        )
