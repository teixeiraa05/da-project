#include "Graph.h"
#include <limits>

// Function to perform DFS and find an augmenting path
template <class T>
bool dfsFindAugmentingPath(Vertex<T>* v, Vertex<T>* t, double& flow) {
    if (v == t) return true;
    v->setVisited(true);

     // TO DO
    for (auto e : v->getAdj()) {
        auto dest = e->getDest();
        double resid_capacity = e->getWeight() - e->getFlow();
        if (!dest->isVisited() && resid_capacity > 0) {
            double min_flow = std::min(flow, resid_capacity);
            if (dfsFindAugmentingPath(dest, t, min_flow)) {
                    e->setFlow(e->getFlow() + min_flow);
                    flow = min_flow;
                return true;
            }
        }
    }

    for (auto e : v->getIncoming()) {
        auto dest = e->getOrig();
        double resid_capacity = e->getFlow();
        if (!dest->isVisited() && resid_capacity > 0) {
            double min_flow = std::min(flow, resid_capacity);
            if (dfsFindAugmentingPath(dest, t, min_flow)) {
                    e->setFlow(e->getFlow() - min_flow);
                    flow = min_flow;
                return true;
            }
        }
    }
    return false;
}

// Ford-Fulkerson algorithm
template <class T>
void fordFulkerson(Graph<T>* g, int source, int target) {
    Vertex<T>* s = g->findVertex(source);
    Vertex<T>* t = g->findVertex(target);

    if (s == nullptr || t == nullptr || s == t)
        throw std::logic_error("Invalid source and/or target vertex");

    // TO DO
    for (auto v : g->getVertexSet()) {
        auto edges = v->getAdj();
        for (auto e : edges) {
            e->setFlow(0);
        }
    }

    double max_flow = 0;
    double flow = INF;
    for (auto v : g->getVertexSet()) v->setVisited(false);
    while (dfsFindAugmentingPath(s, t, flow)) {
        max_flow += flow;
        flow = INF;
        for (auto v : g->getVertexSet()) v->setVisited(false);
    }
};
