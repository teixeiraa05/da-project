#pragma once
#include <iostream>
#include "Parser.h"
#include "AssignmentSolver.h"
#include <memory>

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
 * Menu menu(argv[0]);
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
 * -# Show last assignments
 * -# Run tests
 * -# Exit
 */
class Menu {
public:
    /**
     * @brief Constructs the Menu with the path to the current executable.
     * @param execPath Path to the executable (typically argv[0]).
     */
    Menu(const std::string& execPath = "");

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
        std::unique_ptr<AssignmentSolver> solver;
        std::string execPath;                      ///< Path to the current executable (argv[0])

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
     * @brief Displays all submissions.
     * @pre dataLoaded == true
     * @complexity Time:  O(N) for display.
     * @complexity Space: O(1).
     */
    void handleShowSubmissions();

    /**
     * @brief Displays all reviewers.
     * @pre dataLoaded == true
     * @complexity Time:  O(M) for display.
     * @complexity Space: O(1).
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
     * assignment infeasible. If no assignment has been generated yet,
     * automatically runs Edmonds-Karp first. Results are appended to the output file.
     *
     * @pre dataLoaded == true
     * @complexity Time:  O(M * (N*M + V*E²)) where M = reviewers.
     * @complexity Space: O(N + M) per iteration.
     */
    void handleRiskAnalysis();

    /**
     * @brief Displays the last generated assignments.
     *
     * Prints the assignments from the most recently generated solver.
     * Requires that an assignment has been generated first.
     *
     * @pre dataLoaded == true
     * @pre assignmentGenerated == true
     * @complexity Time:  O(A) where A = number of assignments.
     * @complexity Space: O(1).
     */
    void handleShowLastAssignments();

    /**
     * @brief Runs the test suite on input/output file pairs.
     *
     * Prompts the user for input directory and output directory.
     * For each .csv in the input directory, runs the executable in batch mode,
     * then compares the generated output against the expected output file
     * (same filename with "output_" prefix) in the output directory.
     *
     * @complexity Time:  O(F * (P + D)) where F = files, P = parse+solve, D = diff.
     * @complexity Space: O(L) where L = longest file line.
     */
    void handleRunTests();
};