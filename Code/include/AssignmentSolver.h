#pragma once
#include "ConferenceGraph.h"
#include <vector>

enum FlowAlgorithm {
    FORD_FULKERSON,
    EDMONDS_KARP
};

class AssignmentSolver {
public:
    AssignmentSolver(const ConferenceData& data);
    
    void solve(FlowAlgorithm algo = FlowAlgorithm::FORD_FULKERSON);

    //Exports the assignments to a CSV file. Format: submissionId, reviewerId, matchDomain and reviewerId, submissionId, matchDomain
    void exportAssignments(const std::string& filename) const;

    std::vector<int> riskAnalysis() const;
    
    void generateGraphviz(const std::string& filename) const;

    void printAssignments() const;

    // Returns {flow, capacity} for a given submission ID (data ID, not node ID)
    std::pair<double, double> getSubmissionFlow(int submissionId) const;
    // Returns {flow, capacity} for a given reviewer ID (data ID, not node ID)
    std::pair<double, double> getReviewerFlow(int reviewerId) const;
private:
    const ConferenceData& data;
    Graph<int> flowGraph;

    int getSubmissionRealId(int submissionId) const;
    int getReviewerRealId(int reviewerId) const;
};