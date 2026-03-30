#pragma once
#include "Graph.h"

/**
 * @file EdmondsKarp.h
 * @brief Edmonds-Karp Max-Flow algorithm implementation.
 */

/**
 * @brief Checks if a vertex should be visited and enqueues it if so.
 *
 * Used internally by findAugmentingPath() during BFS traversal.
 *
 * @tparam T Vertex info type.
 * @param q        BFS queue.
 * @param e        Edge being considered.
 * @param w        Destination vertex.
 * @param residual Residual capacity of the edge.
 */
template <class T>
void testAndVisit(std::queue<Vertex<T> *> &q, Edge<T> *e, Vertex<T> *w,
		  double residual);

/**
 * @brief Finds an augmenting path from s to t using BFS.
 *
 * Sets the path field of each visited vertex to the edge used to reach it,
 * enabling path reconstruction in augmentFlowAlongPath().
 *
 * @tparam T Vertex info type.
 * @param g Pointer to the flow graph.
 * @param s Source vertex.
 * @param t Sink vertex.
 * @return true if an augmenting path exists, false otherwise.
 */
template <class T>
bool findAugmentingPath(Graph<T> *g, Vertex<T> *s, Vertex<T> *t);

/**
 * @brief Finds the minimum residual capacity along the augmenting path.
 *
 * Traverses the path from t back to s using the path field set by
 * findAugmentingPath(), computing the bottleneck capacity.
 *
 * @tparam T Vertex info type.
 * @param s Source vertex.
 * @param t Sink vertex.
 * @return Minimum residual capacity (bottleneck) along the path.
 */
template <class T> double findMinResidualAlongPath(Vertex<T> *s, Vertex<T> *t);

/**
 * @brief Augments flow along the augmenting path by f units.
 *
 * Traverses from t back to s, increasing forward edge flows by f
 * and decreasing reverse edge flows by f.
 *
 * @tparam T Vertex info type.
 * @param s Source vertex.
 * @param t Sink vertex.
 * @param f Flow value to augment (bottleneck).
 */
template <class T>
void augmentFlowAlongPath(Vertex<T> *s, Vertex<T> *t, double f);

/**
 * @brief Runs the Edmonds-Karp Max-Flow algorithm on a graph.
 *
 * Repeatedly finds shortest augmenting paths (BFS) from source to sink
 * and augments flow until no augmenting path exists. Resets all edge
 * flows to 0 before starting.
 *
 * @tparam T Vertex info type.
 * @param g      Pointer to the flow graph.
 * @param source Info value of the source vertex.
 * @param sink   Info value of the sink vertex.
 *
 * @note The flow values are stored directly on the graph edges after
 * completion. Use edge->getFlow() to retrieve individual assignment results.
 *
 * @complexity Time: O(V * E^2), where V = number of vertices and E = number of
 * edges. This is the key advantage over Ford-Fulkerson — the BFS guarantees
 * shortest augmenting paths, bounding iterations to O(V*E).
 */
template <class T> void edmondsKarp(Graph<T> *g, int source, int sink);

/**
 * @brief Visits a neighbor during BFS if it is reachable in the residual graph.
 *
 * Marks the vertex as visited, stores the edge used to reach it,
 * and enqueues it for further BFS expansion.
 */
template <class T>
void testAndVisit(std::queue<Vertex<T> *> &q, Edge<T> *e, Vertex<T> *w,
		  double residual) {
    if (!w->isVisited() && residual > 0) {
	w->setVisited(true);
	w->setPath(e);
	q.push(w);
    }
}

/**
 * @brief Performs BFS on the residual graph to find an augmenting path.
 * @return true if sink t is reachable from source s, false otherwise.
 */
template <class T>
bool findAugmentingPath(Graph<T> *g, Vertex<T> *s, Vertex<T> *t) {
    for (auto v : g->getVertexSet()) {
	v->setVisited(false);
	v->setPath(nullptr);
    }

    std::queue<Vertex<T> *> q;
    s->setVisited(true);
    q.push(s);

    while (!q.empty() && !t->isVisited()) {
	auto v = q.front();
	q.pop();

	// Traverse residual forward edges.
	for (auto e : v->getAdj()) {
	    testAndVisit(q, e, e->getDest(), e->getWeight() - e->getFlow());
	}

	// Traverse residual backward edges.
	for (auto e : v->getIncoming()) {
	    testAndVisit(q, e, e->getOrig(), e->getFlow());
	}
    }

    return t->isVisited();
}

/**
 * @brief Computes the bottleneck (minimum residual capacity) on the current
 * path.
 *
 * Walks backwards from sink to source using `path` pointers set by BFS.
 */
template <class T> double findMinResidualAlongPath(Vertex<T> *s, Vertex<T> *t) {
    double f = INF;

    for (auto v = t; v != s;) {
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

/**
 * @brief Pushes flow value `f` through the augmenting path.
 *
 * Increases forward-edge flow and decreases backward-edge flow according
 * to the direction stored in each vertex path edge.
 */
template <class T>
void augmentFlowAlongPath(Vertex<T> *s, Vertex<T> *t, double f) {
    for (auto v = t; v != s;) {
	auto e = v->getPath();

	if (e->getDest() == v) {
	    e->setFlow(e->getFlow() + f);
	    v = e->getOrig();
	} else {
	    e->setFlow(e->getFlow() - f);
	    v = e->getDest();
	}
    }
}

/**
 * @brief Executes Edmonds-Karp until no augmenting path exists.
 *
 * Initializes all edge flows to zero and then repeatedly:
 * 1) finds an augmenting path with BFS,
 * 2) computes the bottleneck,
 * 3) augments flow along the path.
 */
template <class T> void edmondsKarp(Graph<T> *g, int source, int sink) {
    auto s = g->findVertex(source);
    auto t = g->findVertex(sink);

    if (s == nullptr || t == nullptr) {
	throw std::logic_error("Source or sink vertex not found in graph");
    }

    for (auto v : g->getVertexSet()) {
	for (auto e : v->getAdj()) {
	    e->setFlow(0);
	}
    }

    while (findAugmentingPath(g, s, t)) {
	double f = findMinResidualAlongPath(s, t);
	augmentFlowAlongPath(s, t, f);
    }
}