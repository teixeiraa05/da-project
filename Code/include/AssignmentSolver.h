#include "ConferenceGraph.h"


class AssignmentSolver {
public:
    AssignmentSolver(const ConferenceGraph& graph);
    
    void solve();

    //Exports the assignments to a CSV file. Format: submissionId, reviewerId, matchDomain and reviewerId, submissionId, matchDomain
    void exportAssignments(const std::string& filename) const;

private:
    const ConferenceData& data;
    Graph<int> flowGraph;
};