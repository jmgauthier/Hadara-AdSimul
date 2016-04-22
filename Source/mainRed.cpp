#include <iostream>
#include <chrono>

#include "CGraph.h"
#include "CGraphUtility.h"
#include "CWFReducer.h"


void PrintLogo(std::string Version)
{
    std::cout << "                   _                 \n"
            "    /\\  /\\__ _  __| | __ _ _ __ __ _ \n"
            "   / /_/ / _` |/ _` |/ _` | '__/ _` |\n"
            "  / __  / (_| | (_| | (_| | | | (_| |\n"
            "  \\/ /_/ \\__,_|\\__,_|\\__,_|_|  \\__,_|\n"
            "                                     \n"
            "    _       _ __  _                 _ \n"
            "   /_\\   __| / _\\(_)_ __ ___  _   _| |\n"
            "  //_\\\\ / _` \\ \\ | | '_ ` _ \\| | | | |\n"
            " /  _  \\ (_| |\\ \\| | | | | | | |_| | |\n"
            " \\_/ \\_/\\__,_\\__/|_|_| |_| |_|\\__,_|_|" << std::endl << std::endl;

    std::cout << std::endl << " *** Hadara - AdSimul - Red *** " <<  Version << std::endl << std::endl;
}
bool isAtomic(CPetriNet * cPetriNet)
{
    if(cPetriNet->GetNodes()->size() != 3)
    {
        return false;
    }

    tNode * firstNode = *(cPetriNet->GetTransitions()->begin());
    tNodeSet * inFirstNode = firstNode->GetInNeighbors();
    tNodeSet * outFirstNode = firstNode->GetOutNeighbors();

    if(inFirstNode->size() == 1 && outFirstNode->size()  == 1)
    {
        tNode * i = *(inFirstNode->begin());
        tNode * o = *(outFirstNode->begin());
        if(i->GetInNeighbors()->size() == 0 && o->GetOutNeighbors()->size() == 0)
        {
            return true;
        }
    }
    return false;
}

int main(int argc, char* argv[])
{
    try
    {
        std::string Version = "Alpha 0.1.2";
        PrintLogo(Version);

        if(argc != 2)
        {
            throw new LDException("Invalid args, usage : [sWFPath]");
        }

        std::string sWFPath(argv[1]);

        CPetriNet * cGraph = CGraphUtility::GetPetriNetFromXML(sWFPath);

        CWFReducer * cWFReducer = new CWFReducer(cGraph);
        cWFReducer->Initialize();

        std::cout << "Reducing workflow " << cGraph->GetLabel() << "(size : " << cGraph->GetNodes()->size() << ").." << std::endl;

        int initialSize = cGraph->GetNodes()->size();

        cWFReducer->ReduceWF();

        std::cout << std::endl;

        std::cout << "*RemovedPlaces: " << cWFReducer->GetRemovedPlaces() << std::endl;
        std::cout << "*RemovedTransitions: " << cWFReducer->GetRemovedTransitions() << std::endl;
        std::cout << "*RemovedSelfloopTransitions: " << cWFReducer->GetRemovedSelfloopTransitions() << std::endl;
        std::cout << "*RemovedConvergentPlaces: " << cWFReducer->GetRemovedConvergentPlaces() << std::endl;
        std::cout << "*RemovedDivergentPlaces: " << cWFReducer->GetRemovedDivergentPlaces() << std::endl;

        std::cout << std::endl;

        std::cout << "**Reduced workflow size: " << cWFReducer->GetReducedWF()->GetNodes()->size() << ")" << std::endl;
        std::cout << "**ReductionDuration t(ms): " << cWFReducer->GetReductionDuration() << std::endl;

        std::cout <<  std::endl;

        if(isAtomic(cWFReducer->GetReducedWF()))
        {
            std::cout <<  cGraph->GetLabel() << " is a generalised sound workflow ! **** SUCCESS ***" << std::endl;
        }
        else
        {
            std::cout << "Could not decide the generalised soundness of " <<  cGraph->GetLabel() << " !" << std::endl;
            cWFReducer->GetReducedWF()->SaveAsDot("dot/red_"+cWFReducer->GetReducedWF()->GetLabel()+".dot");
            cWFReducer->GetReducedWF()->SaveAsXML("xml/red_"+cWFReducer->GetReducedWF()->GetLabel()+".xml");
        }

        std::cout <<  std::endl;

        /*std::ofstream outfile;
        outfile.open("results.txt", std::ios::app);
        outfile << sWFPath << " " << initialSize << " " << cWFReducer->GetReductionDuration() << " " << cWFReducer->GetReducedWF()->GetNodes()->size() << std::endl;
        */

        cWFReducer->Terminate();
    }
    catch ( LDException * e )
    {
        std::cout << "Exception was caught: " << e->what() << "\n";
    }

    return 0;
}
