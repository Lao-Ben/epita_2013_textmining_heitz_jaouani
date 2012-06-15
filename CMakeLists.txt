CMAKE_MINIMUM_REQUIRED (VERSION 2.8)

PROJECT (TMLN CXX)

#set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake-modules)


# find_package(Dmalloc)
# if (Dmalloc_FOUND)
#   include_directories(${Dmalloc_INCLUDE_DIRS})
#   target_link_libraries (Dmalloc ${BZIP2_LIBRARIES})
# endif (Dmalloc_FOUND)


SET (CMAKE_CXX_COMPILER "g++")


IF ((DEFINED ENV{DEBUG}) AND ("$ENV{DEBUG}" EQUAL 1))
  # Flags de debug
  message("Debug mode")
  SET (CMAKE_CXX_FLAGS
    "-W -Wall -Wextra -pedantic -pthread -std=c++0x -O0 -g -ggdb -Wpadded")

ELSE()
  # Flags de batard
  message("Release mode")
  SET (CMAKE_CXX_FLAGS
    "-W -Wall -Wextra -pedantic -pthread -std=c++0x -O3")
ENDIF()




SET (TMLN_SOURCE_DIR "src")

# Documentation
ADD_CUSTOM_TARGET (doc doxygen ./doc/doc.conf)


INCLUDE_DIRECTORIES (
  "${TMLN_SOURCE_DIR}/"
)



FILE (GLOB_RECURSE TMLN_SRC_PATRICIA_TREE "${TMLN_SOURCE_DIR}/patricia_tree/*.cc")


FILE (GLOB_RECURSE TMLN_SRC_COMPILER "${TMLN_SOURCE_DIR}/compiler/*.cc")
#ADD_EXECUTABLE (TextMiningCompiler ${TMLN_SRC_COMPILER} ${TMLN_SRC_PATRICIA_TREE})

ADD_EXECUTABLE (TextMiningCompiler
  ${TMLN_SRC_COMPILER}
  ${TMLN_SOURCE_DIR}/patricia_tree/abstract_patricia_tree.cc
  ${TMLN_SOURCE_DIR}/patricia_tree/abstract_patricia_tree_node.cc
  ${TMLN_SOURCE_DIR}/patricia_tree/patricia_tree_compiler.cc
  ${TMLN_SOURCE_DIR}/patricia_tree/patricia_tree_node_compiler.cc
  )



FILE (GLOB_RECURSE TMLN_SRC_APP "${TMLN_SOURCE_DIR}/app/*.cc")
#ADD_EXECUTABLE (TextMiningApp ${TMLN_SRC_APP} ${TMLN_SRC_PATRICIA_TREE})

ADD_EXECUTABLE (TextMiningApp
  ${TMLN_SRC_APP}
  ${TMLN_SOURCE_DIR}/patricia_tree/abstract_patricia_tree.cc
  ${TMLN_SOURCE_DIR}/patricia_tree/abstract_patricia_tree_node.cc
  ${TMLN_SOURCE_DIR}/patricia_tree/patricia_tree_app.cc
  ${TMLN_SOURCE_DIR}/patricia_tree/patricia_tree_node_app.cc
  ${TMLN_SOURCE_DIR}/patricia_tree/search_result.cc
  ${TMLN_SOURCE_DIR}/patricia_tree/minion.cc
  ${TMLN_SOURCE_DIR}/patricia_tree/thread_pool.cc
  )

set_property(
TARGET TextMiningApp
PROPERTY COMPILE_DEFINITIONS NBCORE=$ENV{NBTHREAD}
)



add_custom_target(
check
DEPENDS TextMiningApp TextMiningCompiler
COMMAND cd ./check/ && ./check.sh
)


SET_DIRECTORY_PROPERTIES(PROPERTIES
  ADDITIONAL_MAKE_CLEAN_FILES
  "doc/html;doc/latex;doc/man"
  )
