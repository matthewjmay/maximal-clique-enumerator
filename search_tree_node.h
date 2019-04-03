//
// Contains all config information for a single search tree node. Note clique is stored per thread, and can
// be derived from a search tree node, its cliqueSize, and the thread that generated it.
//

#ifndef MAXIMAL_CLIQUE_ENUMERATOR_SEARCH_TREE_NODE_H
#define MAXIMAL_CLIQUE_ENUMERATOR_SEARCH_TREE_NODE_H


#include <cstddef>
#include "graph.h"

struct SearchTreeNode {
    // analogous to level in search tree
    size_t cliqueSize;
    std::vector<vertex_id> visited;
    std::vector<vertex_id> candidates;
    vertex_id newCliqueVertex;
};


#endif //MAXIMAL_CLIQUE_ENUMERATOR_SEARCH_TREE_NODE_H
