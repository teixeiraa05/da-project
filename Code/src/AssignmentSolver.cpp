#include "../include/AssignmentSolver.h"
#include "../include/MaxFlow.h"
#include <fstream>
#include <iostream>

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
        std::cerr << "Error opening file for writing: " << filename << std::endl;
        return;
    }

    outFile << "#SubmissionId,ReviewerId,Match\n";
    int totalMatches = 0;
    
    int sourceId = 0;
    int sinkId = (int)(data.reviewers.size() + data.submissions.size() + 1);

    for(auto v : flowGraph.getVertexSet()) {
        if (v->getInfo() == sourceId || v->getInfo() == sinkId) continue;
    
        for (auto e : v->getAdj()) {
            if (e->getFlow() > 0) { // If there is flow, it means there is an assignment
                int submissionId = getSubmissionRealId(v->getInfo());
                int reviewerId = getReviewerRealId(e->getDest()->getInfo());
                int matchDomain = domainsMatch(data.submissions[submissionId], data.reviewers[reviewerId], data.control.generateAssignments);
                outFile << data.submissions[submissionId].submissionId << "," << data.reviewers[reviewerId].reviewerId << "," << matchDomain << "\n";
                totalMatches++;
            }
        }
    }

    outFile << "#Total: " << totalMatches << "\n";
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