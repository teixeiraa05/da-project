#pragma once
#include <iostream>
#include <string>
#include <vector>
#include "ConferenceData.h"

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

        //validation functions
        bool validate(ConferenceData& data); //Validates the parsed data, returns true if valid, false otherwise. Also reports any issues found.

};  