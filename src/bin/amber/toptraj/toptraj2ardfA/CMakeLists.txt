# ==============================================================================
# CATs CMake File
# ==============================================================================

# program objects --------------------------------------------------------------
SET(TOPTRAJ2ARDFA_SRC
        main.cpp
        TopTraj2ARdfA.cpp
        TopTraj2ARdfAOptions.cpp
        )

# final build ------------------------------------------------------------------
ADD_EXECUTABLE(toptraj2ardfA ${TOPTRAJ2ARDFA_SRC})
QT5_USE_MODULES(toptraj2ardfA Core)
TARGET_LINK_LIBRARIES(toptraj2ardfA
        ${CATS_LIBS})

INSTALL(TARGETS
            toptraj2ardfA
        DESTINATION
            bin
        )

