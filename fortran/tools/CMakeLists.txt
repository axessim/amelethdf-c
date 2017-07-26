# amelethdf/fortran/tools/CMakeList.txt file

# build fortran tools
INCLUDE_DIRECTORIES(
  ${AMELETHDF_BINARY_DIR}/fortran/src
  )

FILE (GLOB tools_src "*.f90")
FOREACH (tool_file ${tools_src})
  FILE (RELATIVE_PATH rel_tool_file ${CMAKE_CURRENT_SOURCE_DIR} ${tool_file})
  STRING (REPLACE ".f90"  "" tool_exec ${rel_tool_file})
  SET(tool_exec "fortran_${tool_exec}")

  ADD_DEFINITIONS("-DAH5_DATA_DIR=\"${AMELETHDF_EXEMPLES_DIR}\"")
  ADD_DEFINITIONS("-DAH5_TEST_DATA_DIR=\"${AMELETHDF_TEST_DATA_DIR}\"")

  ADD_EXECUTABLE (${tool_exec} ${tool_file})
  TARGET_LINK_LIBRARIES (${tool_exec}
    amelethdff
    amelethdfc
    ${AMELETHDF_DEP_LINK_LIBS})

ENDFOREACH (tool_file)
