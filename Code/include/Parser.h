#pragma once
#include <iostream>
#include <string>
#include <vector>
#include "ConferenceData.h"

/**
 * @file Parser.h
 * @brief CSV input file parser for the Conference Assignment Tool.
 */

/**
 * @class Parser
 * @brief Parses a structured CSV input file into a ConferenceData structure.
 *
 * The input file is divided into four sections:
 * - \#Submissions: paper submission records
 * - \#Reviewers: reviewer records
 * - \#Parameters: problem control parameters
 * - \#Control: output and execution settings
 *
 * Lines beginning with \# are treated as section headers or comments.
 * Inline comments (anything after \# on a data line) are stripped before parsing.
 * Quoted fields (e.g. "Title with, comma") are handled correctly.
 *
 * Example usage:
 * @code
 * Parser parser;
 * ConferenceData data = parser.parseFile("input.csv");
 * if (data.submissions.empty()) { // handle error }
 * @endcode
 */
class Parser {
public:
    /** @name Main Parsing */
    /** @{ */

    /**
     * @brief Parses a CSV input file and returns the populated ConferenceData.
     *
     * Reads the file section by section, delegating each row to the appropriate
     * section parser. After parsing, validates the data for mandatory fields
     * and duplicate identifiers.
     *
     * @param filename Path to the input CSV file (must have a .csv extension).
     * @return Populated ConferenceData struct. On failure (file not found, parse
     *         error, or validation failure), returns a default-constructed
     *         ConferenceData with empty submissions and reviewers.
     *
     * @complexity Time: O(C + N + M), where C = total input size (characters),
     *                   N = number of submissions, M = number of reviewers.
     */
    ConferenceData parseFile(const std::string& filename);

    /** @} */

private:
    /** @name Section Parsers */
    /** @{ */

    /**
     * @brief Parses a single submission row and appends it to data.submissions.
     * @param tokens Tokenised fields from the CSV row.
     * @param data   ConferenceData to populate.
     */
    void parseSubmission(const std::vector<std::string>& tokens, ConferenceData& data);

    /**
     * @brief Parses a single reviewer row and appends it to data.reviewers.
     * @param tokens Tokenised fields from the CSV row.
     * @param data   ConferenceData to populate.
     */
    void parseReviewer(const std::vector<std::string>& tokens, ConferenceData& data);

    /**
     * @brief Parses a parameter key-value pair and stores it in data.params.
     * @param tokens Tokenised fields from the CSV row.
     * @param data   ConferenceData to populate.
     */
    void parseParameter(const std::vector<std::string>& tokens, ConferenceData& data);

    /**
     * @brief Parses a control key-value pair and stores it in data.control.
     * @param tokens Tokenised fields from the CSV row.
     * @param data   ConferenceData to populate.
     */
    void parseControl(const std::vector<std::string>& tokens, ConferenceData& data);

    /** @} */

    /** @name Helper Functions */
    /** @{ */

    /**
     * @brief Converts a string to an integer.
     * @param str Input string (may contain surrounding whitespace).
     * @return Parsed integer, or -1 if conversion fails.
     */
    int toInt(const std::string& str);

    /**
     * @brief Strips inline comments from a line.
     *
     * Removes everything from the first \# character onward, unless the line
     * begins with \# (section header), in which case it is returned unchanged.
     *
     * @param line Raw input line.
     * @return Line with inline comment removed.
     */
    std::string stripComment(const std::string& line);

    /**
     * @brief Splits a CSV line into tokens, respecting quoted fields.
     *
     * Handles fields that contain commas inside double quotes, e.g.:
     * @code
     * 31, "Title with, comma", Author
     * @endcode
     *
     * @param line CSV line to split.
     * @return Vector of string tokens.
     */
    std::vector<std::string> splitLine(const std::string& line);

    /**
     * @brief Trims leading and trailing whitespace from a string.
     * @param str Input string.
     * @return Trimmed string.
     */
    std::string trim(const std::string& str);

    /** @} */

    /** @name Validation */
    /** @{ */

    /**
     * @brief Validates the parsed ConferenceData for correctness.
     *
     * Checks for:
     * - Presence of mandatory parameters (MinReviewsPerSubmission, MaxReviewsPerReviewer)
     * - Duplicate submission IDs
     * - Duplicate reviewer IDs
     *
     * Reports all errors to stderr before returning.
     *
     * @param data ConferenceData to validate.
     * @return true if data is valid, false otherwise.
     * @complexity Time: O(N + M), where N = submissions and M = reviewers.
     */
    bool validate(ConferenceData& data);

    /** @} */
};