# ==============================================================================
# CATs CMake File
# ==============================================================================

# include subdirectories -------------------------------------------------------
IF(FFTW3_FOUND)
  #  ADD_SUBDIRECTORY(blur)
ENDIF(FFTW3_FOUND)

ADD_SUBDIRECTORY(d2u)
ADD_SUBDIRECTORY(gauss-aw-gen)
ADD_SUBDIRECTORY(gauss-integrate)
ADD_SUBDIRECTORY(histogram)
ADD_SUBDIRECTORY(integrate)
ADD_SUBDIRECTORY(propsum)
ADD_SUBDIRECTORY(u2d)



