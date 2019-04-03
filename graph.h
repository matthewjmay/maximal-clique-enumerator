#ifndef MAXIMAL_CLIQUE_ENUMERATOR_GRAPH_H
#define MAXIMAL_CLIQUE_ENUMERATOR_GRAPH_H

#include <vector>
#include <cstddef>
#include <string>

typedef unsigned long vertex_id;

class Graph {
    // TODO: faster lookup with adjacency matrix or hash table?
    std::vector<std::vector<vertex_id> > adjacency_list_;

    friend std::ostream& operator<<(std::ostream& out, const Graph& graph);
public:
    // filename with first line containing number of vertices n, and the next n lines containing edges
    explicit Graph(std::string fileName);

    bool isConnected(vertex_id v1, vertex_id v2) const;
    vertex_id getDegree(vertex_id) const;
    void addEdge(vertex_id v1, vertex_id v2);
    vertex_id getNumVertices() const;
    const std::vector<vertex_id> &getNeighbors(vertex_id v) const;
};

#endif //MAXIMAL_CLIQUE_ENUMERATOR_GRAPH_H
