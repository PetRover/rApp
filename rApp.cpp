//
// Created by Bryce Cater on 10/24/15.
//

#include <sys/socket.h>
#include <qimage.h>
#include "rApp.h"
//
//Canvas::Canvas(QWidget* parent)
//        : QOpenGLWidget(parent)
//{
//    setFixedSize(200, 200);
//    setAutoFillBackground(false);
//}
//
//void Canvas::setImage(const QImage& image)
//{
//    img = image;
//}
//
//void Canvas::paintEvent(QPaintEvent*)
//{
//    QPainter p(this);
//
//    //Set the painter to use a smooth scaling algorithm.
//    p.setRenderHint(QPainter::SmoothPixmapTransform, 1);
//
//    p.drawImage(this->rect(), img);
//}

rAppMainView::rAppMainView(QWidget *parent)
: QWidget(parent)
{
    this->networkManager = new RVR::NetworkManager();
    this->networkManager->setConnectTimeout(10000);

//    this->camera = new RVR::Camera(this->networkManager);

//    this->canvas = new Canvas(this);

    this->myLabel = new QLabel(this); // sets parent of label to main window

    this->currentCamChunk = new RVR::NetworkChunk();

    this->cameraImage = QImage( 640, 480, QImage::Format_RGB888);

    QPushButton *listenButton = new QPushButton("listen", this);
    QPushButton *driveForwardButton = new QPushButton("FORWARD", this);
    QPushButton *driveBackwardButton = new QPushButton("REVERSE", this);
    QPushButton *turnLeftButton = new QPushButton("LEFT", this);
    QPushButton *turnRightButton = new QPushButton("RIGHT", this);
    QPushButton *stopDrivingButton = new QPushButton("STOP", this);
    QPushButton *startStreamButton = new QPushButton("Start Stream", this);
    QPushButton *flipCameraButton = new QPushButton("Flip Camera", this);

    listenButton->resize(400, 30);
    driveForwardButton->resize(100, 100);
    driveBackwardButton->resize(100, 100);
    turnLeftButton->resize(100, 100);
    turnRightButton->resize(100, 100);
    stopDrivingButton->resize(100, 100);
    startStreamButton->resize(100, 100);
    flipCameraButton->resize(100, 100);

    this->titleLabel = new QLabel("ROVER", this);

    QGridLayout *mainLayout = new QGridLayout(this);
    QGridLayout *driveLayout = new QGridLayout(this);
    mainLayout->addWidget(this->titleLabel, 0, 0, Qt::AlignHCenter);
    mainLayout->addWidget(listenButton, 1, 0, Qt::AlignHCenter);
    mainLayout->addWidget(this->myLabel, 2, 0, Qt::AlignHCenter);
    mainLayout->addWidget(startStreamButton, 3, 0, Qt::AlignHCenter);
    mainLayout->addLayout(driveLayout, 4, 0, Qt::AlignHCenter);
    mainLayout->addWidget(flipCameraButton, 5, 0, Qt::AlignHCenter);

    driveLayout->addWidget(driveForwardButton, 0, 1, Qt::AlignHCenter);
    driveLayout->addWidget(driveBackwardButton, 2, 1, Qt::AlignHCenter);
    driveLayout->addWidget(turnLeftButton, 1, 0, Qt::AlignHCenter);
    driveLayout->addWidget(turnRightButton, 1, 2, Qt::AlignHCenter);
    driveLayout->addWidget(stopDrivingButton, 1, 1, Qt::AlignHCenter);

    this->setLayout(mainLayout);

    connect(listenButton, SIGNAL(clicked()), this, SLOT(listen()));
    connect(driveForwardButton, SIGNAL(clicked()), this, SLOT(driveForward()));
    connect(driveBackwardButton, SIGNAL(clicked()), this, SLOT(driveBackwards()));
    connect(turnLeftButton, SIGNAL(clicked()), this, SLOT(turnLeft()));
    connect(turnRightButton, SIGNAL(clicked()), this, SLOT(turnRight()));
    connect(stopDrivingButton, SIGNAL(clicked()), this, SLOT(stopDriving()));
    connect(startStreamButton, SIGNAL(clicked()), this, SLOT(startStream()));
    connect(flipCameraButton, SIGNAL(clicked()), this, SLOT(flipCamera()));

    this->frameTimer = new QTimer();
    connect(this->frameTimer, SIGNAL(timeout()), this, SLOT(getFrames()));
}

void rAppMainView::listen()
{
    this->networkManager->initializeNewConnection("COMMANDS", "192.168.7.1", "192.168.7.2", 1024, RVR::ConnectionInitType::LISTEN, RVR::ConnectionProtocol::TCP);
    this->networkManager->initializeNewConnection("CAMERA","192.168.7.1", "192.168.7.2", 1025, RVR::ConnectionInitType::LISTEN, RVR::ConnectionProtocol::UDP);

}

void rAppMainView::driveForward()
{
    VLOG(2) << "Issuing DRIVE_FORWARD command";
    RVR::Command cmd = RVR::Command();
    cmd.setCommandType(RVR::CommandType::DRIVE_FORWARD);
    cmd.setCommandData((char *)"50");
    RVR::NetworkChunk *nc = new RVR::NetworkChunk;
    *nc = cmd.toNetworkChunk();
    this->networkManager->sendData("COMMANDS", nc);
}

void rAppMainView::driveBackwards()
{
    VLOG(2) << "Issuing DRIVE_BACKWARD command";
    RVR::Command cmd = RVR::Command();
    cmd.setCommandType(RVR::CommandType::DRIVE_BACKWARD);
    cmd.setCommandData((char *)"50");
    RVR::NetworkChunk *nc = new RVR::NetworkChunk;
    *nc = cmd.toNetworkChunk();
    this->networkManager->sendData("COMMANDS", nc);
}

void rAppMainView::turnLeft()
{
    VLOG(2) << "Issuing TURN_LEFT command";
    RVR::Command cmd = RVR::Command();
    cmd.setCommandType(RVR::CommandType::TURN_LEFT);
//    cmd.setCommandData((char *)"50");
    RVR::NetworkChunk *nc = new RVR::NetworkChunk;
    *nc = cmd.toNetworkChunk();
    this->networkManager->sendData("COMMANDS", nc);
}

void rAppMainView::turnRight()
{
    VLOG(2) << "Issuing TURN_RIGHT command";
    RVR::Command cmd = RVR::Command();
    cmd.setCommandType(RVR::CommandType::TURN_RIGHT);
//    cmd.setCommandData((char *)"50");
    RVR::NetworkChunk *nc = new RVR::NetworkChunk;
    *nc = cmd.toNetworkChunk();
    VLOG(2) << "Command is all set up.. getting ready to send it";
    this->networkManager->sendData("COMMANDS", nc);
}

void rAppMainView::stopDriving()
{
    VLOG(2) << "Issuing STOP_DRIVE command";
    RVR::Command cmd = RVR::Command();
    cmd.setCommandType(RVR::CommandType::STOP_DRIVE);
    RVR::NetworkChunk *nc = new RVR::NetworkChunk;
    *nc = cmd.toNetworkChunk();
    this->networkManager->sendData("COMMANDS", nc);
}

void rAppMainView::startStream()
{
//    this->camera->startStream();

    VLOG(2) << "Issuing START_STREAM command";
    RVR::Command cmd = RVR::Command();
    cmd.setCommandType(RVR::CommandType::START_STREAM);
    RVR::NetworkChunk *nc = new RVR::NetworkChunk;
    *nc = cmd.toNetworkChunk();
    this->networkManager->sendData("COMMANDS", nc);
    this->frameTimer->start(0);
}

void rAppMainView::flipCamera()
{
    VLOG(2) << "Issuing FLIP_CAMERA command";
    RVR::Command cmd = RVR::Command();
    cmd.setCommandType(RVR::CommandType::FLIP_CAMPERA);
    RVR::NetworkChunk *nc = new RVR::NetworkChunk;
    *nc = cmd.toNetworkChunk();
    this->networkManager->sendData("COMMANDS", nc);
}

void rAppMainView::getFrames()
{
    VLOG(3) << "getting frames ";

    RVR::ReceiveType rt = this->networkManager->getData("CAMERA", this->currentCamChunk);
    if (rt == RVR::ReceiveType::NETWORKCHUNK)
    {
        if (this->currentCamChunk->getDataType() == RVR::DataType::CAMERA)
        {

            unsigned char* jpegData = (unsigned char*)this->currentCamChunk->getData();
            this->cameraImage.loadFromData(jpegData, this->currentCamChunk->getLength(), "JPEG");
            myLabel->setPixmap(QPixmap::fromImage(this->cameraImage));
        }
    }
    VLOG(3) << "[DONE] getting frames ";


//    }
}
