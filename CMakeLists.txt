CMAKE_MINIMUM_REQUIRED (VERSION 2.8)

PROJECT (TMLN CXX)

#set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake-modules)


# find_package(Dmalloc)
# if (Dmalloc_FOUND)
#   include_directories(${Dmalloc_INCLUDE_DIRS})
#   target_link_libraries (Dmalloc ${BZIP2_LIBRARIES})
# endif (Dmalloc_FOUND)

SET (CMAKE_CXX_COMPILER "g++")
#SET (CMAKE_CXX_FLAGS "-W -Wall -Wextra -pedantic -pthread -std=c++0x -O0 -g -ggdb")
SET (CMAKE_CXX_FLAGS "-W -Wall -Wextra -pedantic -pthread -std=c++0x -O3")

SET (TMLN_SOURCE_DIR "src")

ADD_CUSTOM_TARGET (doc doxygen doc/doc.conf WORKING_DIRECTORY ${TMLN_SOURCE_DIR})


INCLUDE_DIRECTORIES (
  "${TMLN_SOURCE_DIR}/"
)



FILE (GLOB_RECURSE TMLN_SRC_PATRICIA_TREE "${TMLN_SOURCE_DIR}/patricia_tree/*.cc")


FILE (GLOB_RECURSE TMLN_SRC_COMPILER "${TMLN_SOURCE_DIR}/compiler/*.cc")
ADD_EXECUTABLE (TextMiningCompiler ${TMLN_SRC_COMPILER} ${TMLN_SRC_PATRICIA_TREE})


FILE (GLOB_RECURSE TMLN_SRC_APP "${TMLN_SOURCE_DIR}/app/*.cc")
ADD_EXECUTABLE (TextMiningApp ${TMLN_SRC_APP} ${TMLN_SRC_PATRICIA_TREE})


add_custom_target(
check
DEPENDS TextMiningApp TextMiningCompiler
COMMAND cd ./check/ && ./check.sh
1000 # Number of test lines
)
