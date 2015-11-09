//
// Created by Bryce Cater on 10/28/15.
//
#include "../rCore/easylogging++.h"
INITIALIZE_EASYLOGGINGPP;

int main()
{
    char buffer[10];
    char bufHex[10];

    for(int j = 0; j < 10; j++)
    {
        sprintf(bufHex, "%X", buffer[j]);
    }

//    LOG(INFO) << output;

}

