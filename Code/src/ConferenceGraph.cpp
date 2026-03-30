#include "../include/ConferenceGraph.h"
#include <iostream>

ConferenceGraph::ConferenceGraph(const ConferenceData &data) : data(data) {}

Graph<int> ConferenceGraph::buildGraph() const {
    Graph<int> graph;
    graph.addVertex(getSourceID());
    graph.addVertex(getSinkID());

    // Add vertices for submissions and connect them to the source vertex
    for (size_t i = 0; i < data.submissions.size(); i++) {
	int subID = getSubmissionID(i);
	graph.addVertex(subID);
	graph.addEdge(getSourceID(), subID,
		      data.params.minReviewsPerSubmission);
    }

    // Add vertices for reviewers and connect them to the sink vertex ()
    for (size_t i = 0; i < data.reviewers.size(); i++) {
	int revID = getReviewerID(i);
	graph.addVertex(revID);
	graph.addEdge(revID, getSinkID(), data.params.maxReviewsPerReviewer);
    }

    // Connect submission vertices to reviewer vertices based on domain matching
    for (size_t i = 0; i < data.submissions.size(); i++) {
	for (size_t j = 0; j < data.reviewers.size(); j++) {
	    if (domainsMatch(data.submissions[i], data.reviewers[j],
			     data.control.generateAssignments)) {
		graph.addEdge(getSubmissionID(i), getReviewerID(j),
			      1); // Connect submission vertices to reviewer
				  // vertices with capacity 1
	    }
	}
    }

    return graph;
}


int ConferenceGraph::getReviewerID(size_t index) const {
    return static_cast<int>(index + 1 + data.submissions.size());
}

int ConferenceGraph::getSubmissionID(size_t index) const {
    return static_cast<int>(index + 1);
}

int ConferenceGraph::getSourceID() const { return 0; }

int ConferenceGraph::getSinkID() const {
    return static_cast<int>(data.reviewers.size() + data.submissions.size() +
			    1);
}