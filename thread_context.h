#ifndef MAXIMAL_CLIQUE_ENUMERATOR_THREAD_CONTEXT_H
#define MAXIMAL_CLIQUE_ENUMERATOR_THREAD_CONTEXT_H

#include <stack>
#include "search_tree_node.h"
#include "graph.h"
#include <mutex>

class ThreadContext {
    // mutex to protect access to jobs stack
    std::mutex barrier_;

    // current clique for this thread
    std::vector<vertex_id> workingClique_;

    // stack of search tree nodes to be processed
    std::stack<SearchTreeNode*> jobs_;

public:
    ThreadContext();
    SearchTreeNode* getNextJob();
    std::pair<SearchTreeNode*, std::vector<vertex_id> > stealJob();
    void enqueueJob(SearchTreeNode* job);
    void expandClique(size_t cliqueSize, vertex_id v);
    vertex_id cliqueAt(vertex_id i) const;
    void setClique(std::vector<vertex_id> &&clique);
    void setClique(const std::vector<vertex_id> &clique);
};


#endif //MAXIMAL_CLIQUE_ENUMERATOR_THREAD_CONTEXT_H
