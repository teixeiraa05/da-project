#include "../include/Parser.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>

std::string Parser::trim(std::string s) {
    if (s.empty()) return s;

    s.erase(0, s.find_first_not_of(" \t\r\n"));
    s.erase(s.find_last_not_of(" \t\r\n") + 1);

    // Remove standard quoutes
    s.erase(std::remove(s.begin(), s.end(), '\"'), s.end());

    auto removeQuotes = [&](const std::string& quote) {
        size_t pos;
        while ((pos = s.find(quote)) != std::string::npos) {
            s.erase(pos, quote.length());
        }
    };

    removeQuotes("“");
    removeQuotes("”");

    return s;
}

int Parser::toInt(std::string& s) {
    std::string cleaned = trim(s);

    if (cleaned.empty()) return -1;
    try {
        return std::stoi(cleaned);
    } catch (...) {
        return -1;
    }
}

ConferenceData Parser::parseFile(std::string& filename) {
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

        std::vector<std::string> tokens;
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