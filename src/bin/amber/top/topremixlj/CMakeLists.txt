# ==============================================================================
# CATs CMake File
# ==============================================================================

# program objects --------------------------------------------------------------
SET(TOPREMIXLJ_SRC
        main.cpp
        TopRemixLJ.cpp
        TopRemixLJOptions.cpp
        )

# final build ------------------------------------------------------------------
ADD_EXECUTABLE(topremixlj ${TOPREMIXLJ_SRC})
ADD_DEPENDENCIES(topremixlj cats_shared)

TARGET_LINK_LIBRARIES(topremixlj Qt5::Core
        ${CATS_LIBS})

INSTALL(TARGETS
            topremixlj
        DESTINATION
            bin
        )
