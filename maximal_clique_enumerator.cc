#include <iostream>
#include <thread>
#include "maximal_clique_enumerator.h"

struct MaximalCliqueEnumerator::MaximalCliqueEnumeratorImpl {
    // workerID used to determine which subset of vertices this worker initially handles
    size_t workerID_;
    const Graph &g_;
    ThreadContext context_;
    LoadBalancer &balancer_;
    std::ostream &out_;
    std::mutex &outputBarrier_;
    size_t numThreads_;

    MaximalCliqueEnumeratorImpl(size_t workerID, const Graph &g, LoadBalancer &b, std::ostream &out, size_t numThreads, std::mutex &outputBarrier) : workerID_{
            workerID}, g_{g}, balancer_{b}, out_{out}, numThreads_{numThreads}, outputBarrier_{outputBarrier} {
    }
};

MaximalCliqueEnumerator::MaximalCliqueEnumerator(size_t workerID, const Graph &g, LoadBalancer &b, std::ostream &out, size_t numThreads, std::mutex &outputBarrier) : pImpl_{new MaximalCliqueEnumeratorImpl{
        workerID, g, b, out, numThreads, outputBarrier}
} {}

void MaximalCliqueEnumerator::operator()() {
    pImpl_->balancer_.addThread(pImpl_->workerID_, &pImpl_->context_);
    generateInitialCliques();
    enumerateCliques();
    pImpl_->balancer_.removeThread(pImpl_->workerID_);
}

void MaximalCliqueEnumerator::generateInitialCliques() {
    // std::cout << "generating initial cliques" << std::endl;
    // create initial cliques of size 1
    // use ID to find subset of vertices this algorithm instance is responsible for
    vertex_id numVertices = pImpl_->g_.getNumVertices();
    for (size_t rootVertex = pImpl_->workerID_; rootVertex < numVertices; rootVertex += pImpl_->numThreads_) {
        auto *node = new SearchTreeNode{};
        node->newCliqueVertex = rootVertex;
        node->cliqueSize = 1;
        for (size_t neighbor : pImpl_->g_.getNeighbors(rootVertex)) {
            if (neighbor < rootVertex) {
                // add to NOT list, respecting vertex ordering
                node->visited.push_back(neighbor);
            } else {
                node->candidates.push_back(neighbor);
            }
        }
        // add this to potential cliques to be processed
        pImpl_->context_.enqueueJob(node);
    }
}

MaximalCliqueEnumerator::MaximalCliqueEnumerator(MaximalCliqueEnumerator &&other) noexcept {
    pImpl_ = std::move(other.pImpl_);
    other.pImpl_ = nullptr;
}

void MaximalCliqueEnumerator::enumerateCliques() {
    // std::cout << "enumerating cliques" << std::endl;
    SearchTreeNode *currJob;
    while (true) {
        currJob = pImpl_->context_.getNextJob();
        if (currJob) {
            // there is work to do on this local thread
            expandClique(currJob);
            delete currJob;
        } else {
            // steal from another thread
            if (!pImpl_->balancer_.requestJob(pImpl_->workerID_)) {
                std::cout << "Could not steal job, thread " << pImpl_->workerID_ << " exiting!" << std::endl;
                break;
            } else {
                std::cout << "successfully stole job!!!" << std::endl;
            }
        }
    }
}

void MaximalCliqueEnumerator::expandClique(SearchTreeNode *n) {
    std::cout << "expanding clique" << std::endl;
    pImpl_->context_.expandClique(n->cliqueSize, n->newCliqueVertex);
    printClique(n->cliqueSize);
    if (n->candidates.empty()) {
        if (n->visited.empty()) {
            outputClique(n->cliqueSize);
        }
        return;
    }
//    std::cout << "CANDIDATES:\n";
//    for (vertex_id can : n->candidates) {
//        std::cout << can << " ";
//    }
//    std::cout << std::endl;
//
//    std::cout << "VISITED:\n";
//    for (vertex_id vis : n->visited) {
//        std::cout << vis << " ";
//    }
//    std::cout << std::endl;

    vertex_id pivot = findPivot(n);
    // std::cout << "using pivot " << pivot << std::endl;
    for (long int i = n->candidates.size() - 1; i >= 0; --i) {
        vertex_id v = n->candidates[i];
        // std::cout << "processing cand " << v << std::endl;
        if (pImpl_->g_.isConnected(pivot, v)) {
            continue;
        }
        auto *node = new SearchTreeNode{};
        node->newCliqueVertex = v;
        node->cliqueSize = n->cliqueSize + 1;

        // build new candidates
        for (vertex_id can : n->candidates) {
            if (pImpl_->g_.isConnected(v, can)) {
                node->candidates.push_back(can);
            }
        }

        // build new visited
        for (vertex_id vis : n->visited) {
            if (pImpl_->g_.isConnected(v, vis)) {
                node->visited.push_back(vis);
            }
        }
        pImpl_->context_.enqueueJob(node);
        // std::cout << "enqueued job from adding candidate " << v << std::endl;
        n->visited.push_back(v);

        // remove from candidates by swapping with last element and popping
        std::swap(n->candidates[i], n->candidates[n->candidates.size() - 1]);
        n->candidates.pop_back();
    }
}

void MaximalCliqueEnumerator::printClique(vertex_id cliqueSize) {
    std::cout << "CURRENT CLIQUE (" << cliqueSize << "):" << std::endl;
    for (vertex_id i = 0; i < cliqueSize; ++i) {
        std::cout << pImpl_->context_.cliqueAt(i);
        if (i != cliqueSize - 1) {
            std::cout << " ";
        }
    }
    std::cout << std::endl;
}

void MaximalCliqueEnumerator::outputClique(vertex_id cliqueSize) {
    std::lock_guard<std::mutex> block(pImpl_->outputBarrier_);
    std::cout << "thread " << pImpl_->workerID_ << " generated clique of size " << cliqueSize << std::endl;
    for (vertex_id i = 0; i < cliqueSize; ++i) {
        pImpl_->out_ << pImpl_->context_.cliqueAt(i);
        if (i != cliqueSize - 1) {
            pImpl_->out_ << " ";
        }
    }
    pImpl_->out_ << std::endl;
}

vertex_id MaximalCliqueEnumerator::findPivot(SearchTreeNode *n) const {
    vertex_id currMinNonConnectivity = n->candidates.size(), currPivot = 0;

    for (vertex_id i : n->candidates) {
        vertex_id nonConnectivity = 0;
        for (vertex_id j : n->candidates) {
            if (!pImpl_->g_.isConnected(i, j)) {
                if (++nonConnectivity > currMinNonConnectivity) {
                    break;
                }
            }
        }
        if (nonConnectivity <= currMinNonConnectivity) {
            currMinNonConnectivity = nonConnectivity;
            currPivot = i;
        }
    }

    for (vertex_id i : n->visited) {
        vertex_id nonConnectivity = 0;
        for (vertex_id j : n->candidates) {
            if (!pImpl_->g_.isConnected(i, j)) {
                if (++nonConnectivity > currMinNonConnectivity) {
                    break;
                }
            }
        }
        if (nonConnectivity <= currMinNonConnectivity) {
            currMinNonConnectivity = nonConnectivity;
            currPivot = i;
        }
    }
    return currPivot;
}

MaximalCliqueEnumerator::~MaximalCliqueEnumerator() = default;


