#include "comtraderecord.hpp"

ComtradeRecord::ComtradeRecord() : isValid_(true) {
}

void ComtradeRecord::readComtradeData(const ComtradeConfig &config, const QByteArray &dataFileContent) {
    config_ = config;
    qDebug() << "config OK";
    if (config_.dataFileType() == ComtradeDataFileType::ascii) {
        readTextData(dataFileContent);
    } else {
        readBinaryData(dataFileContent);
    }
    formAnalogValuesFromRaw();
    formPrimaryAndSecondaryValues();
}

bool ComtradeRecord::isValid() const {
    return isValid_;
}

void ComtradeRecord::setIsValid(bool newValue) {
    isValid_ = newValue;
}

QList<QList<double>> ComtradeRecord::rawAnalogValues() const {
    return rawAnalogValues_;
}

QList<QList<double>> ComtradeRecord::analogValues() const {
    return analogValues_;
}

QList<QList<double>> ComtradeRecord::primaryAnalogValues() const {
    return primaryAnalogValues_;
}
QList<QList<double>> ComtradeRecord::secondaryAnalogValues() const {
    return secondaryAnalogValues_;
}

QList<QList<int>> ComtradeRecord::digitalValues() const {
    return digitalValues_;
}

QList<double> ComtradeRecord::timesOfValuesInMs() const {
    QList<double> result;
    result.reserve(config().lastSampleNumbers().at(0));
    quint64 intervalStart = config().firstValueTime();
    for (int sr = 0; sr < config().samplingRatesNumber(); sr++) {
        double timeDeltaInMs = 1000.0 / config().samplingRates().at(sr);
        for (int i = 0; i < config().lastSampleNumbers().at(sr); i++) {
            result.append(intervalStart / 1'000'000 + timeDeltaInMs * i);
        }
        intervalStart += timeDeltaInMs * (config().lastSampleNumbers().at(sr) - 1) * 1'000'000;
    }
    return result;
}

ComtradeConfig ComtradeRecord::config() const {
    return config_;
}

void ComtradeRecord::setConfig(const ComtradeConfig &config) {
    config_ = config;
}

QString ComtradeRecord::errorString() const {
    return errorString_;
}

int ComtradeRecord::analogChannelsNumber() const {
    return config_.analogChannelsNumber();
}
int ComtradeRecord::digitalChannelsNumber() const {
    return config_.digitalChannelsNumber();
}

ComtradeAnalogChannel ComtradeRecord::analogChannel(int channelIndex) const {
    return config_.analogChannel(channelIndex);
}
ComtradeDigitalChannel ComtradeRecord::digitalChannel(int channelIndex) const {
    return config_.digitalChannel(channelIndex);
}

template <typename DataType>
double readSample(QDataStream &stream) {
    DataType initialValue;
    stream >> initialValue;
    return static_cast<double>(initialValue);
}

void ComtradeRecord::readBinaryData(const QByteArray &dataFileContent) {
    qDebug() << "readBinaryData()" << dataFileContent.size();
    QDataStream dataFileStream(dataFileContent);
    dataFileStream.setByteOrder(QDataStream::LittleEndian);
    dataFileStream.setFloatingPointPrecision(QDataStream::SinglePrecision);
    //---
    int analogSampleSize = 0;
    ComtradeDataFileType dataFileType = config_.dataFileType();
    if (dataFileType == ComtradeDataFileType::binary) { analogSampleSize = 2; }
    else if (dataFileType == ComtradeDataFileType::binary32 or dataFileType == ComtradeDataFileType::float32) { analogSampleSize = 4; }
    if (analogSampleSize == 0) {}
    int bytesPerSample = (analogSampleSize * config_.analogChannelsNumber() + 2 * (config_.digitalChannelsNumber() / 16 + 1) + 8);
    int samplesCount = dataFileContent.size() / bytesPerSample;

    qDebug() << "IN PRACTICE:" << config_.lastSampleNumbers().at(config_.lastSampleNumbers().size() - 1) << "IN THEORY:" << samplesCount;
    // if (config_.lastSampleNumbers().at(config_.lastSampleNumbers().size() - 1) != samplesCount) {
    //     qDebug() << "Wrong data file size";
    //     errorString_ = "Wrong data file size";
    //     isValid_ = false;
    // }
    //---

    qDebug() << "CHANNELS:" << config_.analogChannelsNumber() << config_.digitalChannelsNumber() << config_.digitalChannelsNumber() / 16 + 1;
    qDebug() << "bytesPerSample:" << bytesPerSample;

    allocateData(samplesCount);
    for (int n = 0; n < samplesCount; n++) {
        qint32 recordNumber;
        qint32 timestamp;
        dataFileStream >> recordNumber;
//            qDebug() << recordNumber;
        dataFileStream >> timestamp;

        double analogValue;
        if (dataFileType == ComtradeDataFileType::binary) {
            for (int i = 0; i < config_.analogChannelsNumber(); i++) {
                rawAnalogValues_[i].append(readSample<qint16>(dataFileStream));
            }
        } else if (dataFileType == ComtradeDataFileType::binary32) {
            for (int i = 0; i < config_.analogChannelsNumber(); i++) {
                rawAnalogValues_[i].append(readSample<qint32>(dataFileStream));
            }
        } else if (dataFileType == ComtradeDataFileType::float32) {
            for (int i = 0; i < config_.analogChannelsNumber(); i++) {
                dataFileStream >> analogValue;
                rawAnalogValues_[i].append(analogValue);
            }
        }

        if (config_.digitalChannelsNumber() > 0) {
            quint16 digitalData;
            quint16 one = 1;
            for (int i = 0; i < config_.digitalChannelsNumber() / 16 + 1; i++) {
                dataFileStream >> digitalData;
                for (int j = 0; j < 15; j++) {
                    if (16 * i + j >= config_.digitalChannelsNumber()) { break; }
                    if ((digitalData & (one << j)) != 0) { digitalValues_[16 * i + j].append(1); }
                    else { digitalValues_[16 * i + j].append(0); }
                }
            }
        }
    }
    qDebug() << "DATA FILE SIZE IN SAMPLES:" << rawAnalogValues().at(0).size();
}

void ComtradeRecord::allocateData(int samplesCount) {
    for (int i = 0; i < config_.analogChannelsNumber(); i++) {
        QList<double> newList;
        newList.reserve(samplesCount);
        rawAnalogValues_.append(newList);
        analogValues_.append(newList);
        primaryAnalogValues_.append(newList);
        secondaryAnalogValues_.append(newList);
    }
    for (int i = 0; i < config_.digitalChannelsNumber(); i++) {
        QList<int> newList;
        newList.reserve(samplesCount);
        digitalValues_.append(newList);
    }
}

void ComtradeRecord::readTextData(const QByteArray &dataFileContent) {
    qDebug() << "READ TEXT DATA";
    QTextStream dataFileStream(dataFileContent);
    allocateData(qMax<int>(dataFileContent.count("\r"), dataFileContent.count("\n")));
    qDebug() << "COUNT:" << dataFileContent.count("\r") << dataFileContent.count("\n");
    while (dataFileStream.status() != QTextStream::ReadPastEnd  && !dataFileStream.atEnd()) {
        parseTextDataLine(dataFileStream.readLine());
    }
}

void ComtradeRecord::parseTextDataLine(const QString &dataLine) {
    int analogChannelsNumber = config().analogChannelsNumber();
    int digitalChannelsNumber = config().digitalChannelsNumber();
    if (dataLine.count(',') != analogChannelsNumber + digitalChannelsNumber + 1) { isValid_ = false; }
    QStringList lineContent = dataLine.split(',');

    bool convertedOk;
    for (int i = 0; i < lineContent.size(); i++) {
        if (i == 0) {
            // record number
        } else if (i == 1) {
            // time stamp
        } else if (i <= analogChannelsNumber + 1) {
            rawAnalogValues_[i - 2].append(lineContent.at(i).toDouble(&convertedOk));
            if (!convertedOk) { isValid_ = false; }
        } else if (i <= analogChannelsNumber + digitalChannelsNumber + 1) {
            int digitalValue = lineContent.at(i).toInt(&convertedOk);
            if (!convertedOk) { isValid_ = false; }
            if (digitalValue == 0 || digitalValue == 1) { digitalValues_[i - analogChannelsNumber - 2].append(digitalValue); }
            else { isValid_ = false; }

        }
    }
}

void ComtradeRecord::formAnalogValuesFromRaw() {
    for (auto oldChannelValues : analogValues_) { oldChannelValues.clear(); }
    if (rawAnalogValues_.size() != analogValues_.size()) { analogValues_.clear(); }
    for (int i = 0; i < rawAnalogValues_.size(); i++) {
        ComtradeAnalogChannel currentChannel(config().analogChannels().at(i));
        for (double rawValue : rawAnalogValues_.at(i)) {
            analogValues_[i].append(rawValue * currentChannel.multiplier() + currentChannel.offsetAdder());
        }
    }
}

void ComtradeRecord::formPrimaryAndSecondaryValues() {
    for (auto oldChannelValues : primaryAnalogValues_) { oldChannelValues.clear(); }
    if (rawAnalogValues_.size() != primaryAnalogValues_.size()) { primaryAnalogValues_.clear(); }
    for (auto oldChannelValues : secondaryAnalogValues_) { oldChannelValues.clear(); }
    if (rawAnalogValues_.size() != secondaryAnalogValues_.size()) { secondaryAnalogValues_.clear(); }
    for (int i = 0; i < rawAnalogValues_.size(); i++) {
        for (double rawValue : rawAnalogValues_.at(i)) {
            ComtradeAnalogChannel currentChannel(config().analogChannels().at(i));
            double value = rawValue * currentChannel.multiplier() + currentChannel.offsetAdder();
            if (currentChannel.scalingIdentifier() == AnalogChannelScalingIdentifier::primary) {
                primaryAnalogValues_[i].append(value);
                secondaryAnalogValues_[i].append(value * currentChannel.primaryFactor() / currentChannel.secondaryFactor());
            } else {
                primaryAnalogValues_[i].append(value * currentChannel.secondaryFactor() / currentChannel.primaryFactor());
                secondaryAnalogValues_[i].append(value);
            }
        }
    }
}
