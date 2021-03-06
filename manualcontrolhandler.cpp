#include "manualcontrolhandler.h"

ManualControlHandler::ManualControlHandler(QQuickItem* parent):
    QQuickItem(parent), m_log(""),
    m_x(0), m_y(0), m_z(0), m_r(0),
    m_voltage(0), m_current(0),
    m_latitude(0), m_longitude(0), m_height(0){
    serial = new MavSerialPort(this);
    initSerialPort();
    initSerialConnections();
}

QString ManualControlHandler::log() const{
    return m_log;
}

int ManualControlHandler::x() const{
    return m_x;
}
int ManualControlHandler::y() const{
    return m_y;
}
int ManualControlHandler::z() const{
    return m_z;
}
int ManualControlHandler::r() const{
    return m_r;
}

int ManualControlHandler::voltage() const{
    return m_voltage;
}

int ManualControlHandler::current() const{
    return m_current;
}


double ManualControlHandler::latitude() const{
    return m_latitude;
}

double ManualControlHandler::longitude() const{
    return m_longitude;
}

double ManualControlHandler::height() const{
    return m_height;
}

void ManualControlHandler::setLog(QString l){
    if(m_log.compare(l)){
        m_log = l;
        emit logChanged();
    }
}

void ManualControlHandler::setX(int x){
    if(m_x != x){
        m_x = x;
        emit xChanged(x);
    }
}

void ManualControlHandler::setY(int y){
    if(m_y != y){
        m_y = y;
        emit yChanged(y);
    }
}

void ManualControlHandler::setZ(int z){
    if(m_z != z){
        m_z = z;
        emit zChanged(z);
    }
}

void ManualControlHandler::setR(int r){
    if(m_r != r){
        m_r = r;
        emit rChanged(r);
    }
}

void ManualControlHandler::setVoltage(int v){
    if(m_voltage != v){
        m_voltage = v;
        emit voltageChanged(v);
    }
}

void ManualControlHandler::setCurrent(int i){
    if(m_current != i){
        m_current = i;
        emit currentChanged(i);
    }
}

void ManualControlHandler::setBattery(int v, int i){
    setVoltage(v);
    setCurrent(i);
}


void ManualControlHandler::setLatitude(double l){
    if(abs(m_latitude - l) > 0.001){
        m_latitude = l;
        emit latitudeChanged(l);
    }
}

void ManualControlHandler::setLongitude(double l){
    if(abs(m_longitude - l) > 0.001){
        m_longitude = l;
        emit longitudeChanged(l);
    }
}

void ManualControlHandler::setHeight(double h){
    if(abs(m_height - h) > 0.001){
        m_height = h;
        emit heightChanged(h);
    }
}

void ManualControlHandler::initSerialPort(){
    //note that apple products use absolute address
    //need to deal with open error
    serial->setPortName("/dev/cu.SLAB_USBtoUART");
    serial->setBaudRate(QSerialPort::Baud57600);
    serial->setDataBits(QSerialPort::Data8);
    serial->setParity(QSerialPort::NoParity);
    serial->setStopBits(QSerialPort::OneStop);
    serial->setFlowControl(QSerialPort::NoFlowControl);
    if (serial->open(QIODevice::ReadWrite)) {
       qDebug() << "serial port is open now";
    }else {
       qDebug() << "serial port open fails";
       serial->stopTimer();
       serial->close();
    }
}

void ManualControlHandler::initSerialConnections(){
    connect(serial, SIGNAL(readyRead()), this, SLOT(readData()));
    connect(serial, SIGNAL(flightLogReady()), this, SLOT(writeFlightLog()));
    connect(serial,SIGNAL(batteryChanged(int, int)),this, SLOT(setBattery(int,int)));
    connect(serial, SIGNAL(globalChanged()), this, SLOT(updateLocation()));
    connect(this, SIGNAL(xChanged(int)), serial, SLOT(setX(int)));
    connect(this, SIGNAL(yChanged(int)), serial, SLOT(setY(int)));
    connect(this, SIGNAL(zChanged(int)), serial, SLOT(setZ(int)));
    connect(this, SIGNAL(rChanged(int)), serial, SLOT(setR(int)));
}

void ManualControlHandler::readData(){
    QByteArray data = serial->readAll();
    serial->mavRead(&data);
}

void ManualControlHandler::writeFlightLog(){
    setLog(QString(serial->statustext.text));
}

void ManualControlHandler::setArmed(bool armed){
    if (armed) {
        serial->set_mode_arm();
    }
    else {
        serial->set_mode_disarm();
    }
}

void ManualControlHandler::updateLocation(){
    setLatitude(serial->latitude());
    setLongitude(serial->longitude());
    setHeight(serial->relative_altitude());
}

// Controls flight modes
void ManualControlHandler::setFlightMode(int m){
    switch (m){
    case 0:
        serial->set_mode_return();
        qDebug() <<"*********MODE SET TO RETURN*********";
        break;
    case 1:
        serial->set_mode_manual();
        qDebug() << "*********MODE SET TO MANUAL*********";
        break;
    case 2:
        serial->set_mode_assist_altctl();
        qDebug() << "*********MODE SET TO ASSIST ALTCTL*********";
        break;
    case 3:
        serial->set_mode_assist_posctl();
        qDebug() << "*********MODE SET TO ASSIST POSTCTL*********";
        break;
    case 4:
        serial->set_mode_auto_mission();
        qDebug() << "*********MODE SET TO AUTO MISSION*********";
        break;
    case 5:
        serial->set_mode_auto_loiter();
        qDebug() << "*********MODE SET TO AUTO LOITER*********";
        break;
    }
}
