//
// Created by Bryce Cater on 10/24/15.
//

#ifndef RAPP_RAPP_H
#define RAPP_RAPP_H

#include <QApplication>
#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QGridLayout>
#include <QProgressBar>
#include <QKeyEvent>
#include <QEvent>
//#include <QSizePolicy>
#include <QObject>
#include <Qt>
#include <QtGui>
#include "../rCore/easylogging++.h"
#include "rWifi.h"
#include "rCamera.h"
#include <QPainter>
#include <QTimer>


class rAppMainView : public QWidget
{
Q_OBJECT // Must be used in classes that define their own signals and slots
private:
    RVR::Command* cmd;
    QLabel *statusLabel;
    RVR::NetworkManager *networkManager;
    QLabel *roverStreamLabel;
    RVR::NetworkChunk* currentCamChunk;
    QImage cameraImage;
    QTimer *frameTimer;
//    QTimer *heartbeatTimer;
    QProgressBar* progressBar;
    QPushButton *connectButton;
    QPushButton *startStreamButton;
    QPushButton *flipCameraButton;
    QPushButton *dispenseTreatButton;
    QWidget *controlHolder;
    QWidget *treatCountHolder;
    QLabel* treatImageLabels[10];
    QImage* treatFull;
    QImage* treatUsed;
    QImage* chargingImage;
    QLabel* chargingLabel;
    RVR::NetworkChunk* currentCommandChunk;

    int treatCount;

    bool cameraFlipped = false;
    bool connected = false;

    void sendCommand(RVR::CommandType, short value);

private slots:
    void waitForConnection();
    void startStream();
    void flipCamera();
    void dispenseTreat();
    void getFrames();
//    void processHeartbeat();
    void updateTreatCount();

public:
    rAppMainView(QWidget *parent = 0);

    void keyPressEvent(QKeyEvent* e);
    void keyReleaseEvent(QKeyEvent *e);
    bool eventFilter(QObject *obj, QEvent *event);
};

#endif //RAPP_RAPP_H
