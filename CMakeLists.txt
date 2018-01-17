# Set the name and the supported language of the project
cmake_policy(SET CMP0048 NEW)
project(gmysqlcc VERSION 0.4.0 LANGUAGES C)

# Set the minimum version of cmake required to build this project
cmake_minimum_required(VERSION 2.6)

# Use the package PkgConfig to detect GTK+ headers/library files
find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK3 REQUIRED gtk+-3.0>=3.20)
pkg_check_modules(MYSQL REQUIRED mysqlclient>=16)
pkg_check_modules(GTKSRCVW3 gtksourceview-3.0>=3.20)
IF( GTKSRCVW3_FOUND )
    ADD_DEFINITIONS('-DUSE_GTKSOURCEVIEW')
ENDIF()

# Setup CMake to use GTK+, tell the compiler where to look for headers
# and to the linker where to look for libraries
include_directories(${GTK3_INCLUDE_DIRS})
include_directories(${MYSQL_INCLUDE_DIRS})
include_directories(${GTKSRCVW3_INCLUDE_DIRS})
link_directories(${GTK3_LIBRARY_DIRS})
link_directories(${MYSQL_LIBRARY_DIRS})
link_directories(${GTKSRCVW3_LIBRARY_DIRS})

# Add other flags to the compiler
add_definitions(${GTK3_CFLAGS_OTHER})
add_definitions(${MYSQL_CFLAGS_OTHER})
add_definitions(${GTKSRCVW3_CFLAGS_OTHER})

file(GLOB_RECURSE SRC_FILES ${PROJECT_SOURCE_DIR}/src/*.c)

# Add an executable compiled 
add_executable(gmysqlcc ${SRC_FILES})

# Link the target to the GTK+ libraries
target_link_libraries(gmysqlcc ${GTK3_LIBRARIES} ${MYSQL_LIBRARIES} ${GTKSRCVW3_LIBRARIES})

INSTALL(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin)

INSTALL( FILES data/${PROJECT_NAME}.desktop 
	DESTINATION ${CMAKE_INSTALL_PREFIX}/share/applications )
	
INSTALL( FILES data/${PROJECT_NAME}-16.png 
	DESTINATION ${CMAKE_INSTALL_PREFIX}/share/icons/hicolor/16x16/apps 
	RENAME ${PROJECT_NAME}.png )
INSTALL( FILES data/${PROJECT_NAME}-32.png  
	DESTINATION ${CMAKE_INSTALL_PREFIX}/share/icons/hicolor/32x32/apps 
	RENAME ${PROJECT_NAME}.png )
INSTALL( FILES data/${PROJECT_NAME}-48.png 
	DESTINATION ${CMAKE_INSTALL_PREFIX}/share/icons/hicolor/48x48/apps 
	RENAME ${PROJECT_NAME}.png )
INSTALL( FILES data/${PROJECT_NAME}-64.png 
	DESTINATION ${CMAKE_INSTALL_PREFIX}/share/icons/hicolor/64x64/apps 
	RENAME ${PROJECT_NAME}.png )

# for translations
IF (GETTEXT_FOUND)

  # get all translation files; they're expected to be named using the following convention:
  # '<lang code>[_<country code].po'
  # eg: fr_CA.po (the country code is optional)
  FILE(GLOB PO_FILES ${CMAKE_SOURCE_DIR}/po/*.po)

  # Install each of the translation files
  FOREACH(PO_FILE IN LISTS PO_FILES)
    # extract the language (and optional country code) from the filename
    GET_FILENAME_COMPONENT(LANG ${PO_FILE} NAME_WE)
    GETTEXT_PROCESS_PO_FILES(${LANG} ALL PO_FILES ${PO_FILE})

    SET(TRANSLATIONS_INSTALL_DIR ${CMAKE_INSTALL_PREFIX}/share/locale/${LANG}/LC_MESSAGES)

    # install to share/locale and rename each to ring-client-gnome.mo
    INSTALL(
      FILES ${PROJECT_BINARY_DIR}/${LANG}.gmo
      DESTINATION ${TRANSLATIONS_INSTALL_DIR}
      RENAME ${PROJECT_NAME}.mo
    )
  ENDFOREACH(PO_FILE)

ENDIF(GETTEXT_FOUND)

