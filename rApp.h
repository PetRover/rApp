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
//#include <rCamera.h>
#include "../rCore/easylogging++.h"
#include "rWifi.h"
#include <QOpenGLWidget>
#include <QPainter>
#include <QTimer>
extern "C" {
#include <libswscale/swscale.h>
#include <libavutil/frame.h>
#include <libavcodec/avcodec.h>
}

class Canvas : public QOpenGLWidget
{
public:
    Canvas(QWidget* parent = NULL);
    void setImage(const QImage& image);
protected:
    void paintEvent(QPaintEvent*);
private:
    QImage img;
};

class rAppMainView : public QWidget
{
Q_OBJECT // Must be used in classes that define their own signals and slots
private:
    QLabel *titleLabel;
    RVR::NetworkManager *networkManager;
//    RVR::Camera* camera;
    Canvas* canvas;
    QLabel *myLabel;
    RVR::NetworkChunk* currentCamChunk;
    QImage cameraImage;
    QTimer *frameTimer;

private slots:
    void listen();
    void driveForward();
    void drivBackwards();
    void turnLeft();
    void turnRight();
    void stopDriving();
    void startStream();
    void flipCamera();
    void getFrames();

public:
    rAppMainView(QWidget *parent = 0);
};

#endif //RAPP_RAPP_H
