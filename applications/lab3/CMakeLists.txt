# Get the name of this directory and use that as the name of the build_target/application

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_FLAGS_RELEASE "-O3")

# SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pg")
# SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -pg")
# SET(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -pg")

get_filename_component(TARGET_NAME "${CMAKE_CURRENT_SOURCE_DIR}" NAME)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/include/vr/vrlib_version.h.in ${CMAKE_CURRENT_SOURCE_DIR}/include/vr/vrlib_version.h @ONLY)

# Find all source files
FILE(GLOB_RECURSE SOURCE *.cpp *.c *.h)
#FILE(GLOB SOURCE_EXTRA ../*.cpp ../*.h)

INCLUDE_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}/include")


# Create an executable from the sources
ADD_EXECUTABLE(${TARGET_NAME} ${SOURCE} ${SOURCE_EXTRA})


# This executable requires a few libraries to link
TARGET_LINK_LIBRARIES(${TARGET_NAME} ${GLFW3_LIBRARY} ${OPENGL_LIBRARIES} ${SOIL_LIBRARIES} ${ASSIMP_LIBRARIES_RELEASE} ${ZLIB_LIBRARY} ${FREETYPE_LIBRARIES} )

IF(NOT WIN32) 
	TARGET_LINK_LIBRARIES(${TARGET_NAME} ${X_LIBS}  )
ENDIF()

# Where to put files during install
INSTALL(TARGETS ${TARGET_NAME}  RUNTIME DESTINATION bin ARCHIVE DESTINATION lib LIBRARY DESTINATION bin )
