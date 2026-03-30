#include "../include/Menu.h"
#include "../include/AssignmentSolver.h"

int main(int argc, char* argv[]) {
    //Batch mode: if arguments are provided, run the program in batch mode
    if (argc >= 3 && std::string(argv[1]) == "-b") {
        std::string inputFile = argv[2];
        std::string riskFile = (argc >= 4) ? argv[3] : "";
        
        Parser parser;
        ConferenceData data = parser.parseFile(inputFile);
        if (data.submissions.empty() || data.reviewers.empty()) return 1;

        AssignmentSolver solver(data);
        solver.solve();

        if (data.control.riskAnalysis > 0) {
            solver.riskAnalysis(riskFile);
        }

        return 0;
    }


    Menu menu(argv[0]);
    menu.run();
    return 0;
}