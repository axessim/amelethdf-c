# amelethdf/fortran/src/CMakeList.txt file

FILE(GLOB amelethdff_src *.f90)

ADD_LIBRARY(amelethdff_obj OBJECT ${amelethdff_src})

# shared libraries need PIC
set_property(TARGET amelethdff_obj PROPERTY POSITION_INDEPENDENT_CODE 1)

# ADD_LIBRARY(amelethdff_static STATIC ${amelethdff_src})
ADD_LIBRARY(amelethdff_static STATIC $<TARGET_OBJECTS:amelethdff_obj>)
SET_TARGET_PROPERTIES(amelethdff_static PROPERTIES OUTPUT_NAME "amelethdff")
TARGET_LINK_LIBRARIES(amelethdff_static amelethdfc_static ${amelethdff_lib_dep})
INSTALL(TARGETS amelethdff_static
  EXPORT        AmeletHDFFLibraryDepends
  RUNTIME       DESTINATION "${INSTALL_BIN_DIR}" COMPONENT bin
  LIBRARY       DESTINATION "${INSTALL_LIB_DIR}" COMPONENT lib
  ARCHIVE       DESTINATION "${INSTALL_LIB_DIR}" COMPONENT lib
  PUBLIC_HEADER DESTINATION "${INSTALL_INCLUDE_DIR}"
  COMPONENT dev
  )

# shared and static libraries built from the same object files
ADD_LIBRARY(amelethdff_shared SHARED $<TARGET_OBJECTS:amelethdff_obj>)
SET_TARGET_PROPERTIES(amelethdff_shared PROPERTIES OUTPUT_NAME "amelethdff")
TARGET_LINK_LIBRARIES(amelethdff_shared ${amelethdff_lib_dep})
INSTALL(TARGETS amelethdff_shared
  EXPORT        AmeletHDFFLibraryDepends
  RUNTIME       DESTINATION "${INSTALL_BIN_DIR}" COMPONENT bin
  LIBRARY       DESTINATION "${INSTALL_LIB_DIR}" COMPONENT lib
  ARCHIVE       DESTINATION "${INSTALL_ARCHIVE_DIR}" COMPONENT lib
  COMPONENT dev
  )

FILE(GLOB amelethdff_module ${CMAKE_CURRENT_BINARY_DIR}/*.mod)
INSTALL(FILES ${amelethdff_module} DESTINATION "${INSTALL_INCLUDE_DIR}/fortran")
