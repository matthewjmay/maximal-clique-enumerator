#ifndef MAXIMAL_CLIQUE_ENUMERATOR_MAXIMAL_CLIQUE_ENUMERATOR_H
#define MAXIMAL_CLIQUE_ENUMERATOR_MAXIMAL_CLIQUE_ENUMERATOR_H


#include <stack>
#include <bits/unique_ptr.h>
#include "graph.h"
#include "thread_context.h"
#include "load_balancer.h"

class MaximalCliqueEnumerator {
    struct MaximalCliqueEnumeratorImpl;
    std::unique_ptr<MaximalCliqueEnumeratorImpl> pImpl_;

    void generateInitialCliques();
    void enumerateCliques();
    void expandClique(SearchTreeNode *);
    void outputClique(vertex_id cliqueSize);
    vertex_id findPivot(SearchTreeNode *) const;
    void printClique(vertex_id cliqueSize);
public:
    MaximalCliqueEnumerator(size_t workerID, const Graph &g, LoadBalancer &b, std::ostream &out, size_t numThreads, std::mutex &outputBarrier);
    MaximalCliqueEnumerator(MaximalCliqueEnumerator&&) noexcept;
    ~MaximalCliqueEnumerator();
    void operator()();
};


#endif //MAXIMAL_CLIQUE_ENUMERATOR_MAXIMAL_CLIQUE_ENUMERATOR_H
