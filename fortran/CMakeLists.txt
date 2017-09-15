# amelethdf/fortran/CMakeList.txt file

enable_language(Fortran)

SET(CMAKE_Fortran_FLAGS "${CMAKE_Fortran_FLAGS} -cpp")
# SET(CMAKE_Fortran_FLAGS "${CMAKE_Fortran_FLAGS} -fpp") # TODO ifort

INCLUDE_DIRECTORIES(${HDF5_INCLUDE_DIR_FORTRAN} "/opt/axessim/adk-2.0/include/shared")
INCLUDE_DIRECTORIES(${HDF5_INCLUDE_DIRS})

IF(HDF5_Fortran_LIBRARIES)
  SET(amelethdff_lib_dep
    ${HDF5_Fortran_LIBRARIES}
    ${HDF5_Fortran_HL_LIBRARIES}
    ${AMELETHDF_DEP_LINK_LIBS})
ELSE()
  IF(HDF5_VERSION_STRING VERSION_LESS "1.8.17")
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
ENDIF()

ADD_SUBDIRECTORY(src)
ADD_SUBDIRECTORY(test)
ADD_SUBDIRECTORY(tools)
