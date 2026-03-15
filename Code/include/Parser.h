#pragma once
#include <iostream>
#include <string>
#include <vector>

//Data structures --------------------

struct Submission {
    int submissionId;
    std::string title;
    std::string author;
    std::string email;
    int primary;
    int secondary = -1; //-1 if not set
};

struct Reviewer {
    int reviewerId;
    std::string name;
    std::string email;
    int primary;  
    int secondary = -1; //-1 if not set
};

struct Parameters{
    int minReviewsPerSubmission = -1;
    int maxReviewsPerReviewer = -1;
    int primaryReviewerExpertise = 1;
    int secondaryReviewerExpertise = 0;
    int primarySubmissionDomain = 1;
    int secondarySubmissionDomain = 0;
};


struct Control {
    int generateAssignments = 0; //Controls the generation of an assignment of the submissions. 0: The assignment is carried out but not reported. 1: Consider the Primary Sumission domains and Primary Reviewer expertise. 2: Consider the Primary and Secondary Submission domains and exclusively the Primary Reviewer expertise. 3: Consider the Primary and Secondary Submission domains and the Primary and Secondary Reviewer expertise. 
    int riskAnalysis = 0; //Controls the generation of a risk analysis. 0: No risk analysis is carried out. 1: Determines if the assignment is still possible if any of the reviewers does not do his work. Reports the assignments carried out.
    std::string outputFileName = "output.csv";
};

struct ConferenceData {
    std::vector<Submission> submissions;
    std::vector<Reviewer> reviewers;
    Parameters params;
    Control control;
};

// PARSER ---------------------------------

class Parser {
    public:
        ConferenceData parseFile(const std::string& filename);

    private:
        //parse each section
        void parseSubmission(const std::vector<std::string>& tokens, ConferenceData& data);
        void parseReviewer(const std::vector<std::string>& tokens, ConferenceData& data);
        void parseParameter(const std::vector<std::string>& tokens, ConferenceData& data);
        void parseControl(const std::vector<std::string>& tokens, ConferenceData& data);

        //utility functions
        int toInt(const std::string& str); //Converts a string to an integer, returns -1 if conversion fails
        std::string stripComment(const std::string& line); //Removes comments from a line (anything after '#')
        std::vector<std::string> splitLine (const std::string& line); //Splits a line into tokens based on commas, accounting for empty fields
        std::string trim(const std::string& str); //Trims whitespace from both ends of a string

};  