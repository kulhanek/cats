# ==============================================================================
# CATs CMake File
# ==============================================================================

# program objects --------------------------------------------------------------
SET(D2U_SRC
        d2u.cpp
        d2u-opts.cpp
        )

# final build ------------------------------------------------------------------
ADD_EXECUTABLE(d2u ${D2U_SRC})
ADD_DEPENDENCIES(d2u cats_shared)

QT5_USE_MODULES(d2u Core)
TARGET_LINK_LIBRARIES(d2u ${HIPOLY_LIB_NAME} ${SYSTEM_LIBS})

INSTALL(TARGETS
            d2u
        DESTINATION
            bin
        )
