#include "ConferenceGraph.h"
#include "../include/FlowAlgorithm.h"

class AssignmentSolver {
public:
    AssignmentSolver(const ConferenceData& data);
    
    void solve(FlowAlgorithm algo = FlowAlgorithm::FORD_FULKERSON);

    //Exports the assignments to a CSV file. Format: submissionId, reviewerId, matchDomain and reviewerId, submissionId, matchDomain
    void exportAssignments(const std::string& filename) const;

    void riskAnalysis() const;
    
    void generateGraphviz(const std::string& filename) const;
private:
    const ConferenceData& data;
    Graph<int> flowGraph;

    int getSubmissionRealId(int submissionId) const;
    int getReviewerRealId(int reviewerId) const;
};