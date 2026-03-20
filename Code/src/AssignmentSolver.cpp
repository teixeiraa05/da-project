#include "../include/AssignmentSolver.h"
#include "../include/MaxFlow.h"
#include <fstream>
#include <iostream>
#include <algorithm>

AssignmentSolver::AssignmentSolver(const ConferenceData& data) : data(data) {
    ConferenceGraph buildGraph(data);
    this->flowGraph = buildGraph.buildGraph();
}

void AssignmentSolver::solve() {
    int sourceId = GetSourceID();
    int sinkId = GetSinkID(data.submissions.size(), data.reviewers.size());

    edmondsKarp(&flowGraph, sourceId, sinkId);

    if (data.control.generateAssignments > 0) {
        exportAssignments(data.control.outputFileName);
    }
}

void AssignmentSolver::exportAssignments(const std::string& filename) const {
    std::ofstream outFile(filename);
    if (!outFile.is_open()) {
        std::cerr << "Error opening file for writing: " << filename << std::endl;
        return;
    }

    struct Assignment {
        int subId;
        int revId;
        int match;
        bool operator<(const Assignment& other) const {
            if (subId != other.subId) return subId < other.subId;
            return revId < other.revId;
        }
    };
    struct RevAssignment {
        int revId;
        int subId;
        int match;
        bool operator<(const RevAssignment& other) const {
            if (revId != other.revId) return revId < other.revId;
            return subId < other.subId;
        }
    };

    std::vector<Assignment> subToRev;
    std::vector<RevAssignment> revToSub;
    std::vector<int> subFlow(data.submissions.size(), 0);

    for (size_t i = 0; i < data.submissions.size(); i++) {
        int vId = GetSubmissionID(i);
        Vertex<int>* v = flowGraph.findVertex(vId);
        if (!v) continue;
        for (auto e : v->getAdj()) {
            if (e->getDest()->getInfo() != GetSourceID() && e->getDest()->getInfo() != GetSinkID(data.submissions.size(), data.reviewers.size()) && e->getFlow() > 0) {
                int revIdx = GetReviewerIdx(e->getDest()->getInfo(), data.submissions.size());
                int match = domainsMatch(data.submissions[i], data.reviewers[revIdx], data.control.generateAssignments);
                subToRev.push_back({data.submissions[i].submissionId, data.reviewers[revIdx].reviewerId, match});
                revToSub.push_back({data.reviewers[revIdx].reviewerId, data.submissions[i].submissionId, match});
                subFlow[i] += (int)e->getFlow();
            }
        }
    }

    std::sort(subToRev.begin(), subToRev.end());
    std::sort(revToSub.begin(), revToSub.end());

    outFile << "#SubmissionId,ReviewerId,Match\n";
    for (const auto& a : subToRev) outFile << a.subId << ", " << a.revId << ", " << a.match << "\n";
    
    outFile << "#ReviewerId,SubmissionId,Match\n";
    for (const auto& a : revToSub) outFile << a.revId << ", " << a.subId << ", " << a.match << "\n";

    outFile << "#Total: " << subToRev.size() << "\n";

    // Unsuccessful reporting
    bool hasUnsuccessful = false;
    for (size_t i = 0; i < data.submissions.size(); i++) {
        if (subFlow[i] < data.params.minReviewsPerSubmission) {
            if (!hasUnsuccessful) {
                outFile << "#SubmissionId,Domain,MissingReviews\n";
                hasUnsuccessful = true;
            }
            outFile << data.submissions[i].submissionId << ", " << data.submissions[i].primary << ", " << (data.params.minReviewsPerSubmission - subFlow[i]) << "\n";
        }
    }

    outFile.close();
}

void AssignmentSolver::riskAnalysis() const {
    if (data.control.riskAnalysis == 0) return;

    int sourceId = GetSourceID();
    int sinkId = GetSinkID(data.submissions.size(), data.reviewers.size());
    int totalRequired = (int)(data.submissions.size() * data.params.minReviewsPerSubmission);

    // Initial check
    int initialFlow = 0;
    Vertex<int>* s_orig = flowGraph.findVertex(sourceId);
    if (s_orig) {
        for (auto e : s_orig->getAdj()) initialFlow += (int)e->getFlow();
    }

    std::vector<int> riskyReviewers;
    if (initialFlow >= totalRequired) {
        for (size_t i = 0; i < data.reviewers.size(); i++) {
            ConferenceGraph tempBuilder(data);
            Graph<int> tempGraph = tempBuilder.buildGraph();
            tempGraph.removeVertex(GetReviewerID(i, data.submissions.size()));
            
            edmondsKarp(&tempGraph, sourceId, sinkId);

            int totalFlow = 0;
            Vertex<int>* s = tempGraph.findVertex(sourceId);
            if (s) {
                for (auto e : s->getAdj()) totalFlow += (int)e->getFlow();
            }

            if (totalFlow < totalRequired) {
                riskyReviewers.push_back(data.reviewers[i].reviewerId);
            }
        }
    }

    std::sort(riskyReviewers.begin(), riskyReviewers.end());

    std::ofstream outFile(data.control.outputFileName, std::ios::app);
    if (outFile.is_open()) {
        outFile << "#Risk Analysis: " << data.control.riskAnalysis << "\n";
        if (riskyReviewers.empty()) {
            outFile << "\n";
        } else {
            for (size_t i = 0; i < riskyReviewers.size(); i++) {
                outFile << riskyReviewers[i] << (i == riskyReviewers.size() - 1 ? "" : ", ");
            }
            outFile << "\n";
        }
        outFile.close();
    }
}

int AssignmentSolver::getSubmissionRealId(int submissionId) const { return GetSubmissionIdx(submissionId); }
int AssignmentSolver::getReviewerRealId(int reviewerId) const { return GetReviewerIdx(reviewerId, data.submissions.size()); }