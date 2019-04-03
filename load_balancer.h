#ifndef MAXIMAL_CLIQUE_ENUMERATOR_LOAD_BALANCER_H
#define MAXIMAL_CLIQUE_ENUMERATOR_LOAD_BALANCER_H

#include <stack>
#include <vector>
#include <map>
#include "thread_context.h"

class LoadBalancer {
    std::map<size_t, ThreadContext*> threadContexts_;
    std::mutex barrier_;
public:
    bool requestJob(size_t requestingThreadId);
    void addThread(size_t threadId, ThreadContext *);
    void removeThread(size_t threadId);
};


#endif //MAXIMAL_CLIQUE_ENUMERATOR_LOAD_BALANCER_H
