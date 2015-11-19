//
// Created by Bryce Cater on 10/24/15.
//

#include <sys/socket.h>
#include <qimage.h>
#include "rApp.h"

Canvas::Canvas(QWidget* parent)
        : QOpenGLWidget(parent)
{
    setFixedSize(200, 200);
    setAutoFillBackground(false);
}

void Canvas::setImage(const QImage& image)
{
    img = image;
}

void Canvas::paintEvent(QPaintEvent*)
{
    QPainter p(this);

    //Set the painter to use a smooth scaling algorithm.
    p.setRenderHint(QPainter::SmoothPixmapTransform, 1);

    p.drawImage(this->rect(), img);
}

rAppMainView::rAppMainView(QWidget *parent)
: QWidget(parent)
{
    this->networkManager = new RVR::NetworkManager();
    this->networkManager->setConnectTimeout(10000);

    this->camera = new RVR::Camera(this->networkManager);

    this->canvas = new Canvas(this);

    this->myLabel = new QLabel(this); // sets parent of label to main window

    VLOG(2) << "Setting stream at YUYV, 640px by 480px @ 30fps";
    this->camera->setupStream(UVC_FRAME_FORMAT_YUYV, 640, 480, 30);
    VLOG(2) << "Setting callback function to saveFrame()";
    this->camera->setFrameCallback(RVR::queueFrame);
    this->camera->setAutoExposure(true);

    QPushButton *listenButton = new QPushButton("listen", this);
    QPushButton *driveForwardButton = new QPushButton("FORWARD", this);
    QPushButton *driveBackwardButton = new QPushButton("REVERSE", this);
    QPushButton *turnLeftButton = new QPushButton("LEFT", this);
    QPushButton *turnRightButton = new QPushButton("RIGHT", this);
    QPushButton *stopDrivingButton = new QPushButton("STOP", this);
    QPushButton *startStreamButton = new QPushButton("Start Stream", this);

    listenButton->resize(400, 30);
    driveForwardButton->resize(100, 100);
    driveBackwardButton->resize(100, 100);
    turnLeftButton->resize(100, 100);
    turnRightButton->resize(100, 100);
    stopDrivingButton->resize(100, 100);
    startStreamButton->resize(100, 100);

    this->titleLabel = new QLabel("ROVER", this);

    QGridLayout *mainLayout = new QGridLayout(this);
    QGridLayout *driveLayout = new QGridLayout(this);
    mainLayout->addWidget(this->titleLabel, 0, 0, Qt::AlignHCenter);
    mainLayout->addWidget(listenButton, 1, 0, Qt::AlignHCenter);
    mainLayout->addWidget(this->myLabel, 2, 0, Qt::AlignHCenter);
    mainLayout->addWidget(startStreamButton, 3, 0, Qt::AlignHCenter);
    mainLayout->addLayout(driveLayout, 4, 0, Qt::AlignHCenter);

    driveLayout->addWidget(driveForwardButton, 0, 1, Qt::AlignHCenter);
    driveLayout->addWidget(driveBackwardButton, 2, 1, Qt::AlignHCenter);
    driveLayout->addWidget(turnLeftButton, 1, 0, Qt::AlignHCenter);
    driveLayout->addWidget(turnRightButton, 1, 2, Qt::AlignHCenter);
    driveLayout->addWidget(stopDrivingButton, 1, 1, Qt::AlignHCenter);

    this->setLayout(mainLayout);

    connect(listenButton, SIGNAL(clicked()), this, SLOT(listen()));
    connect(driveForwardButton, SIGNAL(clicked()), this, SLOT(driveForward()));
    connect(driveBackwardButton, SIGNAL(clicked()), this, SLOT(drivBackwards()));
    connect(turnLeftButton, SIGNAL(clicked()), this, SLOT(turnLeft()));
    connect(turnRightButton, SIGNAL(clicked()), this, SLOT(turnRight()));
    connect(stopDrivingButton, SIGNAL(clicked()), this, SLOT(stopDriving()));
    connect(startStreamButton, SIGNAL(clicked()), this, SLOT(startStream()));

    QTimer *frameTimer = new QTimer();
    connect(frameTimer, SIGNAL(timeout()), this, SLOT(getFrames()));
    frameTimer->start(10);
}

void rAppMainView::listen()
{
//    this->networkManager->initializeNewConnection("COMMANDS", "192.168.7.1", "192.168.7.2", 1024, RVR::ConnectionInitType::LISTEN, RVR::ConnectionProtocol::UDP);
    this->networkManager->initializeNewConnection("COMMANDS", "192.168.7.1", "192.168.7.2", 1024, RVR::ConnectionInitType::LISTEN, RVR::ConnectionProtocol::TCP);
//    this->networkManager->initializeNewConnection("CAMERA", "127.0.0.1", 1025, RVR::ConnectionInitType::LISTEN);
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

void rAppMainView::startStream()
{
    this->camera->startStream();

//    VLOG(2) << "Issuing START_STREAM command";
//    RVR::Command cmd = RVR::Command();
//    cmd.setCommandType(RVR::CommandType::START_STREAM);
//    RVR::NetworkChunk nc = cmd.toNetworkChunk();
//    this->networkManager->sendData("COMMANDS", &nc);

}

void rAppMainView::getFrames()
{
    if (!this->camera->frameQueue.empty())
    {
        VLOG(3) << "Got frame from a queue with length: " << this->camera->frameQueue.size();
        RVR::NetworkChunk* nc = this->camera->frameQueue.front();
        this->camera->frameQueue.pop();
        unsigned char* yuvdata = (unsigned char*)nc->getData();


        SwsContext *ConversionContext;
        ConversionContext = sws_getCachedContext(NULL, 640, 480, AV_PIX_FMT_YUYV422 , 640, 480, AV_PIX_FMT_RGB24, SWS_BILINEAR, 0, 0, 0);

        AVFrame* yuvFrame = av_frame_alloc();
        AVFrame* rgbFrame = av_frame_alloc();

        int num_bytes = avpicture_get_size(AV_PIX_FMT_RGB24, 640, 480);
        uint8_t* rgbFrame_buffer = (uint8_t *)av_malloc(num_bytes*sizeof(uint8_t));

        avpicture_fill((AVPicture*)rgbFrame, rgbFrame_buffer, AV_PIX_FMT_RGB24, 640, 480);
        avpicture_fill((AVPicture*)yuvFrame, yuvdata, AV_PIX_FMT_YUYV422, 640, 480);

        sws_scale(ConversionContext, (const uint8_t* const*)yuvFrame->data, yuvFrame->linesize, 0, 480, rgbFrame->data, rgbFrame->linesize);

        QImage img = QImage( 640, 480, QImage::Format_RGB888);
        for( int y = 0; y < 480; ++y )
        {
            memcpy( img.scanLine(y), rgbFrame->data[0]+y * rgbFrame->linesize[0],  rgbFrame->linesize[0]  );
        }
        myLabel->setPixmap(QPixmap::fromImage(img));
//        this->canvas->setImage(img);
    }
}
