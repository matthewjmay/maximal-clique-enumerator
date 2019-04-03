cmake_minimum_required(VERSION 3.5)
project(maximal_clique_enumerator)

set(CMAKE_CXX_STANDARD 14)
SET(CMAKE_CXX_FLAGS -pthread)

set(SOURCE_FILES main.cc graph.cc graph.h maximal_clique_enumerator.cc maximal_clique_enumerator.h search_tree_node.cc search_tree_node.h load_balancer.cc load_balancer.h thread_context.cc thread_context.h)
add_executable(maximal_clique_enumerator ${SOURCE_FILES})