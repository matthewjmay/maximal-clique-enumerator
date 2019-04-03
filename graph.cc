#include "graph.h"
#include <fstream>
#include <sstream>
#include <iostream>

Graph::Graph(std::string fileName) {
    std::ifstream inFile;
    inFile.open(fileName);
    if (!inFile) {
        std::cerr << "Cannot read graph from " << fileName << std::endl;
        exit(1);
    }

    // first line of file contains the number of vertices in this graph
    vertex_id numVertices;
    inFile >> numVertices;
    adjacency_list_.resize(numVertices);

    vertex_id u = 0, v = 0;
    while (inFile >> u >> v) {
        addEdge(u, v);
    }
    inFile.close();
}

bool Graph::isConnected(vertex_id v1, vertex_id v2) const {
    vertex_id lowerDegreeVertex = getDegree(v1) < getDegree(v2) ? v1 : v2;
    for (const auto id : adjacency_list_[lowerDegreeVertex]) {
        if (id == v2) {
            return true;
        }
    }
    return false;
}

vertex_id Graph::getDegree(vertex_id id) const {
    return adjacency_list_[id].size();
}



void Graph::addEdge(vertex_id v1, vertex_id v2) {
    if (v1 > adjacency_list_.size() || v2 > adjacency_list_.size() || v1 == v2) {
        return;
    }
    adjacency_list_[v1].push_back(v2);
    adjacency_list_[v2].push_back(v1);
}

std::ostream &operator<<(std::ostream &out, const Graph &g) {
    out << "Number of Vertices: " << g.adjacency_list_.size() << std::endl;
    for (size_t i = 0; i < g.adjacency_list_.size(); ++i) {
        out << i << " |";
        for (const auto neighbor : g.adjacency_list_[i]) {
            out << " " << neighbor;
        }
        out << std::endl;
    }
    return out;
}

const std::vector<vertex_id> &Graph::getNeighbors(vertex_id v) const {
    return adjacency_list_[v];
}

vertex_id Graph::getNumVertices() const {
    return adjacency_list_.size();
}
