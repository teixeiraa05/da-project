#include <iostream>
#include <string>
#include "Parser.h"

class Menu {
    public:
        void run();

    private:
        Parser parser;
        bool dataLoaded = false;
        bool assignmentGenerated = false;
        ConferenceData data;

        void displayMenu();
        void handleLoadFile();

        //Options to show data
        void handleShowSubmissions();
        void handleShowReviewers();
        void handleShowParameters();
        void handleShowControl();

        //Assignment
        void handleRunAssignment();

        //Risk analysis
        void handleRiskAnalysis();

        void requireData();
};