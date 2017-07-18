# ==============================================================================
# CATs CMake File
# ==============================================================================

INCLUDE_DIRECTORIES(lib/cats SYSTEM)
INCLUDE_DIRECTORIES(lib/cats/network SYSTEM)
INCLUDE_DIRECTORIES(lib/cats/network/result SYSTEM)
INCLUDE_DIRECTORIES(lib/cats/network/trajectory SYSTEM)
INCLUDE_DIRECTORIES(lib/cats/maps SYSTEM)
INCLUDE_DIRECTORIES(lib/cats/jscript SYSTEM)

# include subdirectories -------------------------------------------------------
ADD_SUBDIRECTORY(lib)
ADD_SUBDIRECTORY(bin)
