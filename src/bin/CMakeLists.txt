# ==============================================================================
# CATs CMake File
# ==============================================================================

# include subdirectories -------------------------------------------------------
ADD_SUBDIRECTORY(general)
ADD_SUBDIRECTORY(amber)
ADD_SUBDIRECTORY(network)
ADD_SUBDIRECTORY(pdb)
ADD_SUBDIRECTORY(xyz)
ADD_SUBDIRECTORY(svs)
ADD_SUBDIRECTORY(cube)

# cats interpreter -------------------------------------------------------------
ADD_SUBDIRECTORY(cats)
#ADD_SUBDIRECTORY(cats-ide)
