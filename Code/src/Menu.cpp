#include "../include/Menu.h"
#include "../include/Parser.h"
#include "../include/AssignmentSolver.h"

void Menu::run() {
    int choice;
    do {
        displayMenu();
        std::cout << "Enter your choice: ";
        std::cin >> choice;

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
                handleRunAssignment();
                break;
            case 7:
                handleRiskAnalysis();
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
    std::cout << " 6. Run Assignment" << std::endl;
    std::cout << " 7. Run Risk Analysis" << std::endl;
    std::cout << " 0. Exit" << std::endl;
    std::cout << "================================== " << std::endl;
}


void Menu::handleLoadFile() {
    std::string filepath;
    std::cout << "Enter file path: ";
    std::cin >> filepath;
    Parser parser;

    ConferenceData loadedData = parser.parseFile(filepath);

    if (loadedData.submissions.empty() && loadedData.reviewers.empty()) {
        this->dataLoaded = false;
    } else {
        this->data = loadedData;
        this->dataLoaded = true;
        std::cout << "Data loaded successfully. " << this->data.submissions.size() << " submissions found." << std::endl;
    }
}

void Menu::handleShowSubmissions() {
    if (!dataLoaded) {
        std::cout << "No data loaded. Please load a file first." << std::endl;
        return;
    }

    std::cout << "\nSubmissions:\n";
    for (const auto& sub : data.submissions) {
        std::cout << "ID: " << sub.submissionId 
                  << ", Primary Domain: " << sub.primary 
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
        std::cout << "ID: " << rev.reviewerId 
                  << ", Primary Domain: " << rev.primary 
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

void Menu::handleRunAssignment() {
    if (!dataLoaded) {
        std::cout << "No data loaded. Please load a file first." << std::endl;
        return;
    }
    
    std::cout << "\n Running the Assignment... \n";
    AssignmentSolver solver(data);
    solver.solve();

}

void Menu::handleRiskAnalysis() {
    if (!dataLoaded) {
        std::cout << "No data loaded. Please load a file first." << std::endl;
        return;
    }
    
    std::cout << "\n Running Risk Analysis... \n";
    AssignmentSolver solver(data);
    solver.riskAnalysis();
}