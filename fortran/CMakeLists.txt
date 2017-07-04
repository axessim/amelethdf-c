# amelethdf/fortran/CMakeList.txt file

IF (NOT HDF5_BUILD_FORTRAN)
  MESSAGE( FATAL_ERROR "HDF5 not build for Fortran." )
ENDIF ()

enable_language(Fortran)

INCLUDE_DIRECTORIES(${HDF5_INCLUDE_DIR_FORTRAN})

SET(amelethdff_lib_dep
  hdf5_hl_fortran hdf5_hl_f90cstub
  hdf5_hl hdf5_fortran hdf5_f90cstub
  hdf5)

ADD_SUBDIRECTORY(src)
ADD_SUBDIRECTORY(test)
