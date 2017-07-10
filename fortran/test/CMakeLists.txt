# amelethdf/fortran/test/CMakeList.txt file

# build fortran unit test tool
FILE(GLOB_RECURSE utestf_src utest.f90)
ADD_LIBRARY(utestf_lib STATIC ${utestf_src})
SET_TARGET_PROPERTIES(utestf_lib PROPERTIES OUTPUT_NAME "utestf")
INSTALL(TARGETS utestf_lib ARCHIVE DESTINATION "lib")
FILE(GLOB utestf_module ${CMAKE_CURRENT_BINARY_DIR}/*.mod)
INSTALL(FILES ${utestf_inc} DESTINATION "include")
SET(utestf_inc ${CMAKE_CURRENT_BINARY_DIR})


# build tests
INCLUDE_DIRECTORIES(
  ${AMELETHDF_BINARY_DIR}/fortran/src
  )

FILE (GLOB tests_src "test*.f90")
FOREACH (test_file ${tests_src})
  FILE (RELATIVE_PATH rel_test_file ${CMAKE_CURRENT_SOURCE_DIR} ${test_file})
  STRING (REPLACE ".f90"  "" test_exec ${rel_test_file})
  SET(test_exec "fortran_${test_exec}")

  ADD_DEFINITIONS("-DAH5_DATA_DIR=\"${AMELETHDF_EXEMPLES_DIR}\"")
  ADD_DEFINITIONS("-DAH5_TEST_DATA_DIR=\"${AMELETHDF_TEST_DATA_DIR}\"")

  ADD_EXECUTABLE (${test_exec} ${test_file})
  TARGET_LINK_LIBRARIES (${test_exec}
    utestf_lib
    amelethdff
    amelethdfc
    ${AMELETHDF_DEP_LINK_LIBS})

  ADD_TEST (${test_exec} "${EXECUTABLE_OUTPUT_PATH}/${test_exec}")
ENDFOREACH (test_file)
