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
#include <Qt>
#include "../rCore/easylogging++.h"
#include "rWifi.h"

class rAppMainView : public QWidget
{
Q_OBJECT // Must be used in classes that define their own signals and slots
private:
    QLabel *titleLabel;
    RVR::NetworkManager *networkManager;

private slots:
    void listen();
    void driveForward();
    void drivBackwards();
    void turnLeft();
    void turnRight();
    void stopDriving();
    void startStream();

public:
    rAppMainView(QWidget *parent = 0);
};

#endif //RAPP_RAPP_H
