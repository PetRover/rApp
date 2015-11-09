sudo //
// Created by Bryce Cater on 10/27/15.
//

#include "fakeRover.h"
#include "../rCore/easylogging++.h"
INITIALIZE_EASYLOGGINGPP;

int main(int argc, char *argv[])
{
    START_EASYLOGGINGPP(argc, argv);

    RVR::NetworkManager*netMan = new RVR::NetworkManager;
    netMan->initializeNewConnection("COMMANDS", "127.0.0.1", 1024, RVR::ConnectionInitType::CONNECT);

    //make and fill status with data
//    RVR::Text* dataToSend = new RVR::Text;
//    dataToSend->setLength(10);
//    char message[100] = "YESSSSS!!!";
//    dataToSend->setTextMessage(message);
//
//    //turn status into NetworkChunk
//    RVR::NetworkChunk* netChunk = new RVR::NetworkChunk;
//    *netChunk = dataToSend->toNetworkChunk();
//
//    //send NetworkChunk
//    netMan->sendData("CAM", netChunk);
    bool stop = false;
    RVR::NetworkChunk nc = RVR::NetworkChunk();
    while (!stop)
    {
        nc = netMan->getData("COMMANDS");
        if (nc.getDataType() == RVR::DataType::COMMAND)
        {
            RVR::Command cmd = RVR::Command(nc);
            if (cmd.getCommandType() == RVR::CommandType::STOP_DRIVE)
            {
                VLOG(1) << "Got a stop command... stopping";
                stop = true;
            }
            else
            {
                VLOG(1) << "Got a commnad with a value of: " << (int)cmd.getCommandType();
                nc = RVR::NetworkChunk();
            }
        }
        usleep(1000000);
    }

    return 0;
}