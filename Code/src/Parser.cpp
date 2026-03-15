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
    std::ifstream file(filename);

    if (!file.is_open()) {
        std::cerr << "Error: could not open the file " << filename << std::endl;
        return data;
    }

    std::string line;
    std::string currentSection = "";
    
    while (std::getline(file, line)) {
        if (line.empty()) continue;

        /* Parse section headers */
        if (line[0] == '#') {
            if (line.find("#Submissions") != std::string::npos) currentSection = "SUB";
            else if (line.find("#Reviewers") != std::string::npos) currentSection = "REV";
            else if (line.find("#Parameters") != std::string::npos) currentSection = "PARAM";
            else if (line.find("#Control") != std::string::npos) currentSection = "CTRL";
            continue;
        }

        if (line.find("Id, ") == 0 || line.find("MinReviews") == 0 || line.find("GenerateAssignments") == 0) {
            continue;
        }

        std::vector<std::string> tokens = splitLine(line);
        std::stringstream ss(line);
        std::string token;

        while (std::getline(ss, token, ',')) {
            tokens.push_back(token);
        }

        if (!line.empty() && line.back() == ',') tokens.push_back("");

        if (currentSection == "SUB" && tokens.size() >= 5) {
            Submission sub;
            sub.submissionId = toInt(tokens[0]);
            sub.title = trim(tokens[1]);
            sub.author = trim(tokens[2]);
            sub.email = trim(tokens[3]);
            sub.primary = toInt(tokens[4]);
            if (tokens.size() > 5) sub.secondary = toInt(tokens[5]);
            
            data.submissions.push_back(sub);
        }
        else if (currentSection == "REV" && tokens.size() >= 4) {
            Reviewer rev;
            rev.reviewerId = toInt(tokens[0]);
            rev.name = trim(tokens[1]);
            rev.email = trim(tokens[2]);
            rev.primary = toInt(tokens[3]);
            if (tokens.size() > 4) rev.secondary = toInt(tokens[4]);
            
            data.reviewers.push_back(rev);
        }
        else if (currentSection == "PARAM" && tokens.size() >= 2) {
            std::string key = trim(tokens[0]);
            int val = toInt(tokens[1]);

            if (key == "MinReviewsPerSubmission") data.params.minReviewsPerSubmission = val;
            else if (key == "MaxReviewsPerReviewer") data.params.maxReviewsPerReviewer = val;
            else if (key == "PrimaryReviewerExpertise") data.params.primaryReviewerExpertise = val;
            else if (key == "SecondaryReviewerExpertise") data.params.secondaryReviewerExpertise = val;
            else if (key == "PrimarySubmissionDomain") data.params.primarySubmissionDomain = val;
            else if (key == "SecondarySubmissionDomain") data.params.secondarySubmissionDomain = val;
        }
        else if (currentSection == "CTRL" && tokens.size() >= 2) {
            std::string key = trim(tokens[0]);

            if (key == "GenerateAssignments") data.control.generateAssignments = toInt(tokens[1]);
            else if (key == "RiskAnalysis") data.control.riskAnalysis = toInt(tokens[1]);
            else if (key == "OutputFileName") data.control.outputFileName = trim(tokens[1]);
        }
    }

    file.close();
    return data;
}
// -------------------------- PARSE SECTIONS ---------------------------------

    void Parser::parseSubmission(const std::vector<std::string>& tokens, ConferenceData& data) {
        //TODO
    }

    void Parser::parseReviewer(const std::vector<std::string>& tokens, ConferenceData& data) {
        //TODO
    }

    void Parser::parseParameter(const std::vector<std::string>& tokens, ConferenceData& data) {
        //TODO
    }

    void Parser::parseControl(const std::vector<std::string>& tokens, ConferenceData& data) {
        //TODO
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
        return s.substr(start, end - start + 1);
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

