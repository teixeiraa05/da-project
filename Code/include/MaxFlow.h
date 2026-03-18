#include "Graph.h"

// Function to test the given vertex 'w' and visit it if conditions are met
template <class T>
void testAndVisit(std::queue< Vertex<T>*> &q, Edge<T> *e, Vertex<T> *w, double residual) {
    // Check if the vertex 'w' is not visited and there is residual capacity
    if (! w->isVisited() && residual > 0) {
        // Mark 'w' as visited, set the path through which it was reached, and enqueue it
        w->setVisited(true);
        w->setPath(e);
        q.push(w);
    }
}

// Function to find an augmenting path using Breadth-First Search
template <class T>
bool findAugmentingPath(Graph<T> *g, Vertex<T> *s, Vertex<T> *t) {
    // Mark all vertices as not visited
    for(auto v : g->getVertexSet()) {
        v->setVisited(false);
    }

    std::queue<Vertex<T>*> q;
    q.push(s);
    s->setVisited(true);

   while (!q.empty()) {
       auto v = q.front();
       q.pop();

       for (auto e : v->getAdj()) {
           auto dest = e->getDest();
           double resid_capacity = e->getWeight() - e->getFlow();
           testAndVisit(q, e, dest, resid_capacity);
       }

       for (auto e : v->getIncoming()) {
           auto dest = e->getOrig();
           double resid_capacity = e->getFlow();
           testAndVisit(q, e, dest, resid_capacity);
       }
   }
    // Return true if a path to the target is found, false otherwise
    return t->isVisited();
}

// Function to find the minimum residual capacity along the augmenting path
template <class T>
double findMinResidualAlongPath(Vertex<T> *s, Vertex<T> *t) {
    double f = INF;
    
    auto v = t;
    while (v != s) {
        auto e = v->getPath();

        if (e->getDest() == v) {
            f = std::min(f, e->getWeight() - e->getFlow());
            v = e->getOrig();
        } else {
            f = std::min(f, e->getFlow());
            v = e->getDest();
        }
    }

    return f;
}

// Function to augment flow along the augmenting path with the given flow value
template <class T>
void augmentFlowAlongPath(Vertex<T> *s, Vertex<T> *t, double f) {
    // Traverse the augmenting path and update the flow values accordingly
 
    auto v = t;
    while (v != s) {
        auto e = v->getPath();
        if (e->getDest() == v) {
            e->setFlow(e->getFlow()+f);
            v = e->getOrig();
        } else {
            e->setFlow(e->getFlow()-f);
            v = e->getDest();
        }
    }
}

// Main function implementing the Edmonds-Karp algorithm
template <class T>
void edmondsKarp(Graph<T> *g, int source, int sink) {
    // Find source and target vertices in the graph
    Vertex<T>* s = g->findVertex(source);
    Vertex<T>* t = g->findVertex(sink);
    for (auto v : g->getVertexSet()) {
        auto edges = v->getAdj();
        for (auto e : edges) {
            e->setFlow(0);
        }
    }
   
    while (findAugmentingPath(g, s, t)) {
        double resid = findMinResidualAlongPath(s,t);
        augmentFlowAlongPath(s, t, resid);
    }
}
