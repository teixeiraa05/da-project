#include "../include/Parser.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>

// -------------------------- PARSER ---------------------------------

ConferenceData Parser::parseFile(const std::string& filename) {
    /* 
    
    Parses the input file and fills the ConferenceData structure
    
    */
    ConferenceData data; 

    // Check for the .csv extension
    if (filename.size() < 4 || filename.substr(filename.size() - 4) != ".csv") {
        std::cerr << "Error: input file must have a .csv extension." << std::endl;
        return data;
    }

    std::ifstream file(filename);

    if (!file.is_open()) {
        std::cerr << "Error: could not open the file " << filename << std::endl;
        return data;
    }

    std::string line;
    std::string currentSection;
    
    while (std::getline(file, line)) {
        // Strip comment ONLY if line doesn't start with #
        if (!line.empty() && line[0] != '#')
            line = stripComment(line);

        line = trim(line);
        if (line.empty()) continue;
        

        /* Parse section headers */
        if (line[0] == '#') {

            if (line.find("#Submissions") != std::string::npos) currentSection = "SUB";
            else if (line.find("#Reviewers") != std::string::npos) currentSection = "REV";
            else if (line.find("#Parameters") != std::string::npos) currentSection = "PARAM";
            else if (line.find("#Control") != std::string::npos) currentSection = "CTRL";
            // If it's a comment line that doesn't match any section header (like #Id, Title, ...), we just ignore it
            continue;
        }

        
        std::vector<std::string> tokens = splitLine(line);
        std::stringstream ss(line);
        std::string token;


        // Dispatch to the appropriate parsing function based on the current section

        if (currentSection == "SUB") parseSubmission(tokens, data);
        else if (currentSection == "REV" && tokens.size() >= 4) parseReviewer(tokens, data);
        else if (currentSection == "PARAM" && tokens.size() >= 2) parseParameter(tokens, data);
        else if (currentSection == "CTRL" && tokens.size() >= 2) parseControl(tokens, data);
        else {
            std::cerr << "Warning: Unrecognized line in input file: " << line << std::endl;
        }
    }

    file.close();
    return data;
}
// -------------------------- PARSE SECTIONS ---------------------------------

    void Parser::parseSubmission(const std::vector<std::string>& tokens, ConferenceData& data) {
        Submission sub;
        if(tokens.size() < 5) {
            std::cerr << "Error: Invalid submission entry, expected at least 5 fields." << std::endl;
            return;
        }

        sub.submissionId = toInt(tokens[0]);
        sub.title = trim(tokens[1]);
        sub.author = trim(tokens[2]);
        sub.email = trim(tokens[3]);
        sub.primary = toInt(tokens[4]);
        if (tokens.size() > 5) sub.secondary = toInt(tokens[5]);
        
        data.submissions.push_back(sub);
    }

    void Parser::parseReviewer(const std::vector<std::string>& tokens, ConferenceData& data) {
        if(tokens.size() < 4) {
            std::cerr << "Error: Invalid reviewer entry, expected at least 4 fields." << std::endl;
            return;
        }

        Reviewer rev;
        rev.reviewerId = toInt(tokens[0]);
        rev.name = trim(tokens[1]);
        rev.email = trim(tokens[2]);
        rev.primary = toInt(tokens[3]);
        if (tokens.size() > 4) rev.secondary = toInt(tokens[4]);

        data.reviewers.push_back(rev);
    }

    void Parser::parseParameter(const std::vector<std::string>& tokens, ConferenceData& data) {
        if(tokens.size() < 2) {
            std::cerr << "Error: Invalid parameter entry, expected at least 2 fields." << std::endl;
            return;
        }

        std::string key = trim(tokens[0]);
        int val = toInt(tokens[1]);

        if (key == "MinReviewsPerSubmission") data.params.minReviewsPerSubmission = val;
        else if (key == "MaxReviewsPerReviewer") data.params.maxReviewsPerReviewer = val;
        else if (key == "PrimaryReviewerExpertise") data.params.primaryReviewerExpertise = val;
        else if (key == "SecondaryReviewerExpertise") data.params.secondaryReviewerExpertise = val;
        else if (key == "PrimarySubmissionDomain") data.params.primarySubmissionDomain = val;
        else if (key == "SecondarySubmissionDomain") data.params.secondarySubmissionDomain = val;

    }

    void Parser::parseControl(const std::vector<std::string>& tokens, ConferenceData& data) {
        if(tokens.size() < 2) {
            std::cerr << "Error: Invalid control entry, expected at least 2 fields." << std::endl;
            return;
        }

        std::string key = trim(tokens[0]);
        std::string val = trim(tokens[1]);
        if (key == "GenerateAssignments") data.control.generateAssignments = toInt(val);
        else if (key == "RiskAnalysis") data.control.riskAnalysis = toInt(val);
        else if (key == "OutputFileName") data.control.outputFileName = val;
    }


// -------------------------- HELPERS ---------------------------------

    std::string Parser::stripComment(const std::string& line) {
        size_t pos = line.find('#');
        return (pos != std::string::npos) ? line.substr(0, pos) : line;
    }


    std::string Parser::trim(const std::string& s) {
        size_t start = s.find_first_not_of(" \t\r\n");
        if (start == std::string::npos) return "";
        size_t end = s.find_last_not_of(" \t\r\n");
        std::string result = s.substr(start, end - start + 1);

        return result;
    }


    std::vector<std::string> Parser::splitLine(const std::string& line) {
        std::vector<std::string> res;
        std::string cur;
        bool inQuote = false; //there can be commas inside quotes, we should ignore them

        for (char c : line) {
            if (c == '"') inQuote = !inQuote;
            else if (c == ',' && !inQuote) {
                res.push_back(cur);
                cur.clear();
            } else {
                cur += c;
            }
        }

        res.push_back(cur); //push the last element
        return res;
    }

    int Parser::toInt(const std::string& s) {
        std::string cleaned = trim(s);
        
        if (cleaned.empty()) return -1;
        try {
            return std::stoi(cleaned);
        } catch (...) {
            return -1;
        };
    }

