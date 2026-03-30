#pragma once
#include "ConferenceGraph.h"
#include "EdmondsKarp.h"
#include "FordFulkerson.h"

/**
 * @file AssignmentSolver.h
 * @brief Solves the reviewer assignment problem using Max-Flow algorithms.
 */

/**
 * @enum FlowAlgorithm
 * @brief Selects which Max-Flow algorithm to use when solving the assignment.
 */
enum FlowAlgorithm {
    FORD_FULKERSON, ///< Ford-Fulkerson using DFS. Complexity: O(E * F)
    EDMONDS_KARP    ///< Edmonds-Karp using BFS. Complexity: O(V * E^2)
};

/**
 * @class AssignmentSolver
 * @brief Solves the reviewer-submission assignment problem as a Max-Flow problem.
 *
 * Builds a flow network via ConferenceGraph and runs either Edmonds-Karp or
 * Ford-Fulkerson to find a maximum flow. The resulting flow is then interpreted
 * as a set of reviewer-submission assignments.
 *
 * A fully successful assignment saturates all Source->Submission edges,
 * meaning every submission receives its minimum required number of reviews.
 * If the max flow is less than N * MinReviewsPerSubmission, some submissions
 * are under-reviewed and the missing reviews are reported.
 *
 * Example usage:
 * @code
 * AssignmentSolver solver(data);
 * solver.solve(EDMONDS_KARP);
 * solver.exportAssignments("output.csv");
 * if (data.control.riskAnalysis > 0)
 *     solver.riskAnalysis("risk.csv");
 * @endcode
 */
class AssignmentSolver {
public:
    /**
     * @brief Constructs the solver and builds the initial flow network.
     *
     * Initialises the flow graph by calling ConferenceGraph::buildGraph()
     * using the assignment mode specified in data.control.generateAssignments.
     *
     * @param data Parsed conference data containing submissions, reviewers and parameters.
    * @complexity Time: O(N * M), where N = submissions and M = reviewers.
    * @complexity Space: O(V + E) for the flow network.
     */
    AssignmentSolver(const ConferenceData& data);

    /**
    * @brief Runs the selected Max-Flow algorithm on the current flow network.
    *
    * The flow network is built in the constructor from ConferenceData. This
    * method executes either Ford-Fulkerson or Edmonds-Karp once and, if
    * requested by control flags, exports assignments and a Graphviz view.
     *
     * @param algo Algorithm to use (default: FORD_FULKERSON).
     *
    * @complexity Time (Edmonds-Karp): O(V * E^2), where V = N + M + 2 and E = O(N * M).
    * @complexity Time (Ford-Fulkerson): O(E * F), where F is the max-flow value.
     * @complexity Space: O(V + E) for the flow graph.
     */
    void solve(FlowAlgorithm algo = FlowAlgorithm::FORD_FULKERSON);

    /**
     * @brief Exports the assignment results to a CSV output file.
     *
     * Output format (as per project specification):
     * @code
     * #SubmissionId,ReviewerId,Match
     * 31, 2, 4
     * #ReviewerId,SubmissionId,Match
     * 2, 31, 4
     * #Total: N
     * @endcode
     *
     * If any submissions are under-reviewed, also appends:
     * @code
     * #SubmissionId,Domain,MissingReviews
     * 31, 3, 2
     * @endcode
     *
     * All listings are sorted as required by the specification.
     *
     * @param filename Path to the output CSV file.
    * @complexity Time: O(E + A log A), where A = number of assignments.
     * @complexity Space: O(A) for storing match records.
     */
    void exportAssignments(const std::string& filename) const;

    /**
     * @brief Performs risk analysis by simulating reviewer removal.
     *
    * For RiskAnalysis = 1: removes each reviewer one at a time, rebuilds
     * the graph, reruns the flow algorithm and checks if the assignment is
    * still feasible. Reports which reviewers are "risky" - those whose
     * removal makes the full assignment impossible.
     *
     * Results are written to both the main output file and the risk file
     * (if provided), using the same formatted output.
     *
     * Output format:
     * @code
     * #Risk Analysis: 1
     * 1, 2
     * @endcode
     *
     * @param riskFile Path to the risk analysis output file (optional).
     *                 If empty, results are only appended to the main output file.
     *
    * @complexity Time: O(M * V * E^2) in the current implementation,
    *                   where M = number of reviewers.
     *
     */
    void riskAnalysis(const std::string& riskFile = "") const;

    /**
    * @brief Generates a Graphviz DOT file visualizing the flow network.
     *
     * Nodes are colour coded:
     * - Green:  Source
     * - Red:    Sink
     * - Yellow: Submission nodes
     * - Blue:   Reviewer nodes
     *
     * Edges with flow > 0 are drawn in dark green with flow/capacity labels.
     * Edges with flow = 0 are drawn as dashed grey lines.
     *
     * @param filename Path to the output .dot file.
    * @complexity Time: O(V + E), where V = vertices and E = edges.
     */
    void generateGraphviz(const std::string& filename) const;

private:
    const ConferenceData& data;  ///< Reference to the parsed conference data
    Graph<int> flowGraph;        ///< The flow network graph used for assignment

    /**
     * @brief Converts a graph submission node ID to a data.submissions vector index.
     *
     * Submission nodes are numbered 1 to N in the graph.
     * Index = nodeId - 1.
     *
     * @param nodeId Graph node ID of the submission.
     * @return Zero-based index into data.submissions.
     * @complexity Time: O(1). Space: O(1).
     */
    int getSubmissionRealId(int nodeId) const;

    /**
     * @brief Converts a graph reviewer node ID to a data.reviewers vector index.
     *
     * Reviewer nodes are numbered N+1 to N+M in the graph.
     * Index = nodeId - 1 - N where N = number of submissions.
     *
     * @param nodeId Graph node ID of the reviewer.
     * @return Zero-based index into data.reviewers.
     * @complexity Time: O(1). Space: O(1).
     */
    int getReviewerRealId(int nodeId) const;
};