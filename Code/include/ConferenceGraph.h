#pragma once
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
        int getReviewerID(size_t index) const;
        int getSubmissionID(size_t index) const;
        int getSinkID() const;
};