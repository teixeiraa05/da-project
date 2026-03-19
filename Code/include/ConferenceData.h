#include <vector>
#include <string>

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

inline int domainsMatch(const Reviewer& rev, const Submission& sub, int mode) {
    bool primaryMatch = rev.primary == sub.primary;

    if (mode == 1) {
        if(primaryMatch) return sub.primary;
    } else if (mode == 2) {
        if(primaryMatch) return sub.primary;
        if(rev.primary == sub.secondary) return sub.secondary;
    } else if (mode == 3) {
        if(primaryMatch) return sub.primary;
        if(rev.primary == sub.secondary) return sub.secondary;
        if(rev.secondary == sub.primary) return sub.primary;
        if(rev.secondary == sub.secondary) return sub.secondary;
    }
    
    return 0;
}