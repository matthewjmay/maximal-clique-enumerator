#include <iostream>
#include "thread_context.h"

SearchTreeNode *ThreadContext::getNextJob() {
    std::lock_guard<std::mutex> block(barrier_);

    if (jobs_.empty()) {
        return nullptr;
    }
    SearchTreeNode *ret = jobs_.top();
    jobs_.pop();
    return ret;

}

void ThreadContext::enqueueJob(SearchTreeNode *job) {
    std::lock_guard<std::mutex> block(barrier_);
    jobs_.push(job);
}

std::pair<SearchTreeNode*, std::vector<vertex_id> > ThreadContext::stealJob() {
    std::lock_guard<std::mutex> block(barrier_);
    if (jobs_.empty()) {
        return { nullptr, std::vector<vertex_id>{} };
    }
    SearchTreeNode *stolenJob = jobs_.top();
    jobs_.pop();

    return { stolenJob, workingClique_ };
}

void ThreadContext::expandClique(size_t newCliqueSize, vertex_id v) {
    std::lock_guard<std::mutex> block(barrier_);
    if (workingClique_.size() == newCliqueSize - 1) {
        workingClique_.push_back(v);
    } else if (workingClique_.size() >= newCliqueSize) {
        workingClique_[newCliqueSize - 1] = v;
    } else {
        throw "Cannot expand clique by more than 1 item";
    }
}

vertex_id ThreadContext::cliqueAt(vertex_id i) const {
    return workingClique_[i];
}

void ThreadContext::setClique(std::vector<vertex_id> &&clique) {
    std::lock_guard<std::mutex> block(barrier_);
    std::swap(workingClique_, clique);
}

void ThreadContext::setClique(const std::vector<vertex_id> &clique) {
    std::lock_guard<std::mutex> block(barrier_);
    workingClique_ = clique;
}

ThreadContext::ThreadContext() = default;
