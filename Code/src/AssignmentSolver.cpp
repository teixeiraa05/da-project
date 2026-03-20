#include "../include/AssignmentSolver.h"
#include "../include/MaxFlow.h"
#include <fstream>
#include <iostream>
#include <algorithm>
#include <vector>

// -------------------------- Data Structures ---------------------------------
// Internal structs to manage the multi-block output formatting
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
    // Source is always 0, Sink is the last node
    int sourceId = 0;
    int sinkId = static_cast<int>(data.reviewers.size() + data.submissions.size() + 1);

    // Run the Max Flow algorithm
    edmondsKarp(&flowGraph, sourceId, sinkId);

    // Task 2.1: Export the results if requested
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

    // Loop through submissions to collect flow data
    for (size_t i = 0; i < data.submissions.size(); i++) {
        // Graph Node ID for submissions start at 1
        int subNodeId = static_cast<int>(i + 1);
        Vertex<int>* subVertex = flowGraph.findVertex(subNodeId);
        if (subVertex == nullptr) continue;

        const Submission& actualSub = data.submissions[i];
        int actualReviews = 0;

        for (Edge<int>* edge : subVertex->getAdj()) {
            // If flow is 1, an assignment was made to a reviewer
            if (edge->getFlow() == 1.0) {
                actualReviews++;
                
                int revNodeId = edge->getDest()->getInfo();
                int revIndex = getReviewerRealId(revNodeId); 
                const Reviewer& actualRev = data.reviewers[revIndex];

                int matchDomainId = domainsMatch(actualSub, actualRev, data.control.generateAssignments);
                matches.push_back({actualSub.submissionId, actualRev.reviewerId, matchDomainId});
            }
        }

        // Check for missing reviews
        int missing = requiredReviews - actualReviews;
        if (missing > 0) {
            missingReviews.push_back({actualSub.submissionId, actualSub.primary, missing});
        }
    }

    // --- BLOCK 1: Submission Order ---
    // matches is already naturally sorted by submission index from the loop above
    outFile << "#SubmissionId,ReviewerId,Match\n";
    for (const auto& m : matches) {
        outFile << m.subId << ", " << m.revId << ", " << m.matchCode << "\n";
    }

    // --- BLOCK 2: Reviewer Order ---
    std::sort(matches.begin(), matches.end(), [](const MatchRecord& a, const MatchRecord& b) {
        if (a.revId != b.revId) return a.revId < b.revId;
        return a.subId < b.subId; 
    });

    outFile << "#ReviewerId,SubmissionId,Match\n";
    for (const auto& m : matches) {
        outFile << m.revId << ", " << m.subId << ", " << m.matchCode << "\n";
    }

    // --- BLOCK 3: Totals ---
    outFile << "#Total: " << matches.size() << "\n";

    // --- BLOCK 4: Missing Reviews ---
    if (!missingReviews.empty()) {
        outFile << "#SubmissionId,Domain,MissingReviews\n";
        for (const auto& record : missingReviews) {
            outFile << record.subId << ", " << record.domain << ", " << record.missingCount << "\n";
        }
    }

    outFile.close();
}

void AssignmentSolver::riskAnalysis() const {
    if (data.control.riskAnalysis == 0) return;

    int sourceId = 0;
    int sinkId = static_cast<int>(data.reviewers.size() + data.submissions.size() + 1);
    int totalRequired = static_cast<int>(data.submissions.size() * data.params.minReviewsPerSubmission);

    // Initial check to see if we currently meet the requirement
    int initialFlow = 0;
    Vertex<int>* s_orig = flowGraph.findVertex(sourceId);
    if (s_orig) {
        for (auto e : s_orig->getAdj()) initialFlow += static_cast<int>(e->getFlow());
    }

    std::vector<int> riskyReviewers;
    
    // Only proceed if the problem was solvable with everyone present
    if (initialFlow >= totalRequired) {
        for (size_t i = 0; i < data.reviewers.size(); i++) {
            ConferenceGraph tempBuilder(data);
            Graph<int> tempGraph = tempBuilder.buildGraph();
            
            // Calculate the Graph Node ID for the current reviewer to remove them
            int nodeToDelete = static_cast<int>(i + 1 + data.submissions.size());
            tempGraph.removeVertex(nodeToDelete);
            
            edmondsKarp(&tempGraph, sourceId, sinkId);

            int totalFlow = 0;
            Vertex<int>* s = tempGraph.findVertex(sourceId);
            if (s) {
                for (auto e : s->getAdj()) totalFlow += static_cast<int>(e->getFlow());
            }

            // If removal causes a drop below required reviews, they are "Risky"
            if (totalFlow < totalRequired) {
                riskyReviewers.push_back(data.reviewers[i].reviewerId);
            }
        }
    }

    std::sort(riskyReviewers.begin(), riskyReviewers.end());

    // Append to the output file
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

// -------------------------- HELPER METHODS ---------------------------------

int AssignmentSolver::getSubmissionRealId(int nodeId) const {
    // Graph ID 1 maps to Vector Index 0
    return nodeId - 1;
}

int AssignmentSolver::getReviewerRealId(int nodeId) const {
    // Reviewer IDs in graph start after Submissions (1 to N)
    return nodeId - 1 - static_cast<int>(data.submissions.size());
}