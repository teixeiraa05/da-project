#include "../include/Menu.h"

int main(int argc, char* argv[]) {
    //Batch mode: if arguments are provided, run the program in batch mode
    if (argc >= 3 && std::string(argv[1]) == "-b") {
        std::string inputFile = argv[2];
        std::string riskFile = (argc >= 4) ? argv[3] : "";
        
        //TODO - Implement batch mode processing using inputFile and riskFile
        //parse input file, run assignment, and if riskFile is provided, run risk analysis and output results to riskFile

        return 0;
    }


    Menu menu;
    menu.run();
    return 0;
}