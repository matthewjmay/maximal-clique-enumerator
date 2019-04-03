#include <random>
#include <algorithm>
#include <iostream>
#include "load_balancer.h"

bool LoadBalancer::requestJob(size_t requestingThreadId) {
    std::lock_guard<std::mutex> block(barrier_);
    if (requestingThreadId > threadContexts_.size()) {
        return false;
    }
    ThreadContext *requestingThread = threadContexts_[requestingThreadId];

    std::vector<size_t> threadIds;
    for (auto &t : threadContexts_) {
        if (t.first != requestingThreadId) {
            threadIds.push_back(t.first);
        }
    }

    // randomly shuffle thread steal order for even load balancing
    unsigned int seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::shuffle(threadIds.begin(), threadIds.end(), std::default_random_engine(seed));

    for (size_t tid : threadIds) {
        auto stolenJob = threadContexts_[tid]->stealJob();
        if (stolenJob.first) {
            requestingThread->setClique(std::move(stolenJob.second));
            requestingThread->enqueueJob(stolenJob.first);
            return true;
        }
    }

    return false;
}

void LoadBalancer::addThread(size_t threadId, ThreadContext *c) {
    std::lock_guard<std::mutex> block(barrier_);
    threadContexts_.insert({ threadId, c });
}

void LoadBalancer::removeThread(size_t threadId) {
    std::lock_guard<std::mutex> block(barrier_);
    threadContexts_.erase(threadId);
}
