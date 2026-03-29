#pragma once
#include <iostream>
#include <string>
#include "Parser.h"

/**
 * @file Menu.h
 * @brief Interactive command-line menu for the Conference Assignment Tool.
 */

/**
 * @class Menu
 * @brief Provides an interactive command-line interface for the assignment tool.
 *
 * The menu manages the full lifecycle of the tool in interactive mode:
 * loading data, displaying it, running the assignment algorithm and
 * performing risk analysis. It acts as the controller layer — delegating
 * all logic to Parser and AssignmentSolver.
 *
 * Example usage:
 * @code
 * Menu menu;
 * menu.run();   // blocks until user selects exit
 * @endcode
 *
 * Menu options:
 * -# Load input CSV file
 * -# Show submissions
 * -# Show reviewers
 * -# Show parameters
 * -# Show control settings
 * -# Run assignment (Edmonds-Karp)
 * -# Run assignment (Ford-Fulkerson)
 * -# Run risk analysis
 * -# Exit
 */
class Menu {
public:
    /**
     * @brief Starts the interactive menu loop.
     *
     * Displays the menu, reads user input and dispatches to the appropriate
     * handler. Loops until the user selects the exit option.
     *
     * @complexity Time:  O(n * f) where n = number of interactions and
     *                    f = cost of the chosen operation per iteration.
     * @complexity Space: O(1) — no additional memory beyond member variables.
     */
    void run();

private:
    Parser         parser;                    ///< Parser instance used to load and validate input files
    bool           dataLoaded         = false; ///< True after a CSV file has been successfully parsed
    bool           assignmentGenerated = false; ///< True after an assignment has been successfully computed
    ConferenceData data;                       ///< Holds the parsed conference data (submissions, reviewers, parameters)

    /**
     * @brief Renders the main menu to stdout.
     * @complexity Time: O(1). Space: O(1).
     */
    void displayMenu();

    /**
     * @brief Prompts the user for a filename and loads the CSV input file.
     *
     * On success, sets dataLoaded = true and resets assignmentGenerated = false.
     * On failure, prints an error message and leaves dataLoaded unchanged.
     *
     * @complexity Time:  O(N + M) for parsing, where N = submissions, M = reviewers.
     * @complexity Space: O(N + M) for storing parsed data.
     */
    void handleLoadFile();

    /**
     * @brief Displays all submissions in a formatted table sorted by ID.
     * @pre dataLoaded == true
     * @complexity Time:  O(N log N) for sorting + O(N) for display.
     * @complexity Space: O(N) for the sorted copy.
     */
    void handleShowSubmissions();

    /**
     * @brief Displays all reviewers in a formatted table sorted by ID.
     * @pre dataLoaded == true
     * @complexity Time:  O(M log M) for sorting + O(M) for display.
     * @complexity Space: O(M) for the sorted copy.
     */
    void handleShowReviewers();

    /**
     * @brief Displays all problem parameters currently loaded.
     * @pre dataLoaded == true
     * @complexity Time: O(1). Space: O(1).
     */
    void handleShowParameters();

    /**
     * @brief Displays the control settings currently loaded.
     * @pre dataLoaded == true
     * @complexity Time: O(1). Space: O(1).
     */
    void handleShowControl();

    /**
     * @brief Runs the reviewer assignment using the Edmonds-Karp algorithm.
     *
     * Builds the flow network, runs Edmonds-Karp (BFS-based Max-Flow) and
     * exports the results to the output file specified in the control settings.
     * Sets assignmentGenerated = true on success.
     *
     * @pre dataLoaded == true
     * @complexity Time:  O(V * E²) where V = N+M+2, E = O(N*M).
     * @complexity Space: O(V + E) for the flow graph.
     */
    void handleRunAssignmentEdmondsKarp();

    /**
     * @brief Runs the reviewer assignment using the Ford-Fulkerson algorithm.
     *
     * Builds the flow network, runs Ford-Fulkerson (DFS-based Max-Flow) and
     * exports the results to the output file specified in the control settings.
     * Sets assignmentGenerated = true on success.
     *
     * @pre dataLoaded == true
     * @complexity Time:  O(E * maxFlow) where maxFlow ≤ N * MinReviewsPerSubmission.
     * @complexity Space: O(V) for the DFS recursion stack.
     */
    void handleRunAssignmentFordFulkerson();

    /**
     * @brief Runs the risk analysis based on the RiskAnalysis control parameter.
     *
     * For RiskAnalysis = 1: identifies reviewers whose removal makes the
     * assignment infeasible. Results are appended to the output file.
     *
     * @pre dataLoaded == true
     * @pre assignmentGenerated == true
     * @complexity Time:  O(M * (N*M + V*E²)) where M = reviewers.
     * @complexity Space: O(N + M) per iteration.
     */
    void handleRiskAnalysis();

    /**
     * @brief Prints a warning message if no data has been loaded yet.
     *
     * Used as a guard in menu handlers that require data to be loaded first.
     *
     * @complexity Time: O(1). Space: O(1).
     */
    void requireData();
};