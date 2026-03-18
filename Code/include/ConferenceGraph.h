#include "ConferenceData.h"
#include "Graph.h"



class ConferenceGraph {
    public:
        ConferenceGraph(const ConferenceData& data);

        //build the graph
        Graph<int> buildGraph() const;

    private:
        ConferenceData data;
        
        int getSourceID() const;
        int getReviewerID(int index) const;
        int getSubmissionID(int index) const;
        int getSinkID() const;

        bool domainsMatch(const Reviewer& rev, const Submission& sub) const; 
};