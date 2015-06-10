# ==============================================================================
# CATs CMake File
# ==============================================================================

# program objects --------------------------------------------------------------
SET(TOPTRAJ2ARDFB_SRC
        main.cpp
        TopTraj2ARdfB.cpp
        TopTraj2ARdfBOptions.cpp
        )

# final build ------------------------------------------------------------------
ADD_EXECUTABLE(toptraj2ardfB ${TOPTRAJ2ARDFB_SRC})
QT5_USE_MODULES(toptraj2ardfB Core)
TARGET_LINK_LIBRARIES(toptraj2ardfB
                ${ASL_LIB_NAME}
                ${SCIMAFIC_CLIB_NAME}
                ${HIPOLY_LIB_NAME}
                ${SYSTEM_LIBS})

INSTALL(TARGETS
            toptraj2ardfB
        DESTINATION
            bin
        )

