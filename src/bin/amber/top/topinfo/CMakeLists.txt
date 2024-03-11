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

TARGET_LINK_LIBRARIES(topinfo Qt5::Core
        ${CATS_LIBS})

INSTALL(TARGETS
            topinfo
        DESTINATION
            bin
        )
