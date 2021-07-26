# ==============================================================================
# CATs CMake File
# ==============================================================================

INCLUDE_DIRECTORIES(lib/cats)
INCLUDE_DIRECTORIES(lib/cats/network)
INCLUDE_DIRECTORIES(lib/cats/network/result)
INCLUDE_DIRECTORIES(lib/cats/network/trajectory)
INCLUDE_DIRECTORIES(lib/cats/maps)
INCLUDE_DIRECTORIES(lib/cats/jscript)
INCLUDE_DIRECTORIES(lib/cats/sqlite3)
INCLUDE_DIRECTORIES(lib/cats/vs)

# include subdirectories -------------------------------------------------------
ADD_SUBDIRECTORY(lib)
ADD_SUBDIRECTORY(bin)
