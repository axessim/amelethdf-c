# amelethdf/fortran/CMakeList.txt file

IF (NOT HDF5_BUILD_FORTRAN)
  MESSAGE( FATAL_ERROR "HDF5 not build for Fortran." )
ENDIF ()

enable_language(Fortran)

SET(CMAKE_Fortran_FLAGS "${CMAKE_Fortran_FLAGS} -cpp")
# SET(CMAKE_Fortran_FLAGS "${CMAKE_Fortran_FLAGS} -fpp") # TODO ifort

INCLUDE_DIRECTORIES(${HDF5_INCLUDE_DIR_FORTRAN} "/opt/axessim/adk-2.0/include/shared")

IF (HDF5_VERSION_STRING VERSION_LESS "1.8.17")
  SET(amelethdff_lib_dep
    hdf5_hl_fortran hdf5_hl_f90cstub
    hdf5_fortran hdf5_f90cstub
    ${AMELETHDF_DEP_LINK_LIBS})
ELSE()
  SET(amelethdff_lib_dep
    hdf5_hl_fortran-shared hdf5_hl_f90cstub-shared
    hdf5_fortran-shared hdf5_f90cstub-shared
    ${AMELETHDF_DEP_LINK_LIBS})
ENDIF()

ADD_SUBDIRECTORY(src)
ADD_SUBDIRECTORY(test)
ADD_SUBDIRECTORY(tools)
