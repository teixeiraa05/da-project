#pragma once
#include <vector>
#include <string>

/**
 * @file ConferenceData.h
 * @brief Core data structures and domain matching logic for the Conference Assignment Tool.
 */

/**
 * @struct Submission
 * @brief Represents a scientific paper submission to the conference.
 */
struct Submission {
    int submissionId;        ///< Unique identifier for the submission
    std::string title;       ///< Title of the paper
    std::string author;      ///< Author(s) of the paper
    std::string email;       ///< Contact email of the author
    int primary;             ///< Primary topic domain identifier
    int secondary = -1;      ///< Secondary topic domain identifier (-1 if not set)
};

/**
 * @struct Reviewer
 * @brief Represents a reviewer assigned to evaluate submissions.
 */
struct Reviewer {
    int reviewerId;          ///< Unique identifier for the reviewer
    std::string name;        ///< Full name of the reviewer
    std::string email;       ///< Contact email of the reviewer
    int primary;             ///< Primary domain of expertise
    int secondary = -1;      ///< Secondary domain of expertise (-1 if not set)
};

/**
 * @struct Parameters
 * @brief Problem control parameters parsed from the input CSV file.
 */
struct Parameters {
    int minReviewsPerSubmission    = -1; ///< Minimum number of reviewers required per submission (-1 if not set)
    int maxReviewsPerReviewer      = -1; ///< Maximum number of submissions a reviewer can handle (-1 if not set)
    int primaryReviewerExpertise   =  1; ///< Weight of primary reviewer expertise (default: 1)
    int secondaryReviewerExpertise =  0; ///< Weight of secondary reviewer expertise (default: 0)
    int primarySubmissionDomain    =  1; ///< Weight of primary submission domain (default: 1)
    int secondarySubmissionDomain  =  0; ///< Weight of secondary submission domain (default: 0)
};

/**
 * @struct Control
 * @brief Output and execution control settings parsed from the input CSV file.
 */
struct Control {
    /**
     * @brief Controls which domain combinations are used for assignment.
     * - 0: Assignment is carried out but not reported.
     * - 1: Primary submission domains and primary reviewer expertise only.
     * - 2: Primary and secondary submission domains, primary reviewer expertise only.
     * - 3: All primary and secondary domains for both submissions and reviewers.
     */
    int generateAssignments = 0;

    /**
     * @brief Controls the risk analysis mode.
     * - 0: No risk analysis.
     * - 1: Checks if assignment is still possible if any single reviewer is removed.
     * - K (K>1): Checks if assignment is still possible if any K reviewers are removed.
     */
    int riskAnalysis = 0;

    std::string outputFileName = "output.csv"; ///< Name of the output file (default: output.csv)
};

/**
 * @struct ConferenceData
 * @brief Aggregates all parsed data from the input CSV file.
 */
struct ConferenceData {
    std::vector<Submission> submissions; ///< List of paper submissions
    std::vector<Reviewer>   reviewers;   ///< List of reviewers
    Parameters              params;      ///< Problem parameters
    Control                 control;     ///< Execution and output control settings
};

/**
 * @brief Determines if a reviewer can review a submission based on domain matching.
 *
 * Checks domain compatibility between a submission and a reviewer according
 * to the specified assignment mode. Priority is always given to primary domain
 * matches over secondary domain matches.
 *
 * Mode behaviour:
 * - Mode 1: Only primary submission domain vs primary reviewer expertise.
 * - Mode 2: Primary and secondary submission domains vs primary reviewer expertise.
 * - Mode 3: All combinations of primary and secondary domains on both sides.
 *
 * @param sub  The submission to check.
 * @param rev  The reviewer to check.
 * @param mode The assignment mode (1, 2, or 3).
 * @return The matched domain ID if a match exists, 0 otherwise.
 *
 * @note Priority order within mode 3:
 *       1. primary submission vs primary reviewer
 *       2. secondary submission vs primary reviewer
 *       3. primary submission vs secondary reviewer
 *       4. secondary submission vs secondary reviewer
 *
 * @complexity Time: O(1) — constant number of comparisons regardless of input size.
 * @complexity Space: O(1) — no additional memory allocated.
 */
inline int domainsMatch(const Submission& sub, const Reviewer& rev, int mode) {
    bool primaryMatch = rev.primary == sub.primary;
    bool subHasSecondary = sub.secondary >= 0;
    bool revHasSecondary = rev.secondary >= 0;

    if (mode == 1) {
        if (primaryMatch) return sub.primary;
    } else if (mode == 2) {
        if (primaryMatch) return sub.primary;
        if (subHasSecondary && rev.primary == sub.secondary) return sub.secondary;
    } else if (mode == 3) {
        if (primaryMatch) return sub.primary;
        if (subHasSecondary && rev.primary == sub.secondary)  return sub.secondary;
        if (revHasSecondary && rev.secondary == sub.primary)  return sub.primary;
        if (subHasSecondary && revHasSecondary && rev.secondary == sub.secondary) return sub.secondary;
    }

    return 0;
}