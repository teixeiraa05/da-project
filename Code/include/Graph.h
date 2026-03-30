// Original code by Gonçalo Leão
// Updated by DA 2024/2025 Team

#ifndef DA_TP_CLASSES_GRAPH
#define DA_TP_CLASSES_GRAPH

/**
 * @file Graph.h
 * @brief Generic directed weighted graph with flow support.
 *
 * Provides a templated implementation of a directed graph with:
 * - Weighted edges with flow tracking (for Max-Flow algorithms)
 * - Reverse edge pointers (for residual graph construction)
 * - BFS/DFS auxiliary fields (visited, path, dist)
 * - Incoming edge tracking per vertex
 *
 * Used as the underlying data structure for the flow network in the
 * Conference Assignment Tool.
 */

#include <iostream>
#include <vector>
#include <queue>
#include <limits>
#include <algorithm>

template <class T>
class Edge;

/// @brief Represents positive infinity for use as initial flow/distance value.
#define INF std::numeric_limits<double>::max()

/************************* Vertex  **************************/

/**
 * @class Vertex
 * @brief Represents a node in the graph.
 *
 * Each vertex stores its info value, a list of outgoing edges, a list of
 * incoming edges, and several auxiliary fields used by graph algorithms
 * (BFS, DFS, Tarjan SCC, topological sort, Prim, flow algorithms).
 *
 * @tparam T Type of the vertex identifier (e.g. int for node IDs).
 */
template <class T>
class Vertex {
public:
    /**
     * @brief Constructs a vertex with the given info value.
     * @param in Identifier/content of the vertex.
     * @complexity Time: O(1). Space: O(1).
     */
    Vertex(T in);

    /**
     * @brief Less-than operator required by MutablePriorityQueue.
     * @param vertex Vertex to compare against.
     * @return true if this vertex has smaller dist than vertex.
     * @complexity Time: O(1). Space: O(1).
     */
    bool operator<(Vertex<T>& vertex) const;

    // ── Getters ─────────────────────────────────────────────

    /** @brief Returns the info/identifier of this vertex. */
    T getInfo() const;

    /** @brief Returns the list of outgoing edges. */
    std::vector<Edge<T>*> getAdj() const;

    /** @brief Returns true if this vertex has been visited (used by BFS/DFS). */
    bool isVisited() const;

    /** @brief Returns true if this vertex is currently being processed (used by DAG detection). */
    bool isProcessing() const;

    /** @brief Returns the in-degree of this vertex (used by topological sort). */
    unsigned int getIndegree() const;

    /** @brief Returns the distance value (used by Dijkstra, Prim). */
    double getDist() const;

    /**
     * @brief Returns the edge used to reach this vertex in a path traversal.
     *
     * Set by BFS/DFS during augmenting path search in flow algorithms.
     * Used to reconstruct the augmenting path from sink back to source.
     */
    Edge<T>* getPath() const;

    /** @brief Returns the list of incoming edges. */
    std::vector<Edge<T>*> getIncoming() const;

    /** @brief Returns the low value used by Tarjan's SCC algorithm. */
    int getLow() const;

    /** @brief Returns the discovery number used by Tarjan's SCC algorithm. */
    int getNum() const;

    // ── Setters ─────────────────────────────────────────────

    /** @brief Sets the info/identifier of this vertex. */
    void setInfo(T info);

    /** @brief Sets the visited flag. */
    void setVisited(bool visited);

    /** @brief Sets the processing flag. */
    void setProcessing(bool processing);

    /** @brief Sets the low value (Tarjan SCC). */
    void setLow(int value);

    /** @brief Sets the discovery number (Tarjan SCC). */
    void setNum(int value);

    /** @brief Sets the in-degree of this vertex. */
    void setIndegree(unsigned int indegree);

    /** @brief Sets the distance value. */
    void setDist(double dist);

    /** @brief Sets the path edge used for path reconstruction in flow algorithms. */
    void setPath(Edge<T>* path);

    // ── Edge management ─────────────────────────────────────

    /**
     * @brief Adds an outgoing edge from this vertex to dest with weight w.
     *
     * Also registers the new edge in dest's incoming edge list.
     *
     * @param dest Destination vertex pointer.
     * @param w    Edge weight / capacity.
     * @return Pointer to the newly created Edge.
     * @complexity Time: O(1). Space: O(1).
     */
    Edge<T>* addEdge(Vertex<T>* dest, double w);

    /**
     * @brief Removes all outgoing edges to the vertex with info == in.
     *
     * Supports multigraphs — removes all matching edges, not just the first.
     * Also removes the corresponding entries from the destination's incoming list.
     *
     * @param in Info value of the destination vertex to disconnect from.
     * @return true if at least one edge was removed, false otherwise.
     * @complexity Time: O(E) where E = number of outgoing edges. Space: O(1).
     */
    bool removeEdge(T in);

    /**
     * @brief Removes all outgoing edges from this vertex.
     * @complexity Time: O(E) where E = number of outgoing edges. Space: O(1).
     */
    void removeOutgoingEdges();

protected:
    T info;                           ///< Vertex identifier / content
    std::vector<Edge<T>*> adj;        ///< Outgoing edges
    std::vector<Edge<T>*> incoming;   ///< Incoming edges

    bool         visited    = false;  ///< Used by BFS, DFS, Prim
    bool         processing = false;  ///< Used by DAG cycle detection
    int          low        = -1;     ///< Used by Tarjan SCC
    int          num        = -1;     ///< Used by Tarjan SCC
    unsigned int indegree;            ///< Used by topological sort
    double       dist       = 0;      ///< Used by Dijkstra, Prim
    Edge<T>*     path       = nullptr;///< Used by flow algorithms for path reconstruction
    int          queueIndex = 0;      ///< Required by MutablePriorityQueue and UFDS

    /**
     * @brief Deletes an edge and removes it from the destination's incoming list.
     * @param edge Pointer to the edge to delete.
     * @complexity Time: O(E_in) where E_in = number of incoming edges of destination.
     * @complexity Space: O(1).
     */
    void deleteEdge(Edge<T>* edge);
};

/********************** Edge  ****************************/

/**
 * @class Edge
 * @brief Represents a directed weighted edge with flow support.
 *
 * Each edge stores its origin, destination, weight (capacity), current flow
 * and a pointer to its reverse edge in the residual graph. The reverse edge
 * must be set explicitly via setReverse() when building a flow network and
 * is used by Edmonds-Karp and Ford-Fulkerson to update residual capacities
 * during flow augmentation.
 *
 * @tparam T Type of the vertex identifier.
 */
template <class T>
class Edge {
public:
    /**
     * @brief Constructs a directed edge from orig to dest with weight w.
     * @param orig Origin vertex pointer.
     * @param dest Destination vertex pointer.
     * @param w    Edge weight / capacity.
     * @complexity Time: O(1). Space: O(1).
     */
    Edge(Vertex<T>* orig, Vertex<T>* dest, double w);

    // ── Getters ─────────────────────────────────────────────

    /** @brief Returns the destination vertex of this edge. */
    Vertex<T>* getDest() const;

    /**
     * @brief Returns the weight (capacity) of this edge.
     *
     * In flow networks: residual capacity = weight - flow.
     */
    double getWeight() const;

    /** @brief Returns true if this edge is marked as selected. */
    bool isSelected() const;

    /** @brief Returns the origin vertex of this edge. */
    Vertex<T>* getOrig() const;

    /**
     * @brief Returns the reverse edge in the residual graph.
     *
     * Must be set via setReverse() when building the flow network.
     * Used by flow algorithms to push flow backwards along augmenting paths.
     * Returns nullptr if not set.
     */
    Edge<T>* getReverse() const;

    /**
     * @brief Returns the current flow through this edge.
     *
     * For forward edges:  residual capacity = getWeight() - getFlow().
     * For reverse edges:  residual capacity = getFlow() of the forward edge.
     */
    double getFlow() const;

    // ── Setters ─────────────────────────────────────────────

    /** @brief Sets the selected flag. */
    void setSelected(bool selected);

    /**
     * @brief Sets the reverse edge pointer for residual graph support.
     * @param reverse Pointer to the corresponding reverse edge.
     */
    void setReverse(Edge<T>* reverse);

    /**
     * @brief Sets the current flow through this edge.
     * @param flow New flow value. Should satisfy 0 ≤ flow ≤ weight.
     */
    void setFlow(double flow);

protected:
    Vertex<T>* dest;              ///< Destination vertex
    double     weight;            ///< Edge weight / capacity
    bool       selected = false;  ///< Auxiliary selection flag
    Vertex<T>* orig;              ///< Origin vertex
    Edge<T>*   reverse  = nullptr;///< Reverse edge for residual graph (nullptr if not set)
    double     flow;              ///< Current flow through this edge
};

/********************** Graph  ****************************/

/**
 * @class Graph
 * @brief Generic directed weighted graph with flow support.
 *
 * Stores a set of vertices and provides operations for adding and removing
 * vertices and edges, finding vertices by content, and accessing the full
 * vertex set for algorithm traversal.
 *
 * @tparam T Type of the vertex identifier (must support == comparison).
 *
 * Example usage:
 * @code
 * Graph<int> g;
 * g.addVertex(0);        // source
 * g.addVertex(1);        // sink
 * g.addEdge(0, 1, 5.0);  // directed edge with capacity 5
 * @endcode
 */
template <class T>
class Graph {
public:
    /**
     * @brief Destructor — frees all vertex memory and distance/path matrices.
     * @complexity Time: O(V + E). Space: O(1).
     */
    ~Graph();

    /**
     * @brief Finds and returns the vertex with the given info value.
     * @param in Info value to search for.
     * @return Pointer to the matching vertex, or nullptr if not found.
     * @complexity Time: O(V). Space: O(1).
     */
    Vertex<T>* findVertex(const T& in) const;

    /**
     * @brief Adds a new vertex with the given info value.
     * @param in Info value for the new vertex.
     * @return true if added successfully, false if a vertex with that info already exists.
     * @complexity Time: O(V). Space: O(1).
     */
    bool addVertex(const T& in);

    /**
     * @brief Removes the vertex with the given info value and all its incident edges.
     *
     * Removes all outgoing edges from the vertex and all incoming edges
     * from other vertices that point to it.
     *
     * @param in Info value of the vertex to remove.
     * @return true if removed successfully, false if the vertex was not found.
     * @complexity Time: O(V * E). Space: O(1).
     */
    bool removeVertex(const T& in);

    /**
     * @brief Adds a directed edge from sourc to dest with weight w.
     * @param sourc Info value of the source vertex.
     * @param dest  Info value of the destination vertex.
     * @param w     Edge weight / capacity.
     * @return true if added successfully, false if either vertex does not exist.
     * @complexity Time: O(V). Space: O(1).
     */
    bool addEdge(const T& sourc, const T& dest, double w);

    /**
     * @brief Removes the directed edge from sourc to dest.
     * @param sourc Info value of the source vertex.
     * @param dest  Info value of the destination vertex.
     * @return true if removed successfully, false if not found.
     * @complexity Time: O(V + E). Space: O(1).
     */
    bool removeEdge(const T& sourc, const T& dest);

    /**
     * @brief Adds a bidirectional edge between sourc and dest with weight w.
     *
     * Creates two directed edges (sourc→dest and dest→sourc) and links them
     * as reverse edges of each other, enabling residual graph traversal for
     * flow algorithms.
     *
     * @param sourc Info value of the first vertex.
     * @param dest  Info value of the second vertex.
     * @param w     Edge weight / capacity for both directions.
     * @return true if added successfully, false if either vertex does not exist.
     * @complexity Time: O(V). Space: O(1).
     */
    bool addBidirectionalEdge(const T& sourc, const T& dest, double w);

    /**
     * @brief Returns the number of vertices in the graph.
     * @return Total vertex count.
     * @complexity Time: O(1). Space: O(1).
     */
    int getNumVertex() const;

    /**
     * @brief Returns the full vertex set.
     * @return Vector of pointers to all vertices in the graph.
     * @complexity Time: O(1). Space: O(1).
     */
    std::vector<Vertex<T>*> getVertexSet() const;

protected:
    std::vector<Vertex<T>*> vertexSet;    ///< Set of all vertices in the graph
    double** distMatrix = nullptr;        ///< Distance matrix for Floyd-Warshall
    int**    pathMatrix = nullptr;        ///< Path matrix for Floyd-Warshall

    /**
     * @brief Returns the index of the vertex with the given info value.
     * @param in Info value to search for.
     * @return Index in vertexSet, or -1 if not found.
     * @complexity Time: O(V). Space: O(1).
     */
    int findVertexIdx(const T& in) const;
};

/**
 * @brief Frees a dynamically allocated n×n integer matrix.
 * @param m Pointer to the matrix to free.
 * @param n Dimension of the matrix.
 * @complexity Time: O(n). Space: O(1).
 */
void deleteMatrix(int** m, int n);

/**
 * @brief Frees a dynamically allocated n×n double matrix.
 * @param m Pointer to the matrix to free.
 * @param n Dimension of the matrix.
 * @complexity Time: O(n). Space: O(1).
 */
void deleteMatrix(double** m, int n);


/************************* Vertex  **************************/

template <class T>
Vertex<T>::Vertex(T in): info(in) {}

template <class T>
Edge<T> * Vertex<T>::addEdge(Vertex<T> *d, double w) {
    auto newEdge = new Edge<T>(this, d, w);
    adj.push_back(newEdge);
    d->incoming.push_back(newEdge);
    return newEdge;
}

template <class T>
bool Vertex<T>::removeEdge(T in) {
    bool removedEdge = false;
    auto it = adj.begin();
    while (it != adj.end()) {
        Edge<T> *edge = *it;
        Vertex<T> *dest = edge->getDest();
        if (dest->getInfo() == in) {
            it = adj.erase(it);
            deleteEdge(edge);
            removedEdge = true;
        }
        else {
            it++;
        }
    }
    return removedEdge;
}

template <class T>
void Vertex<T>::removeOutgoingEdges() {
    auto it = adj.begin();
    while (it != adj.end()) {
        Edge<T> *edge = *it;
        it = adj.erase(it);
        deleteEdge(edge);
    }
}

template <class T>
bool Vertex<T>::operator<(Vertex<T> & vertex) const {
    return this->dist < vertex.dist;
}

template <class T>
T Vertex<T>::getInfo() const {
    return this->info;
}

template <class T>
int Vertex<T>::getLow() const {
    return this->low;
}

template <class T>
void Vertex<T>::setLow(int value) {
    this->low = value;
}

template <class T>
int Vertex<T>::getNum() const {
    return this->num;
}

template <class T>
void Vertex<T>::setNum(int value) {
    this->num = value;
}

template <class T>
std::vector<Edge<T>*> Vertex<T>::getAdj() const {
    return this->adj;
}

template <class T>
bool Vertex<T>::isVisited() const {
    return this->visited;
}

template <class T>
bool Vertex<T>::isProcessing() const {
    return this->processing;
}

template <class T>
unsigned int Vertex<T>::getIndegree() const {
    return this->indegree;
}

template <class T>
double Vertex<T>::getDist() const {
    return this->dist;
}

template <class T>
Edge<T> *Vertex<T>::getPath() const {
    return this->path;
}

template <class T>
std::vector<Edge<T> *> Vertex<T>::getIncoming() const {
    return this->incoming;
}

template <class T>
void Vertex<T>::setInfo(T in) {
    this->info = in;
}

template <class T>
void Vertex<T>::setVisited(bool visited) {
    this->visited = visited;
}

template <class T>
void Vertex<T>::setProcessing(bool processing) {
    this->processing = processing;
}

template <class T>
void Vertex<T>::setIndegree(unsigned int indegree) {
    this->indegree = indegree;
}

template <class T>
void Vertex<T>::setDist(double dist) {
    this->dist = dist;
}

template <class T>
void Vertex<T>::setPath(Edge<T> *path) {
    this->path = path;
}

template <class T>
void Vertex<T>::deleteEdge(Edge<T> *edge) {
    Vertex<T> *dest = edge->getDest();
    auto it = dest->incoming.begin();
    while (it != dest->incoming.end()) {
        if ((*it)->getOrig()->getInfo() == info) {
            it = dest->incoming.erase(it);
        }
        else {
            it++;
        }
    }
    delete edge;
}

/********************** Edge  ****************************/

template <class T>
Edge<T>::Edge(Vertex<T> *orig, Vertex<T> *dest, double w): dest(dest), weight(w), orig(orig) {}

template <class T>
Vertex<T> * Edge<T>::getDest() const {
    return this->dest;
}

template <class T>
double Edge<T>::getWeight() const {
    return this->weight;
}

template <class T>
Vertex<T> * Edge<T>::getOrig() const {
    return this->orig;
}

template <class T>
Edge<T> *Edge<T>::getReverse() const {
    return this->reverse;
}

template <class T>
bool Edge<T>::isSelected() const {
    return this->selected;
}

template <class T>
double Edge<T>::getFlow() const {
    return flow;
}

template <class T>
void Edge<T>::setSelected(bool selected) {
    this->selected = selected;
}

template <class T>
void Edge<T>::setReverse(Edge<T> *reverse) {
    this->reverse = reverse;
}

template <class T>
void Edge<T>::setFlow(double flow) {
    this->flow = flow;
}

/********************** Graph  ****************************/

template <class T>
int Graph<T>::getNumVertex() const {
    return vertexSet.size();
}

template <class T>
std::vector<Vertex<T> *> Graph<T>::getVertexSet() const {
    return vertexSet;
}

template <class T>
Vertex<T> * Graph<T>::findVertex(const T &in) const {
    for (auto v : vertexSet)
        if (v->getInfo() == in)
            return v;
    return nullptr;
}

template <class T>
int Graph<T>::findVertexIdx(const T &in) const {
    for (unsigned i = 0; i < vertexSet.size(); i++)
        if (vertexSet[i]->getInfo() == in)
            return i;
    return -1;
}

template <class T>
bool Graph<T>::addVertex(const T &in) {
    if (findVertex(in) != nullptr)
        return false;
    vertexSet.push_back(new Vertex<T>(in));
    return true;
}

template <class T>
bool Graph<T>::removeVertex(const T &in) {
    for (auto it = vertexSet.begin(); it != vertexSet.end(); it++) {
        if ((*it)->getInfo() == in) {
            auto v = *it;
            v->removeOutgoingEdges();
            for (auto u : vertexSet) {
                u->removeEdge(v->getInfo());
            }
            vertexSet.erase(it);
            delete v;
            return true;
        }
    }
    return false;
}

template <class T>
bool Graph<T>::addEdge(const T &sourc, const T &dest, double w) {
    auto v1 = findVertex(sourc);
    auto v2 = findVertex(dest);
    if (v1 == nullptr || v2 == nullptr)
        return false;
    v1->addEdge(v2, w);
    return true;
}

template <class T>
bool Graph<T>::removeEdge(const T &sourc, const T &dest) {
    Vertex<T> * srcVertex = findVertex(sourc);
    if (srcVertex == nullptr) {
        return false;
    }
    return srcVertex->removeEdge(dest);
}

template <class T>
bool Graph<T>::addBidirectionalEdge(const T &sourc, const T &dest, double w) {
    auto v1 = findVertex(sourc);
    auto v2 = findVertex(dest);
    if (v1 == nullptr || v2 == nullptr)
        return false;
    auto e1 = v1->addEdge(v2, w);
    auto e2 = v2->addEdge(v1, w);
    e1->setReverse(e2);
    e2->setReverse(e1);
    return true;
}

inline void deleteMatrix(int **m, int n) {
    if (m != nullptr) {
        for (int i = 0; i < n; i++)
            if (m[i] != nullptr)
                delete [] m[i];
        delete [] m;
    }
}

inline void deleteMatrix(double **m, int n) {
    if (m != nullptr) {
        for (int i = 0; i < n; i++)
            if (m[i] != nullptr)
                delete [] m[i];
        delete [] m;
    }
}

template <class T>
Graph<T>::~Graph() {
    deleteMatrix(distMatrix, vertexSet.size());
    deleteMatrix(pathMatrix, vertexSet.size());
}

#endif /* DA_TP_CLASSES_GRAPH */