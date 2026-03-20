#include "../include/AssignmentSolver.h"
#include "../include/MaxFlow.h"
#include <fstream>
#include <iostream>
// -------------------------- Data Structures ---------------------------------
struct MatchRecord {
    int subId;
    int revId;
    int matchCode;
};

struct MissingRecord {
    int subId;
    int domain;
    int missingCount;
};

// -------------------------- ASSIGNMENT SOLVER ---------------------------------

AssignmentSolver::AssignmentSolver(const ConferenceData& data) : data(data) {
    ConferenceGraph buildGraph(data);
    this->flowGraph = buildGraph.buildGraph();
}

void AssignmentSolver::solve() {
    int sourceId = 0;
    int sinkId = (int)(data.reviewers.size() + data.submissions.size() + 1);

    edmondsKarp(&flowGraph, sourceId, sinkId);

    if (data.control.generateAssignments > 0) {
        exportAssignments(data.control.outputFileName);
    }
}

void AssignmentSolver::exportAssignments(const std::string& filename) const {
    std::ofstream outFile(filename);
    if (!outFile.is_open()) {
        std::cerr << "Error: Could not open file " << filename << "\n";
        return;
    }

    std::vector<MatchRecord> matches;
    std::vector<MissingRecord> missingReviews;

    int requiredReviews = data.params.minReviewsPerSubmission;

    //Loop through the graph ONCE to collect all data
    for (size_t i = 0; i < data.submissions.size(); i++) {
        int subNodeId = static_cast<int>(i + 1);
        Vertex<int>* subVertex = flowGraph.findVertex(subNodeId);
        if (subVertex == nullptr) continue;

        const Submission& actualSub = data.submissions[i];
        int actualReviews = 0;

        for (Edge<int>* edge : subVertex->getAdj()) {
            if (edge->getFlow() == 1.0) {
                actualReviews++;
                
                int revNodeId = edge->getDest()->getInfo();
                int revIndex = revNodeId - static_cast<int>(data.submissions.size()) - 1; 
                const Reviewer& actualRev = data.reviewers[revIndex];

                int matchDomainId = domainsMatch(actualSub, actualRev, data.control.generateAssignments);
                matches.push_back({actualSub.submissionId, actualRev.reviewerId, matchDomainId});
            }
        }

        // Check for missing reviews and record them
        int missing = requiredReviews - actualReviews;
        if (missing > 0) {
            missingReviews.push_back({actualSub.submissionId, actualSub.primary, missing});
        }
    }

    //Print Block 1: Submissions -> Reviewers
    // (It's already naturally sorted by Submission index from our loop)
    outFile << "#SubmissionId,ReviewerId,Match\n";
    for (const auto& m : matches) {
        outFile << m.subId << ", " << m.revId << ", " << m.matchCode << "\n";
    }

    //Print Block 2: Reviewers -> Submissions
    std::sort(matches.begin(), matches.end(), [](const MatchRecord& a, const MatchRecord& b) {
        if (a.revId != b.revId) return a.revId < b.revId;
        return a.subId < b.subId; // If same reviewer, sort by submission ID
    });

    outFile << "#ReviewerId,SubmissionId,Match\n";
    for (const auto& m : matches) {
        outFile << m.revId << ", " << m.subId << ", " << m.matchCode << "\n";
    }

    //Total Matches
    outFile << "#Total: " << matches.size() << "\n";

    //Missing Reviews (Only if there are any!)
    if (!missingReviews.empty()) {
        outFile << "#SubmissionId,Domain,MissingReviews\n";
        for (const auto& record : missingReviews) {
            outFile << record.subId << ", " << record.domain << ", " << record.missingCount << "\n";
        }
    }

    outFile.close();
}

void AssignmentSolver::riskAnalysis() const {
    //TODO
}


int AssignmentSolver::getSubmissionRealId(int submissionId) const {
    return submissionId - 1;
}

int AssignmentSolver::getReviewerRealId(int reviewerId) const {
    return reviewerId - 1 - data.submissions.size();
}