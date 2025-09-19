#include "simleoscreader.hpp"

#include <QDebug>
#include <QFile>
#include <QDataStream>


SimleOscReader::SimleOscReader(QObject *parent) : QObject{parent} {

}

void SimleOscReader::readComtradeRecord(const QString &address) {
    QFile *configFile = new QFile(address.mid(0, address.lastIndexOf('.')) + ".cfg");
    if (!configFile->open(QIODevice::ReadOnly)) {
        comtradeRecord_.setIsValid(false);
        return;
    }
    QByteArray headerFileContent = configFile->readAll();
    configFile->close();

    QFile *dataFile = new QFile(address.mid(0, address.lastIndexOf('.')) + ".dat");
    if (!dataFile->open(QIODevice::ReadOnly)) {
        comtradeRecord_.setIsValid(false);
        return;
    }
    QByteArray dataFileContent = dataFile->readAll();
    dataFile->close();

    ComtradeConfig comtradeConfig = ComtradeConfig();
    if (comtradeConfig.readFromByteArray(headerFileContent)) {
        qDebug() << "COMTRADE config valid";
        comtradeRecord_ = ComtradeRecord();
        comtradeRecord_.readComtradeData(comtradeConfig, dataFileContent);
    } else {
        qDebug() << "COMTRADE config NOT valid:";
        comtradeRecord_.setConfig(comtradeConfig);
        comtradeRecord_.setIsValid(false);
        return;
    }
}

ComtradeRecord SimleOscReader::comtradeRecord() const {
    return comtradeRecord_;
}
