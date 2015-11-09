//
// Created by Bryce Cater on 10/24/15.
//

#include <sys/socket.h>
#include "rApp.h"

rAppMainView::rAppMainView(QWidget *parent)
: QWidget(parent)
{
    this->networkManager = new RVR::NetworkManager();
    this->networkManager->setConnectTimeout(10000);

    QPushButton *listenButton = new QPushButton("listen", this);
    QPushButton *driveForwardButton = new QPushButton("FORWARD", this);
    QPushButton *driveBackwardButton = new QPushButton("REVERSE", this);
    QPushButton *turnLeftButton = new QPushButton("LEFT", this);
    QPushButton *turnRightButton = new QPushButton("RIGHT", this);

    listenButton->resize(400, 30);
    driveForwardButton->resize(100, 100);
    driveBackwardButton->resize(100, 100);
    turnLeftButton->resize(100, 100);
    turnRightButton->resize(100, 100);

    this->titleLabel = new QLabel("ROVER", this);

    QGridLayout *mainLayout = new QGridLayout(this);
    QGridLayout *driveLayout = new QGridLayout(this);
    mainLayout->addWidget(this->titleLabel, 0, 0, Qt::AlignHCenter);
    mainLayout->addWidget(listenButton, 1, 0, Qt::AlignHCenter);
    mainLayout->addLayout(driveLayout, 2, 0, Qt::AlignHCenter);

    driveLayout->addWidget(driveForwardButton, 0, 1, Qt::AlignHCenter);
    driveLayout->addWidget(driveBackwardButton, 2, 1, Qt::AlignHCenter);
    driveLayout->addWidget(turnLeftButton, 1, 0, Qt::AlignHCenter);
    driveLayout->addWidget(turnRightButton, 1, 2, Qt::AlignHCenter);

    this->setLayout(mainLayout);

    connect(listenButton, SIGNAL(clicked()), this, SLOT(listen()));
    connect(driveForwardButton, SIGNAL(clicked()), this, SLOT(driveForward()));
    connect(driveBackwardButton, SIGNAL(clicked()), this, SLOT(drivBackwards()));
    connect(turnLeftButton, SIGNAL(clicked()), this, SLOT(turnLeft()));
    connect(turnRightButton, SIGNAL(clicked()), this, SLOT(turnRight()));
}

void rAppMainView::listen()
{
    this->networkManager->initializeNewConnection("COMMANDS", "127.0.0.1", 1024, RVR::ConnectionInitType::LISTEN);
}

void rAppMainView::driveForward()
{
    VLOG(2) << "Issuing DRIVE_FORWARD command";
    RVR::Command cmd = RVR::Command();
    cmd.setCommandType(RVR::CommandType::DRIVE_FORWARD);
    cmd.setCommandData((char *)"50");
    RVR::NetworkChunk nc = cmd.toNetworkChunk();
    this->networkManager->sendData("COMMANDS", &nc);
}

void rAppMainView::drivBackwards()
{
    VLOG(2) << "Issuing DRIVE_BACKWARD command";
    RVR::Command cmd = RVR::Command();
    cmd.setCommandType(RVR::CommandType::DRIVE_BACKWARD);
    cmd.setCommandData((char *)"50");
    RVR::NetworkChunk nc = cmd.toNetworkChunk();
    this->networkManager->sendData("COMMANDS", &nc);
}

void rAppMainView::turnLeft()
{
    VLOG(2) << "Issuing TURN_LEFT command";
    RVR::Command cmd = RVR::Command();
    cmd.setCommandType(RVR::CommandType::TURN_LEFT);
//    cmd.setCommandData((char *)"50");
    RVR::NetworkChunk nc = cmd.toNetworkChunk();
    this->networkManager->sendData("COMMANDS", &nc);
}

void rAppMainView::turnRight()
{
    VLOG(2) << "Issuing TURN_RIGHT command";
    RVR::Command cmd = RVR::Command();
    cmd.setCommandType(RVR::CommandType::TURN_RIGHT);
//    cmd.setCommandData((char *)"50");
    RVR::NetworkChunk nc = cmd.toNetworkChunk();
    VLOG(2) << "Command is all set up.. getting ready to send it";
    this->networkManager->sendData("COMMANDS", &nc);
}

void rAppMainView::stopDriving()
{
    VLOG(2) << "Issuing STOP_DRIVE command";
    RVR::Command cmd = RVR::Command();
    cmd.setCommandType(RVR::CommandType::STOP_DRIVE);
    RVR::NetworkChunk nc = cmd.toNetworkChunk();
    this->networkManager->sendData("COMMANDS", &nc);
}
