# ==============================================================================
# CATs CMake File
# ==============================================================================

# program objects --------------------------------------------------------------
SET(TOPCRDGEO_SRC
        main.cpp
        TopCrdGeo.cpp
        TopCrdGeoOptions.cpp
        )

# final build ------------------------------------------------------------------
ADD_EXECUTABLE(topcrdgeo ${TOPCRDGEO_SRC})
QT5_USE_MODULES(topcrdgeo Core)
TARGET_LINK_LIBRARIES(topcrdgeo
        ${CATS_LIBS})

INSTALL(TARGETS
            topcrdgeo
        DESTINATION
            bin
        )

