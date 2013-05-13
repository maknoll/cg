##
##  Created on: Apr 03, 2012
##      Author: martinez
##

## FOLDER SPECIFIC

SET(folder_package_depend )
SET(folder_link_libs ${ADDITIONAL_LIBS})
SET(folder_include_dirs )
SET(folder_lib_depend )

IF(WIN32)
  SET(folder_lib_type STATIC)
ENDIF()

### FOLDER GENERIC

SET(current_dir_lists ${CMAKE_CURRENT_LIST_FILE})
INCLUDE(${CMAKE_HOME_DIRECTORY}/cmake/BuildSrcFolderGenericLists.txt NO_POLICY_SCOPE)

# :mode=cmake:
