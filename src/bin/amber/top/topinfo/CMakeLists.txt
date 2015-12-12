# ==============================================================================
# CATs CMake File
# ==============================================================================

# program objects --------------------------------------------------------------
SET(TOPINFO_SRC
        main.cpp
        TopInfo.cpp
        TopInfoOptions.cpp
        )

# final build ------------------------------------------------------------------
ADD_EXECUTABLE(topinfo ${TOPINFO_SRC})
ADD_DEPENDENCIES(topinfo cats_shared)

QT5_USE_MODULES(topinfo Core)
TARGET_LINK_LIBRARIES(topinfo
                ${ASL_LIB_NAME}
                ${SCIMAFIC_CLIB_NAME}
                ${HIPOLY_LIB_NAME}
                ${SYSTEM_LIBS})

INSTALL(TARGETS
            topinfo
        DESTINATION
            bin
        )
