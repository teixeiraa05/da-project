#include "../Code/include/Parser.h"
#include <iostream>

int passed = 0;
int failed = 0;

void check(bool condition, const std::string& testName) {
    if (condition) {
        std::cout << "  [PASS] " << testName << "\n";
        passed++;
    } else {
        std::cout << "  [FAIL] " << testName << "\n";
        failed++;
    }
}

void testPDFExample() {
    std::cout << "\n-- testPDFExample --\n";
    Parser p;
    ConferenceData d = p.parseFile("dataset/data.csv");

    // Submissions
    check(d.submissions.size() == 2,"2 submissions");
    check(d.submissions[0].submissionId == 31,"sub[0] ID = 31");
    check(d.submissions[0].title == "The Eternal Wheel of Reincarnation", "sub[0] title correct"); 
    check(d.submissions[0].author == "Ralph Kholer", "sub[0] author correct");
    check(d.submissions[0].email == "kholer@gmail.com", "sub[0] email correct");
    check(d.submissions[0].primary == 3, "sub[0] primary = 3");
    check(d.submissions[0].secondary == 4,"sub[0] secondary = 4");

    check(d.submissions[1].submissionId == 87,"sub[1] ID = 87");
    check(d.submissions[1].title == "GoDiva: A PIM Architecture","sub[1] title correct");
    check(d.submissions[1].author == "Jeff Draper", "sub[1] author correct");
    check(d.submissions[1].email == "draper@usc.edu","sub[1] email correct");
    check(d.submissions[1].primary == 1, "sub[1] primary = 1");
    check(d.submissions[1].secondary == -1, "sub[1] secondary = -1");
   // Reviewers
    check(d.reviewers.size() == 2, "2 reviewers");
    check(d.reviewers[0].reviewerId == 1, "rev[0] ID = 1");
    check(d.reviewers[0].name == "Jaqueline N. Chame","rev[0] name correct");
    check(d.reviewers[0].email == "jchame@yahoo.com","rev[0] email correct");
    check(d.reviewers[0].primary == 1, "rev[0] primary = 1");
    check(d.reviewers[0].secondary == -1,"rev[0] secondary = -1");

    check(d.reviewers[1].reviewerId == 2, "rev[1] ID = 2");
    check(d.reviewers[1].name == "Mary W. Hall", "rev[1] name correct");
    check(d.reviewers[1].email == "mhall@hotmail.edu", "rev[1] email correct");
    check(d.reviewers[1].primary == 1, "rev[1] primary = 1");
    check(d.reviewers[1].secondary == 4, "rev[1] secondary = 4");

    // Parameters
    check(d.params.minReviewsPerSubmission    == 5, "minReviewsPerSubmission = 5");
    check(d.params.maxReviewsPerReviewer      == 4, "maxReviewsPerReviewer = 4");
    check(d.params.primaryReviewerExpertise   == 1, "primaryReviewerExpertise = 1");
    check(d.params.secondaryReviewerExpertise == 0, "secondaryReviewerExpertise = 0");
    check(d.params.primarySubmissionDomain    == 1, "primarySubmissionDomain = 1");
    check(d.params.secondarySubmissionDomain  == 1, "secondarySubmissionDomain = 1");

    // Control
    check(d.control.generateAssignments == 1, "generateAssignments = 1");
    check(d.control.riskAnalysis        == 0, "riskAnalysis = 0");
    check(d.control.outputFileName      == "assign.csv", "outputFileName = assign.csv"); 
}

int main() {
    testPDFExample();

    std::cout << "\nSummary: " << passed << " passed, " << failed << " failed.\n";
    return 0;
}