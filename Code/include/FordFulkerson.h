#pragma once
#include "Graph.h"
#include <limits>

/**
 * @file FordFulkerson.h
 * @brief Ford-Fulkerson Max-Flow algorithm implementation using DFS.
 */

/**
 * @brief Recursively finds an augmenting path using DFS and augments flow.
 *
 * Explores forward edges (with positive residual capacity) and backward
 * edges (with positive flow) to find a path from v to t. Updates flow
 * values along the path as it unwinds the recursion.
 *
 * @tparam T Vertex info type.
 * @param v    Current vertex being explored.
 * @param t    Target (sink) vertex.
 * @param flow In/out parameter: current minimum flow along the path.
 *             Initialised to INF by the caller, reduced to the bottleneck.
 * @return true if an augmenting path to t was found, false otherwise.
 *
 */
template <class T>
bool dfsFindAugmentingPath(Vertex<T>* v, Vertex<T>* t, double& flow);

/**
 * @brief Runs the Ford-Fulkerson Max-Flow algorithm on a graph.
 *
 * Repeatedly calls dfsFindAugmentingPath() until no augmenting path
 * exists. Resets all edge flows to 0 before starting.
 *
 * Compared to Edmonds-Karp, Ford-Fulkerson uses DFS instead of BFS.
 * This means it does not guarantee shortest augmenting paths, so the
 * number of iterations depends on the max flow value rather than graph
 * structure — making it potentially slower on large instances with high
 * capacity edges.
 *
 * @tparam T Vertex info type.
 * @param g      Pointer to the flow graph.
 * @param source Info value of the source vertex.
 * @param target Info value of the sink vertex.
 *
 * @throws std::logic_error if source or target vertex is not found in the graph.
 *
 * @note The flow values are stored directly on the graph edges after completion.
 *       Use edge->getFlow() to retrieve individual assignment results.
 *
 * @complexity Time: O(E * F), where F is the max-flow value.
 *                    In practice this is fast for this problem since capacities
 *                    are small integers, but theoretically unbounded for large
 *                    capacity values.
 */
template <class T>
void fordFulkerson(Graph<T>* g, int source, int target);

/**
 * @brief DFS search for an augmenting path in the residual graph.
 *
 * Tries forward residual edges first, then backward residual edges.
 * When a path reaches the sink, updates edge flows while recursion unwinds.
 */
template <class T>
bool dfsFindAugmentingPath(Vertex<T>* v, Vertex<T>* t, double& flow) {
	if (v == t) {
		return true;
	}

	v->setVisited(true);

	// Try forward residual edges.
	for (auto e : v->getAdj()) {
		auto w = e->getDest();
		double residual = e->getWeight() - e->getFlow();

		if (!w->isVisited() && residual > 0) {
			double bottleneck = std::min(flow, residual);
			if (dfsFindAugmentingPath(w, t, bottleneck)) {
				e->setFlow(e->getFlow() + bottleneck);
				flow = bottleneck;
				return true;
			}
		}
	}

	// Try backward residual edges.
	for (auto e : v->getIncoming()) {
		auto w = e->getOrig();
		double residual = e->getFlow();

		if (!w->isVisited() && residual > 0) {
			double bottleneck = std::min(flow, residual);
			if (dfsFindAugmentingPath(w, t, bottleneck)) {
				e->setFlow(e->getFlow() - bottleneck);
				flow = bottleneck;
				return true;
			}
		}
	}

	return false;
}

/**
 * @brief Executes Ford-Fulkerson using DFS-based augmenting paths.
 *
 * Initializes all flows to zero and repeatedly runs DFS to find and
 * apply augmenting paths until none can be found.
 */
template <class T>
void fordFulkerson(Graph<T>* g, int source, int target) {
	auto s = g->findVertex(source);
	auto t = g->findVertex(target);

	if (s == nullptr || t == nullptr) {
		throw std::logic_error("Source or target vertex not found in graph");
	}

	for (auto v : g->getVertexSet()) {
		for (auto e : v->getAdj()) {
			e->setFlow(0);
		}
	}

	while (true) {
		for (auto v : g->getVertexSet()) {
			v->setVisited(false);
		}

		double f = INF;
		if (!dfsFindAugmentingPath(s, t, f)) {
			break;
		}
	}
}