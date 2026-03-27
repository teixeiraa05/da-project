#pragma once
#include <iostream>
#include "Parser.h"
#include "AssignmentSolver.h"
#include <memory>

class Menu {
    public:
        void run();

    private:
        Parser parser;
        bool dataLoaded = false;
        bool assignmentGenerated = false;
        ConferenceData data;
        std::unique_ptr<AssignmentSolver> solver;

        void displayMenu();
        void handleLoadFile();

        //Options to show data
        void handleShowSubmissions();
        void handleShowReviewers();
        void handleShowParameters();
        void handleShowControl();

        //Assignment
        void handleRunAssignmentEdmondsKarp();
        void handleRunAssignmentFordFulkerson();
        void handleShowLastAssignments();

        //Risk analysis
        void handleRiskAnalysis();

        void requireData();
};