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




    outFile.close();
}