#include "../include/Menu.h"
#include "../include/Parser.h"
#include "../include/AssignmentSolver.h"
#include <filesystem>
#include <fstream>
#include <cstdlib>

Menu::Menu(const std::string& execPath) : execPath(execPath) {}

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
            case 10:
                handleRunTests();
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
    std::cout << " 10. Run Tests" << std::endl;
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

void Menu::handleRunTests() {
    namespace fs = std::filesystem;

    std::string inputDir, outputDir;

    std::cout << "\n========== Run Tests ==========\n";
    std::cout << "Enter path to input files directory: ";
    std::getline(std::cin, inputDir);
    std::cout << "Enter path to expected output files directory: ";
    std::getline(std::cin, outputDir);

    if (inputDir.empty() || outputDir.empty()) {
        std::cout << "Error: Input and output directory paths cannot be empty.\n";
        return;
    }

    if (!fs::exists(inputDir) || !fs::is_directory(inputDir)) {
        std::cout << "Error: Input directory does not exist: " << inputDir << "\n";
        return;
    }
    if (!fs::exists(outputDir) || !fs::is_directory(outputDir)) {
        std::cout << "Error: Output directory does not exist: " << outputDir << "\n";
        return;
    }

    if (!fs::exists(execPath)) {
        std::cout << "Executable not found. Compiling...\n";
        int ret = std::system("g++ -std=c++17 -Wall -I Code/include Code/src/*.cpp -o run_tests");
        if (ret != 0) {
            std::cout << "Compilation failed!\n";
            return;
        }
        std::cout << "Compilation successful!\n";
    }

    int passed = 0, failed = 0, skipped = 0;

    for (const auto& entry : fs::directory_iterator(inputDir)) {
        if (!entry.is_regular_file()) continue;
        if (entry.path().extension() != ".csv") continue;

        std::string inputPath = entry.path().string();
        std::string inputName = entry.path().filename().string();

        Parser parser;
        ConferenceData data = parser.parseFile(inputPath);
        if (data.submissions.empty() && data.reviewers.empty()) {
            std::cout << "  SKIPPED " << inputName << " (parse error)\n";
            skipped++;
            continue;
        }

        std::string outputFileName = data.control.outputFileName;
        std::cout << "\n  Testing " << inputName << " -> " << outputFileName << "\n";

        std::string cmd = execPath + " -b \"" + inputPath + "\"";
        std::system(cmd.c_str());

        std::string generatedPath = outputFileName;
        std::string expectedPath = (fs::path(outputDir) / ("output_" + inputName)).string();

        if (!fs::exists(generatedPath)) {
            std::cout << "  FAILED  " << inputName << " (no output file: " << outputFileName << ")\n";
            failed++;
            continue;
        }
        if (!fs::exists(expectedPath)) {
            std::cout << "  SKIPPED " << inputName << " (no expected file: output_" << inputName << ")\n";
            skipped++;
            continue;
        }

        std::ifstream genFile(generatedPath), expFile(expectedPath);
        std::string genLine, expLine;
        int genLineNum = 0, expLineNum = 0;
        bool match = true;
        std::string firstDiff;

        while (true) {
            bool hasGen = (bool)std::getline(genFile, genLine);
            bool hasExp = (bool)std::getline(expFile, expLine);
            genLineNum += hasGen;
            expLineNum += hasExp;

            if (!hasGen && !hasExp) break;

            if (hasGen != hasExp) {
                if (firstDiff.empty())
                    firstDiff = "  Line count differs (generated: " + std::to_string(genLineNum) +
                                ", expected: " + std::to_string(expLineNum) + ")";
                match = false;
                break;
            }
            if (genLine != expLine) {
                if (firstDiff.empty())
                    firstDiff = "  Line " + std::to_string(genLineNum) + " differs:\n"
                                "    got:      \"" + genLine + "\"\n"
                                "    expected: \"" + expLine + "\"";
                match = false;
            }
        }

        if (match) {
            std::cout << "  PASSED  " << inputName << "\n";
            passed++;
        } else {
            std::cout << "  FAILED  " << inputName << "\n";
            std::cout << firstDiff << "\n";
            failed++;
        }
    }

    std::cout << "\n========================================\n";
    std::cout << "  Passed:  " << passed << "\n";
    std::cout << "  Failed:  " << failed << "\n";
    std::cout << "  Skipped: " << skipped << "\n";
    std::cout << "  Total:   " << (passed + failed + skipped) << "\n";
    std::cout << "========================================\n";
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

    if (!solver) {
        solver = std::make_unique<AssignmentSolver>(data);
    }
    if (!assignmentGenerated) {
        solver->solve(FlowAlgorithm::EDMONDS_KARP);
        assignmentGenerated = true;
    }

    std::vector<int> riskyReviewers = solver->riskAnalysis();

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