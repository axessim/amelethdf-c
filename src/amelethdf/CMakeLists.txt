# amelethdf/src/lib CMakeList.txt file

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR} ${AMELETHDF_DEP_INCLUDE_DIRS})

FILE(GLOB_RECURSE amelethdf_src *.c)
ADD_DEFINITIONS(-DAMELETHDF_C_LIBRARY)
ADD_LIBRARY(amelethdfc_obj OBJECT ${amelethdf_src})

# shared libraries need PIC
set_property(TARGET amelethdfc_obj PROPERTY POSITION_INDEPENDENT_CODE 1)

# shared and static libraries built from the same object files
add_library(amelethdfc_shared SHARED $<TARGET_OBJECTS:amelethdfc_obj>)
add_library(amelethdfc_static STATIC $<TARGET_OBJECTS:amelethdfc_obj>)

SET_TARGET_PROPERTIES(amelethdfc_shared PROPERTIES OUTPUT_NAME "amelethdfc")
SET_TARGET_PROPERTIES(amelethdfc_static PROPERTIES OUTPUT_NAME "amelethdfc")

TARGET_LINK_LIBRARIES(amelethdfc_shared ${AMELETHDF_DEP_LINK_LIBS})

INSTALL(TARGETS amelethdfc_shared
  EXPORT  AmeletHDFLibraryDepends
  RUNTIME       DESTINATION "${INSTALL_BIN_DIR}"
  LIBRARY       DESTINATION "${INSTALL_LIB_DIR}"
  ARCHIVE       DESTINATION "${INSTALL_LIB_DIR}"
  PUBLIC_HEADER DESTINATION "${INSTALL_INCLUDE_DIR}"
  )
INSTALL(TARGETS amelethdfc_static
  EXPORT  AmeletHDFLibraryDepends
  RUNTIME       DESTINATION "${INSTALL_BIN_DIR}"
  LIBRARY       DESTINATION "${INSTALL_LIB_DIR}"
  ARCHIVE       DESTINATION "${INSTALL_LIB_DIR}"
  PUBLIC_HEADER DESTINATION "${INSTALL_INCLUDE_DIR}"
  )

FILE(GLOB_RECURSE amelethdf_header *.h)
INSTALL(FILES ${amelethdf_header} DESTINATION "include")

add_static_analysis(amelethdfc)
