//
// Created by Bryce Cater on 10/24/15.
//

#include <sys/socket.h>
#include <qimage.h>
#include <qgraphicsanchorlayout.h>
#include "rApp.h"
INITIALIZE_EASYLOGGINGPP;

QFont statusFont = QFont("Lucida Grande", 50, QFont::Bold, false);
QString buttonStyle = QString::fromUtf8("QPushButton{background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,"
                                                "stop: 0 #56D900, stop: 1 #50B700);"
                                                "border-style: solid;"
                                                "border-color: black;"
                                                "border-width: 2px;"
                                                "border-radius: 10px;}");

QString controlButtonStyle = QString::fromUtf8("QPushButton{background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,"
                                                "stop: 0 white, stop: 1 gray);"
                                                "border-style: solid;"
                                                "border-color: black;"
                                                "border-width: 2px;"
                                                "border-radius: 10px;}");

rAppMainView::rAppMainView(QWidget *parent)
: QWidget(parent)
{
    this->cmd = new RVR::Command();
    this->networkManager = new RVR::NetworkManager();
    this->networkManager->setConnectTimeout(10000); // how long to wait after waitForConnection has been pressed
    this->currentCamChunk = new RVR::NetworkChunk();

    this->cameraImage = QImage( CAMERA_RES_WIDTH, CAMERA_RES_HEIGHT, QImage::Format_RGB888);
    this->cameraImage.fill(Qt::black);

    this->connectButton = new QPushButton("CONNECT", this);

    this->controlHolder = new QWidget(this);
    this->startStreamButton = new QPushButton("Start Stream", this->controlHolder);
    this->flipCameraButton = new QPushButton("Flip Camera", this->controlHolder);
    this->dispenseTreatButton= new QPushButton("Dispense Treat", this->controlHolder);

    this->roverStreamLabel = new QLabel(this); // sets parent of label to main window
    this->statusLabel = new QLabel("ROVER", this);
    this->progressBar = new QProgressBar(this);
    this->progressBar->setMinimum(0);
    this->progressBar->setMaximum(0);
    this->progressBar->setValue(1);

    QGridLayout* layout = new QGridLayout(this);
    layout->addWidget(this->statusLabel, 0, 0, Qt::AlignCenter);
    layout->addWidget(this->progressBar, 1, 0, Qt::AlignCenter);
    this->progressBar->hide();
    this->statusLabel->hide();
    this->statusLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    this->statusLabel->setFont(statusFont);
    this->progressBar->setFixedWidth(300);

    this->connectButton->move(CAMERA_RES_WIDTH / 2 - 25, CAMERA_RES_HEIGHT / 2- 10);
    this->connectButton->setStyleSheet(buttonStyle);
    this->connectButton->resize(100, 60);


    this->flipCameraButton->setStyleSheet(controlButtonStyle);
    this->flipCameraButton->setFixedSize(100,60);
    this->startStreamButton->setStyleSheet(controlButtonStyle);
    this->startStreamButton->setFixedSize(100,60);
    this->dispenseTreatButton->setStyleSheet(controlButtonStyle);
    this->dispenseTreatButton->setFixedSize(100,60);
    this->controlHolder->hide();
    this->controlHolder->resize(CAMERA_RES_WIDTH, 60);
    this->controlHolder->move(0,CAMERA_RES_HEIGHT-60);
    QGridLayout* controlLayout = new QGridLayout(this->controlHolder);
    controlLayout->addWidget(this->flipCameraButton, 0, 0, Qt::AlignCenter);
    controlLayout->addWidget(this->startStreamButton, 0, 1, Qt::AlignCenter);
    controlLayout->addWidget(this->dispenseTreatButton, 0, 2, Qt::AlignCenter);
    this->flipCameraButton->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);

    this->startStreamButton->resize(100, 100);
    this->startStreamButton->move(CAMERA_RES_WIDTH / 2 - 25, CAMERA_RES_HEIGHT - 100);

    this->roverStreamLabel->move(25,25);
    this->roverStreamLabel->resize(CAMERA_RES_WIDTH, CAMERA_RES_HEIGHT);
    this->roverStreamLabel->setPixmap(QPixmap::fromImage(this->cameraImage));
    this->roverStreamLabel->stackUnder(this->connectButton);

    connect(this->connectButton, SIGNAL(clicked()), this, SLOT(waitForConnection()));
    connect(this->startStreamButton, SIGNAL(clicked()), this, SLOT(startStream()));
    connect(this->flipCameraButton, SIGNAL(clicked()), this, SLOT(flipCamera()));
    connect(this->dispenseTreatButton, SIGNAL(clicked()), this, SLOT(dispenseTreat()));

    this->frameTimer = new QTimer();
    connect(this->frameTimer, SIGNAL(timeout()), this, SLOT(getFrames()));

    this->heartbeatTimer = new QTimer();
    connect(this->heartbeatTimer, SIGNAL(timeout()), this, SLOT(processHeartbeat()));
}

void rAppMainView::waitForConnection()
{
    this->statusLabel->setText("Connecting...");
    this->connectButton->hide();
    this->setEnabled(false);
    this->progressBar->show();
    this->statusLabel->show();
    this->repaint();
    qApp->processEvents();
    if (this->networkManager->initializeNewConnectionAndConnect("COMMANDS", APP_IP, ROVER_IP, 1024, RVR::ConnectionInitType::LISTEN, RVR::ConnectionProtocol::TCP))
    {
        if (this->networkManager->initializeNewConnectionAndConnect("HEARTBEAT", APP_IP, ROVER_IP, 1026, RVR::ConnectionInitType::LISTEN, RVR::ConnectionProtocol::TCP))
        {
            if (this->networkManager->initializeNewConnectionAndConnect("CAMERA", APP_IP, ROVER_IP, 1025, RVR::ConnectionInitType::LISTEN, RVR::ConnectionProtocol::UDP))
            {
                this->connected = true;
                this->heartbeatTimer->start(1000);
            }
            else
            {
                this->networkManager->terminateConnection("COMMANDS");
                this->networkManager->terminateConnection("HEARTBEAT");
            }
        }
        else
        {
            this->networkManager->terminateConnection("COMMANDS");
        }

    }


    if (this->connected)
    {
        this->controlHolder->show();
    }
    else
    {
        this->connectButton->show();
    }
    this->progressBar->hide();
    this->statusLabel->hide();
    this->setEnabled(true);
}

void rAppMainView::startStream()
{
    this->sendCommand(RVR::CommandType::START_STREAM, 0);
    this->frameTimer->start(0);
}

void rAppMainView::flipCamera()
{
    this->sendCommand(RVR::CommandType::FLIP_CAMPERA, 0);
    this->cameraFlipped = !this->cameraFlipped;
}

void rAppMainView::dispenseTreat()
{
    this->sendCommand(RVR::CommandType::DISPENSE_TREAT, 0);
}

void rAppMainView::updateTreatCount()
{

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
            this->roverStreamLabel->setPixmap(QPixmap::fromImage(this->cameraImage));
        }
    }
    VLOG(3) << "[DONE] getting frames ";
}

void rAppMainView::processHeartbeat()
{
    this->networkManager->sendHeartBeat();
}


void rAppMainView::keyPressEvent(QKeyEvent *e)
{
    if (!e->isAutoRepeat())
    {
        VLOG(2) << "KEY PRESSED";
        switch (e->key())
        {
            case Qt::Key_Up:
                this->sendCommand(RVR::CommandType::DRIVE_FORWARD, 100);
                break;
            case Qt::Key_Down:
                this->sendCommand(RVR::CommandType::DRIVE_BACKWARD, 100);
                break;
            case Qt::Key_Left:
                this->sendCommand(RVR::CommandType::TURN_LEFT, 100);
                break;
            case Qt::Key_Right:
                this->sendCommand(RVR::CommandType::TURN_RIGHT, 100);
                break;
        }
    }
}

void rAppMainView::keyReleaseEvent(QKeyEvent *e)
{
    if (!e->isAutoRepeat())
    {
        switch (e->key())
        {
            case Qt::Key_Up:
                this->sendCommand(RVR::CommandType::DRIVE_FORWARD, -100);
                break;
            case Qt::Key_Down:
                this->sendCommand(RVR::CommandType::DRIVE_BACKWARD, -100);
                break;
            case Qt::Key_Left:
                this->sendCommand(RVR::CommandType::TURN_LEFT, -100);
                break;
            case Qt::Key_Right:
                this->sendCommand(RVR::CommandType::TURN_RIGHT, -100);
                break;
        }
    }
}

// To prevent swallowing arrow key events: http://www.qtcentre.org/threads/25487-Detect-Arrow-Keys
bool rAppMainView::eventFilter(QObject *obj, QEvent *event)
{
    QKeyEvent *keyEvent = NULL;
    bool result = false;

    if (event->type() == QEvent::KeyPress)
    {
        keyEvent = dynamic_cast<QKeyEvent*>(event);
        this->keyPressEvent(keyEvent);
        result = true;
    }
    else if (event->type() == QEvent::KeyRelease)
    {
        keyEvent = dynamic_cast<QKeyEvent*>(event);
        this->keyReleaseEvent(keyEvent);
        result = true;
    }
    else if (event->type() == QEvent::ApplicationDeactivate)
    {
        VLOG(1) << "Losing focus, stopping drive!";
        this->sendCommand(RVR::CommandType::STOP_ALL, -100);
    }

    else
        result = QObject::eventFilter(obj, event);

    return result;
}

void rAppMainView::sendCommand(RVR::CommandType type, short value)
{
    VLOG(2) << "Issuing [ "<< (int)type <<" ] command with value of: " << value;
    if (this->connected)
    {
        this->cmd->setCommandType(type);
        char data[4];

        data[2] = (value >> 8) & 0xFF;
        data[3] = value & 0xFF;

        this->cmd->setCommandData((char*)data);
        RVR::NetworkChunk* nc = this->cmd->toNetworkChunk();
        this->networkManager->sendData("COMMANDS", nc);
    }
    else
    {
        LOG(WARNING) << "Could not send because there is no connection!";
    }

}


int main(int argc, char *argv[])
{
    START_EASYLOGGINGPP(argc, argv);
    QApplication app(argc, argv);

    rAppMainView window;

    window.resize(CAMERA_RES_WIDTH + 50, CAMERA_RES_HEIGHT + 50);
    window.setWindowTitle("Rover");
    window.show();

    // Installing global event filter
    QCoreApplication *rApp = QCoreApplication::instance();
    rApp->installEventFilter(&window);

    return app.exec();
}
