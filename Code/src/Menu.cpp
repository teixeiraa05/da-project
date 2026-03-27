#include "../include/Menu.h"
#include "../include/Parser.h"
#include "../include/AssignmentSolver.h"

void Menu::run() {
    int choice;
    do {
        std::string input;
        displayMenu();
        std::cout << "Enter your choice: ";
        std::getline(std::cin, input);

        try {
            choice = std::stoi(input);
        } catch (...) {
            choice = -1; // Force default case in switch
        }

        switch (choice) {
            case 1:
                handleLoadFile();
                break;
            case 2:
                handleShowSubmissions();
                break;
            case 3:
                handleShowReviewers();
                break;
            case 4:
                handleShowParameters();
                break;
            case 5:
                handleShowControl();
                break;
            case 6:
                handleRunAssignmentEdmondsKarp();
                break;
            case 7:
                handleRunAssignmentFordFulkerson();
                break;
            case 8:
                handleRiskAnalysis();
                break;
            case 9:
                handleShowLastAssignments();
                break;
            case 0:
                std::cout << "Exiting..." << std::endl;
                break;
            default:
                std::cout << "Invalid choice. Please try again." << std::endl;
        }
    } while (choice != 0);
}

void Menu::displayMenu() {
    std::cout << "\nScientific Conference Organization Tool \n" << std::endl;
    std::cout << "============== MENU ============== " << std::endl;
    std::cout << " 1. Load data from file" << std::endl;
    std::cout << " 2. Show Submissions" << std::endl;
    std::cout << " 3. Show Reviewers" << std::endl;
    std::cout << " 4. Show Parameters" << std::endl;
    std::cout << " 5. Show Control Settings" << std::endl;
    std::cout << " 6. Run Assignment (Edmonds-Karp)" << std::endl;
    std::cout << " 7. Run Assignment (Ford-Fulkerson)" << std::endl;
    std::cout << " 8. Run Risk Analysis" << std::endl;
    std::cout << " 9. Show last assignments" << std::endl;
    std::cout << " 0. Exit" << std::endl;
    std::cout << "================================== " << std::endl;
}


void Menu::handleLoadFile() {
    std::string filepath;
    std::cout << "Enter file path: ";
    std::getline(std::cin, filepath);
    
    // Trim potential whitespace from the edges
    filepath.erase(0, filepath.find_first_not_of(" \t\n\r"));
    filepath.erase(filepath.find_last_not_of(" \t\n\r") + 1);

    if (filepath.empty()) {
        std::cout << "Error: File path cannot be empty." << std::endl;
        return;
    }
    Parser parser;

    ConferenceData loadedData = parser.parseFile(filepath);

    if (loadedData.submissions.empty() && loadedData.reviewers.empty()) {
        this->dataLoaded = false;
        std::cout << "Warning: No data loaded. Check if the file exists and has the correct format (.csv)." << std::endl;
    } else {
        this->data = loadedData;
        this->dataLoaded = true;
        this->assignmentGenerated = false;
        this->solver.reset();
        std::cout << "Data loaded successfully. " << this->data.submissions.size() << " submissions and "
                  << this->data.reviewers.size() << " reviewers found." << std::endl;
    }
}

void Menu::handleShowSubmissions() {
    if (!dataLoaded) {
        std::cout << "No data loaded. Please load a file first." << std::endl;
        return;
    }

    std::cout << "\nSubmissions:\n";
    for (const auto& sub : data.submissions) {
        std::cout << "ID: " << sub.submissionId;
        if (assignmentGenerated && solver) {
            auto flowCap = solver->getSubmissionFlow(sub.submissionId);
            std::cout << " [" << (int)flowCap.first << "/" << (int)flowCap.second << "]";
        }
        std::cout << ", Primary Domain: " << sub.primary 
                  << ", Secondary Domain: " << sub.secondary 
                  << std::endl;
    }
}

void Menu::handleShowReviewers() {
    if (!dataLoaded) {
        std::cout << "No data loaded. Please load a file first." << std::endl;
        return;
    }

    std::cout << "\nReviewers:\n";
    for (const auto& rev : data.reviewers) {
        std::cout << "ID: " << rev.reviewerId;
        if (assignmentGenerated && solver) {
            auto flowCap = solver->getReviewerFlow(rev.reviewerId);
            std::cout << " [" << (int)flowCap.first << "/" << (int)flowCap.second << "]";
        }
        std::cout << ", Primary Domain: " << rev.primary 
                  << ", Secondary Domain: " << rev.secondary 
                  << std::endl;
    }
}

void Menu::handleShowParameters() {
    if (!dataLoaded) {
        std::cout << "No data loaded. Please load a file first." << std::endl;
        return;
    }

    std::cout << "\nParameters:\n";
    std::cout << "Minimum reviews per submission: " << data.params.minReviewsPerSubmission << std::endl;
    std::cout << "Maximum reviews per reviewer: " << data.params.maxReviewsPerReviewer << std::endl;
}

void Menu::handleShowControl() {
    if (!dataLoaded) {
        std::cout << "No data loaded. Please load a file first." << std::endl;
        return;
    }

    std::cout << "\nControl Settings:\n";
    std::cout << "Generate Assignments: " << data.control.generateAssignments << std::endl;
    std::cout << "Risk Analysis: " << data.control.riskAnalysis << std::endl;
    std::cout << "Output File Name: " << data.control.outputFileName << std::endl;
}

void Menu::handleRunAssignmentEdmondsKarp() {
    if (!dataLoaded) {
        std::cout << "No data loaded. Please load a file first." << std::endl;
        return;
    }
    
    std::cout << "\nRunning the Assignment using Edmonds-Karp's Algorithm...\n";
    solver = std::make_unique<AssignmentSolver>(data);
    solver->solve(FlowAlgorithm::EDMONDS_KARP);
    assignmentGenerated = true;
}

void Menu::handleRunAssignmentFordFulkerson() {
    if (!dataLoaded) {
        std::cout << "No data loaded. Please load a file first." << std::endl;
        return;
    }

    std::cout << "\nRunning the Assignment using Ford Fulkerson's Algorithm...\n";
    solver = std::make_unique<AssignmentSolver>(data);
    solver->solve(FlowAlgorithm::FORD_FULKERSON);
    assignmentGenerated = true;
}

void Menu::handleShowLastAssignments() {
    if (!dataLoaded) {
        std::cout << "No data loaded. Please load a file first." << std::endl;
        return;
    }
    if (!assignmentGenerated || !solver) {
        std::cout << "No assignments generated yet. Run an algorithm first." << std::endl;
        return;
    }
    solver->printAssignments();
}

void Menu::handleRiskAnalysis() {
    if (!dataLoaded) {
        std::cout << "No data loaded. Please load a file first." << std::endl;
        return;
    }

    if (data.control.riskAnalysis == 0) {
        std::cout << "Risk analysis is disabled in the control settings.\n";
        return;
    }

    std::cout << "\nRunning Risk Analysis...\n";
    AssignmentSolver solver(data);
    std::vector<int> riskyReviewers = solver.riskAnalysis();

    std::cout << "\n--- Risk Analysis Results ---\n";
    if (riskyReviewers.empty()) {
        std::cout << "No risky reviewers found.\n";
    } else {
        std::cout << "Found " << riskyReviewers.size() << " risky reviewer(s): ";
        for (size_t i = 0; i < riskyReviewers.size(); i++) {
            std::cout << riskyReviewers[i] << (i == riskyReviewers.size() - 1 ? "" : ", ");
        }
        std::cout << "\n";
    }
    std::cout << "Results appended to: " << data.control.outputFileName << "\n";
    std::cout << "-----------------------------\n";
}