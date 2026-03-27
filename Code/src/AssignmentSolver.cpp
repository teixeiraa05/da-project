#include "../include/AssignmentSolver.h"
#include "../include/EdmondsKarp.h"
#include "../include/FordFulkerson.h"
#include <fstream>
#include <iostream>
#include <algorithm>
#include <vector>
#include <map>

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

void AssignmentSolver::solve(FlowAlgorithm algo) {
    // Source is always 0, Sink is the last node
    int sourceId = 0;
    int sinkId = static_cast<int>(data.reviewers.size() + data.submissions.size() + 1);

    // Run the Max Flow algorithm
    if (algo == FlowAlgorithm::FORD_FULKERSON) {
        fordFulkerson(&flowGraph, sourceId, sinkId);
    } else if (algo == FlowAlgorithm::EDMONDS_KARP) {
        edmondsKarp(&flowGraph, sourceId, sinkId);
    } else {
        throw std::invalid_argument("Unsupported flow algorithm specified.");
    }

    // Task 2.1: Show the results in the console
    printAssignments();

    // Task 2.1: Export the results if requested
    if (data.control.generateAssignments > 0) {
        exportAssignments(data.control.outputFileName);

        std::string dotFilename = data.control.outputFileName;
        size_t dotPos = dotFilename.find_last_of('.');
        
        if (dotPos != std::string::npos) {
            // Replace ".csv" (or whatever extension) with ".dot"
            dotFilename = dotFilename.substr(0, dotPos) + ".dot";
        } else {
            // If there's no extension, just append ".dot"
            dotFilename += ".dot";
        }
        generateGraphviz(dotFilename);
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

std::vector<int> AssignmentSolver::riskAnalysis() const {
    std::vector<int> riskyReviewers;
    if (data.control.riskAnalysis == 0) return riskyReviewers;

    int sourceId = 0;
    int sinkId = static_cast<int>(data.reviewers.size() + data.submissions.size() + 1);
    int totalRequired = static_cast<int>(data.submissions.size() * data.params.minReviewsPerSubmission);

    // Initial check to see if we currently meet the requirement
    int initialFlow = 0;
    Vertex<int>* s_orig = flowGraph.findVertex(sourceId);
    if (s_orig) {
        for (auto e : s_orig->getAdj()) initialFlow += static_cast<int>(e->getFlow());
    }


    
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
    
    return riskyReviewers;
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

// --------------------------- GRAPHVIZ GENERATION ---------------------------------
void AssignmentSolver::generateGraphviz(const std::string& filename) const {
    std::ofstream out(filename);
    if (!out.is_open()) {
        std::cerr << "Error opening file for Graphviz output!" << std::endl;
        return;
    }

    out << "digraph ConferenceFlow {\n";
    out << "    rankdir=LR;\n"; // Left-to-Right layout
    
    // Default styling for nodes
    out << "    node [fontname=\"Helvetica\", style=filled];\n\n";

    int sourceId = 0;
    int sinkId = static_cast<int>(data.submissions.size() + data.reviewers.size() + 1);

    // ---------------------------------------------------------
    // STEP 1: Define all the Nodes and their Custom Labels
    // ---------------------------------------------------------
    for (auto v : flowGraph.getVertexSet()) {
        int id = v->getInfo();
        std::string label;
        std::string shape = "circle";
        std::string fillcolor = "lightcyan";

        if (id == sourceId) {
            label = "SOURCE";
            shape = "circle";
            fillcolor = "lightgreen";
        } 
        else if (id == sinkId) {
            label = "SINK";
            shape = "circle";
            fillcolor = "lightcoral";
        } 
        else if (id <= static_cast<int>(data.submissions.size())) {
            // It's a Submission! Look up its real ID.
            int subIdx = getSubmissionRealId(id);
            label = "Sub " + std::to_string(data.submissions[subIdx].submissionId);
            fillcolor = "lightyellow";
        } 
        else {
            // It's a Reviewer! Look up their real ID.
            int revIdx = getReviewerRealId(id);
            // Using \\n creates a line break inside the Graphviz node
            label = "Rev " + std::to_string(data.reviewers[revIdx].reviewerId);
            fillcolor = "lightblue";
        }

        // Print the node definition
        out << "    \"" << id << "\" [label=\"" << label << "\", shape=\"" << shape 
            << "\", fillcolor=\"" << fillcolor << "\"];\n";
    }

    out << "\n    // ---------------------------------------------------------\n";
    out << "    // STEP 2: Draw the Edges\n";
    out << "    // ---------------------------------------------------------\n";

    for (auto v : flowGraph.getVertexSet()) {
        for (auto edge : v->getAdj()) {
            
            // Assuming your capacity is stored in getWeight() as per your previous snippet
            if (edge->getWeight() > 0) { 
                out << "    \"" << v->getInfo() << "\" -> \"" << edge->getDest()->getInfo() << "\" ";
                
                // Color code: Green if flow > 0, Gray/Dashed if flow is 0
                if (edge->getFlow() > 0) {
                    out << "[label=\" " << edge->getFlow() << "/" << edge->getWeight() 
                        << "\", color=darkgreen, penwidth=2];\n";
                } else {
                    out << "[label=\" " << edge->getFlow() << "/" << edge->getWeight() 
                        << "\", style=dashed, color=gray];\n";
                }
            }
        }
    }

    out << "}\n";
    out.close();
}

void AssignmentSolver::printAssignments() const {
    std::cout << "\n--- Assignment Results (Result Path) ---\n";
    bool found = false;

    int sourceId = 0;
    int sinkId = static_cast<int>(data.submissions.size() + data.reviewers.size() + 1);
    
    // Use a vector for O(1) access and fewer allocations - Senior Mentor optimization
    std::vector<std::pair<double, double>> nodeAgg(sinkId + 1, {0.0, 0.0});

    Vertex<int>* srcVertex = flowGraph.findVertex(sourceId);
    if (srcVertex) {
        for (auto e : srcVertex->getAdj()) {
            int destId = e->getDest()->getInfo();
            if (destId >= 0 && destId <= sinkId) {
                nodeAgg[destId] = {e->getFlow(), e->getWeight()};
            }
        }
    }

    // Reviewers are nodes from N+1 to M. They connect to Sink (M+1).
    for (size_t i = 0; i < data.reviewers.size(); i++) {
        int revNodeId = static_cast<int>(i + 1 + data.submissions.size());
        Vertex<int>* revVertex = flowGraph.findVertex(revNodeId);
        if (revVertex) {
            for (auto e : revVertex->getAdj()) {
                if (e->getDest()->getInfo() == sinkId) {
                    nodeAgg[revNodeId] = {e->getFlow(), e->getWeight()};
                    break;
                }
            }
        }
    }

    for (size_t i = 0; i < data.submissions.size(); i++) {
        int subNodeId = static_cast<int>(i + 1);
        Vertex<int>* subVertex = flowGraph.findVertex(subNodeId);
        if (subVertex == nullptr) continue;

        const Submission& actualSub = data.submissions[i];

        for (Edge<int>* edge : subVertex->getAdj()) {
            if (edge->getFlow() == 1.0) {
                int revNodeId = edge->getDest()->getInfo();
                int revIndex = getReviewerRealId(revNodeId);
                const Reviewer& actualRev = data.reviewers[revIndex];

                std::cout << "source -> [" << actualSub.submissionId << " : " 
                          << (int)nodeAgg[subNodeId].first << "/" << (int)nodeAgg[subNodeId].second << "] -> [" 
                          << actualRev.reviewerId << " : " 
                          << (int)nodeAgg[revNodeId].first << "/" << (int)nodeAgg[revNodeId].second << "] -> sink" << std::endl;
                found = true;
            }
        }
    }

    if (!found) {
        std::cout << "No assignments found. Flow is 0." << std::endl;
    }
    std::cout << "-----------------------------------------\n" << std::endl;
}
std::pair<double, double> AssignmentSolver::getSubmissionFlow(int subId) const {
    int subNodeId = -1;
    for (size_t i = 0; i < data.submissions.size(); i++) {
        if (data.submissions[i].submissionId == subId) {
            subNodeId = static_cast<int>(i + 1);
            break;
        }
    }
    if (subNodeId == -1) return {0.0, 0.0};

    Vertex<int>* srcVertex = flowGraph.findVertex(0);
    if (srcVertex) {
        for (auto e : srcVertex->getAdj()) {
            if (e->getDest()->getInfo() == subNodeId) {
                return {e->getFlow(), e->getWeight()};
            }
        }
    }
    return {0.0, 0.0};
}

std::pair<double, double> AssignmentSolver::getReviewerFlow(int revId) const {
    int revNodeId = -1;
    for (size_t i = 0; i < data.reviewers.size(); i++) {
        if (data.reviewers[i].reviewerId == revId) {
            revNodeId = static_cast<int>(i + 1 + data.submissions.size());
            break;
        }
    }
    if (revNodeId == -1) return {0.0, 0.0};

    Vertex<int>* revVertex = flowGraph.findVertex(revNodeId);
    if (revVertex) {
        for (auto e : revVertex->getAdj()) {
            if (e->getDest()->getInfo() == static_cast<int>(data.reviewers.size() + data.submissions.size() + 1)) {
                return {e->getFlow(), e->getWeight()};
            }
        }
    }
    return {0.0, 0.0};
}
