#pragma once
#include "ConferenceData.h"
#include "Graph.h"

/**
 * @file ConferenceGraph.h
 * @brief Builds the Max-Flow network graph from conference assignment data.
 */

/**
 * @class ConferenceGraph
 * @brief Constructs a directed flow network modelling the reviewer assignment problem.
 *
 * The flow network has the following structure:
 * @code
 * Source → Submissions → Reviewers → Sink
 * @endcode
 *
 * Node ID encoding:
 * - 0                          → Source
 * - 1 to N                     → Submission nodes (N = total submissions)
 * - N+1 to N+M                 → Reviewer nodes   (M = total reviewers)
 * - N+M+1                      → Sink
 *
 * Edge capacities:
 * - Source → Submission:   capacity = MinReviewsPerSubmission
 * - Submission → Reviewer: capacity = 1 (each reviewer reviews each submission at most once)
 * - Reviewer → Sink:       capacity = MaxReviewsPerReviewer
 *
 * A Submission→Reviewer edge is only created if their domains match
 * according to the assignment mode (see domainsMatch()).
 */
class ConferenceGraph {
public:
    /**
     * @brief Constructs a ConferenceGraph from the given conference data.
     * @param data Parsed conference data including submissions, reviewers and parameters.
     */
    ConferenceGraph(const ConferenceData& data);

    /**
     * @brief Builds and returns the complete flow network graph.
     *
     * Creates all vertices and edges according to the node encoding described
     * in the class documentation. Domain matching is determined by
     * data.control.generateAssignments (the assignment mode).
     *
     * @return A directed Graph<int> representing the flow network.
     *
    * @complexity Time: O(N*M), where N = submissions and M = reviewers.
    *                   This comes from checking all submission-reviewer pairs.
     */
    Graph<int> buildGraph() const;

private:
    ConferenceData data; ///< Local copy of the parsed conference data

    /**
     * @brief Returns the graph node ID for the source vertex.
     * @return Always 0.
     */
    int getSourceID() const;

    /**
     * @brief Returns the graph node ID for the sink vertex.
     * @return N + M + 1 where N = submissions, M = reviewers.
     */
    int getSinkID() const;

    /**
     * @brief Returns the graph node ID for a submission at the given index.
     * @param index Zero-based index into data.submissions.
     * @return index + 1
     */
    int getSubmissionID(size_t index) const;

    /**
     * @brief Returns the graph node ID for a reviewer at the given index.
     * @param index Zero-based index into data.reviewers.
     * @return index + 1 + N where N = number of submissions.
     */
    int getReviewerID(size_t index) const;
};