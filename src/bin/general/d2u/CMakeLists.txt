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

TARGET_LINK_LIBRARIES(d2u Qt5::Core
        ${CATS_LIBS})

INSTALL(TARGETS
            d2u
        DESTINATION
            bin
        )
