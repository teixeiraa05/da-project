#include "../include/Menu.h"
#include "../include/Parser.h"

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
    // TODO
}

void Menu::handleShowReviewers() {
    // TODO
}

void Menu::handleShowParameters() {
    // TODO
}

void Menu::handleShowControl() {
    // TODO
}

void Menu::handleRunAssignment() {
    // TODO
}

void Menu::handleRiskAnalysis() {
    // TODO
}