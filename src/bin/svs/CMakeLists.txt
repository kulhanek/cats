# ==============================================================================
# CATs CMake File
# ==============================================================================

# structure database management ------------------
ADD_SUBDIRECTORY(svs-split-mol)

# project database management --------------------
ADD_SUBDIRECTORY(svs-project-sql)
ADD_SUBDIRECTORY(svs-project-create)
ADD_SUBDIRECTORY(svs-project-info)
ADD_SUBDIRECTORY(svs-project-alter)
ADD_SUBDIRECTORY(svs-project-add-structures)

# client/server architecture ---------------------
ADD_SUBDIRECTORY(svs-client)
ADD_SUBDIRECTORY(svs-server)


