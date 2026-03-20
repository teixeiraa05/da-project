#ifndef ASSIGNMENT_SOLVER_H
#define ASSIGNMENT_SOLVER_H

#include "ConferenceGraph.h"


class AssignmentSolver {
public:
    AssignmentSolver(const ConferenceData& data);
    
    void solve();

    //Exports the assignments to a CSV file. Format: submissionId, reviewerId, matchDomain and reviewerId, submissionId, matchDomain
    void exportAssignments(const std::string& filename) const;

    void riskAnalysis() const;

private:
    const ConferenceData& data;
    Graph<int> flowGraph;

    int getSubmissionRealId(int submissionId) const;
    int getReviewerRealId(int reviewerId) const;
};

#endif // ASSIGNMENT_SOLVER_H