#include "comtradeconfig.hpp"

#include <QDebug>
#include <QDataStream>

#include <chrono>

/*
 * ComtradeChannel
 *
 * */

ComtradeChannel::ComtradeChannel() :
        channelNumber_(-1),
        name_(QString()),
        phase_(QString()),
        ccbm_(QString()) {
}

QString ComtradeChannel::name() const {
    return name_;
}

QString ComtradeChannel::phase() const {
    return phase_;
}

QString ComtradeChannel::ccbm() const {
    return ccbm_;
}

PhaseType ComtradeChannel::phaseTypeFromPhase() const {
    const QString &phaseUpper = phase_.toUpper();
    if (phase_.size() == 0) { return PhaseType::phaseNone; }
    else if (phase_.size() == 1) {
        if (phaseUpper == "A" || phaseUpper == "А" || phaseUpper == "1") { return PhaseType::phaseA; }
        if (phaseUpper == "B" || phaseUpper == "Б" || phaseUpper == "2") { return PhaseType::phaseB; }
        if (phaseUpper == "C" || phaseUpper == "В" || phaseUpper == "3") { return PhaseType::phaseC; }
        if (phaseUpper == "N" || phaseUpper == "Н") { return PhaseType::phaseNeutral; }
        if (phaseUpper == "G") { return PhaseType::phaseGround; }
    } else if (phase_.size() == 2) {
        if (phaseUpper == "UA" || phaseUpper == "VA" || phaseUpper == "IA") { return PhaseType::phaseA; }
        if (phaseUpper == "UB" || phaseUpper == "VB" || phaseUpper == "IB") { return PhaseType::phaseB; }
        if (phaseUpper == "UC" || phaseUpper == "VC" || phaseUpper == "IC") { return PhaseType::phaseC; }
    }
    return PhaseType::phaseNone;
}

PhaseType ComtradeChannel::phaseTypeFromPhaseAndName() const {
    PhaseType resultFromPhase = phaseTypeFromPhase();
    if (resultFromPhase != PhaseType::phaseNone) { return resultFromPhase; }

    const QString &nameUpper = name_.toUpper();
    if (nameUpper == "A" || nameUpper == "А" || nameUpper == "1") { return PhaseType::phaseA; }
    if (nameUpper == "B" || nameUpper == "Б" || nameUpper == "2") { return PhaseType::phaseB; }
    if (nameUpper == "C" || nameUpper == "В" || nameUpper == "3") { return PhaseType::phaseC; }
    if (nameUpper.startsWith("UA") || nameUpper.startsWith("VA") || nameUpper.startsWith("IA")) { return PhaseType::phaseA; }
    if (nameUpper.startsWith("UB") || nameUpper.startsWith("VB") || nameUpper.startsWith("IB")) { return PhaseType::phaseB; }
    if (nameUpper.startsWith("UC") || nameUpper.startsWith("VC") || nameUpper.startsWith("IC")) { return PhaseType::phaseC; }
//    QString nameUpperBegin = nameUpper.split(' ', Qt::SkipEmptyParts)[0];
//    if (nameUpperBegin.size() != 2) { nameUpperBegin = nameUpper.split('_', Qt::SkipEmptyParts)[0]; }
//    if (nameUpperBegin.size() == 2) {
//        if (nameUpperBegin == "UA" || nameUpperBegin == "VA" || nameUpperBegin == "IA") { return PhaseType::phaseA; }
//        if (nameUpperBegin == "UB" || nameUpperBegin == "VB" || nameUpperBegin == "IB") { return PhaseType::phaseB; }
//        if (nameUpperBegin == "UC" || nameUpperBegin == "VC" || nameUpperBegin == "IC") { return PhaseType::phaseC; }
//    }
//    QString nameUpperEnd = nameUpper.split(' ', Qt::SkipEmptyParts)[nameUpper.split(' ', Qt::SkipEmptyParts).size() - 1];
//    if (nameUpperEnd.size() != 2) { nameUpperEnd = nameUpper.split('_', Qt::SkipEmptyParts)[nameUpper.split('_', Qt::SkipEmptyParts).size() - 1]; }
//    if (nameUpperEnd.size() != 2) { nameUpperEnd = nameUpper.split(':', Qt::SkipEmptyParts)[nameUpper.split(':', Qt::SkipEmptyParts).size() - 1]; }
//    if (nameUpperEnd.size() == 2) {
//        if (nameUpperEnd == "UA" || nameUpperEnd == "VA" || nameUpperEnd == "IA") { return PhaseType::phaseA; }
//        if (nameUpperEnd == "UB" || nameUpperEnd == "VB" || nameUpperEnd == "IB") { return PhaseType::phaseB; }
//        if (nameUpperEnd == "UC" || nameUpperEnd == "VC" || nameUpperEnd == "IC") { return PhaseType::phaseC; }
//    }
    return PhaseType::phaseNone;
}

QString ComtradeChannel::errorString() const {
    return errorString_;
}

/*
 * ComtradeAnalogChannel
 *
 * */

ComtradeAnalogChannel::ComtradeAnalogChannel() :
        units_(QString()),
        multiplier_(1),
        offsetAdder_(0),
        timeSkewMs_(0),
        min_(0),
        max_(0),
        primaryFactor_(1),
        secondaryFactor_(1),
        scalingIdentifier_(AnalogChannelScalingIdentifier::primary) {
}

bool ComtradeAnalogChannel::readFromString(const QString &dataLine, const int standardRevisionYear) {
    QStringList lineContent = dataLine.split(',');
    qDebug() << "ANALOG CHANNEL PARAMS:" << lineContent.size();
    if ((standardRevisionYear == 0 && lineContent.size() != 10) || (standardRevisionYear > 0 && lineContent.size() != 13)) {
        errorString_ = "Wrong number of fields in analog channel information";
        return false;
    }
    bool convertedOk;
    channelNumber_ = lineContent[0].toInt(&convertedOk);
    if (!convertedOk) {
        errorString_ = "Analog channel index number must be an integer";
        return false;
    }
    name_ = lineContent[1];
    phase_ = lineContent[2];
    ccbm_ = lineContent[3];
    units_ = lineContent[4];
    multiplier_ = lineContent[5].toDouble(&convertedOk);
    if (!convertedOk) {
        errorString_ = "Channel multiplier must be a real number";
        return false;
    }
    offsetAdder_ = lineContent[6].toDouble(&convertedOk);
    if (!convertedOk) {
        errorString_ = "Channel offset adder must be a real number";
        return false;
    }
    if (lineContent[7].size() == 0) {
        timeSkewMs_ = 0;
    } else {
        timeSkewMs_ = lineContent[7].toDouble(&convertedOk);
        if (!convertedOk) {
            errorString_ = "Channel time skew must be a real number";
            return false;
        }
    }
    min_ = lineContent[8].toDouble(&convertedOk);
    if (!convertedOk) {
        errorString_ = "Channel minimum data value must be a real number";
        return false;
    }
    max_ = lineContent[9].toDouble(&convertedOk);
    if (!convertedOk) {
        errorString_ = "Channel maximum data value must be a real number";
        return false;
    }
    if (standardRevisionYear == 0) { return true; }

    primaryFactor_ = lineContent[10].toDouble(&convertedOk);
    if (!convertedOk) {
        errorString_ = "Channel transformer ratio primary factor must be a real number";
        return false;
    }
    secondaryFactor_ = lineContent[11].toDouble(&convertedOk);
    if (!convertedOk) {
        errorString_ = "Channel transformer ratio secondary factor must be a real number";
        return false;
    }
    if (lineContent[12].toUpper() == "P") {
        scalingIdentifier_ = AnalogChannelScalingIdentifier::primary;
    } else if (lineContent[12].toUpper() == "S") {
        scalingIdentifier_ = AnalogChannelScalingIdentifier::secondary;
    } else {
        qDebug() << "Channel data scaling identifier must be one of the following: P, S, p, s";
        return false;
    }
    qDebug() << channelNumber_ << name_ << phase_ << ccbm_ << units_ << multiplier_ << offsetAdder_ << timeSkewMs_ << min_ << max_ << primaryFactor_
             << secondaryFactor_ << (int) scalingIdentifier_ << (int) phaseTypeFromPhaseAndName();
    return true;
}

QString ComtradeAnalogChannel::units() const {
    return units_;
}

double ComtradeAnalogChannel::multiplier() const {
    return multiplier_;
}

double ComtradeAnalogChannel::offsetAdder() const {
    return offsetAdder_;
}

double ComtradeAnalogChannel::timeSkewMs() const {
    return timeSkewMs_;
}

double ComtradeAnalogChannel::min() const {
    return min_;
}

double ComtradeAnalogChannel::max() const {
    return max_;
}

double ComtradeAnalogChannel::primaryFactor() const {
    return primaryFactor_;
}

double ComtradeAnalogChannel::secondaryFactor() const {
    return secondaryFactor_;
}

AnalogChannelScalingIdentifier ComtradeAnalogChannel::scalingIdentifier() const {
    return scalingIdentifier_;
}

/*
 * ComtradeDigitalChannel
 *
 * */

ComtradeDigitalChannel::ComtradeDigitalChannel() :
    normalState_(false) {
}

bool ComtradeDigitalChannel::readFromString(const QString &dataLine, const int standardRevisionYear) {
    QList<QString> lineContent = dataLine.split(',');
    if ((standardRevisionYear == 0 && lineContent.size() != 3) || (standardRevisionYear > 0 && lineContent.size() != 5)) {
        errorString_ = "Wrong number of fields in digital channel information";
        return false;
    }
    bool convertedOk;
    channelNumber_ = lineContent[0].toInt(&convertedOk);
    if (!convertedOk) {
        errorString_ = "Digital channel index number must be an integer";
        return false;
    }
    name_ = lineContent[1];
    int normalStateInt;
    if (standardRevisionYear == 0) {
        normalStateInt = lineContent[2].toInt(&convertedOk);
    } else {
        phase_ = lineContent[2];
        ccbm_ = lineContent[3];
        normalStateInt = lineContent[4].toInt(&convertedOk);
    }
    if (!convertedOk) {
        errorString_ = "Digital channel normal state must be an integer";
        return false;
    }
    if (normalStateInt == 0) { normalState_ = false; }
    else if (normalStateInt == 1) { normalState_ = true; }
    else {
        errorString_ = "Digital channel normal state must be 0 or 1";
        return false;
    }
    qDebug() << channelNumber_ << name_ << phase_ << ccbm_ << normalState_;
    return true;
}

/*
 * ComtradeConfig
 *
 * */

ComtradeConfig::ComtradeConfig() :
    stationName_(QString()),
    deviceName_(QString()),
    standardRevisionYear_(0),
    analogChannelsNumber_(-1),
    digitalChannelsNumber_(-1),
//    analogChannels_(),
//    digitalChannels_(),
    lineFrequency_(0),
    samplingRatesNumber_(0),
//    samplingRates_(),
//    lastSampleNumbers_(),
    firstValueTime_(0),
    triggerPointTime_(0),
    dataFileType_(ComtradeDataFileType::ascii),
    timeStampFactor_(1),
    timeCode_(QString()),
    localCode_(QString()),
    timeQualityCode_(-1),
    leapSecondIndicator_(ComtradeLeapSecondIndicator::zero),
    errorString_("Error reading COMTRADE config:") {
}

bool ComtradeConfig::readFromByteArray(const QByteArray &headerFileContent) {
    QTextStream headerFileStream(headerFileContent);
    QString line;

    if (!readParametersBeforeChannels(headerFileStream)) {
//        errorString_ = "FAIL TO READ PARAMS BEFORE CHANNELS";
        return false;
    }
    if (!readChannelsParameters(headerFileStream)) { return false; }
    if (!readSamplingParameters(headerFileStream)) { return false; }

    line = headerFileStream.readLine();
    firstValueTime_ = convertToTimestamp(line);
    line = headerFileStream.readLine();
    triggerPointTime_ = convertToTimestamp(line);

    if (!readDataFileType(headerFileStream)) {
//        errorString_ = "Error reading data file type";
        return false;
    }
    if (!readTimeParameters(headerFileStream)) {
//        errorString_ = "Error reading time parameters";
        return false;
    }
    errorString_ = QString();
    return true;
}

bool ComtradeConfig::readParametersBeforeChannels(QTextStream &stream) {
    QString line;
    QStringList lineContent;
    bool convertedOk;

    line = stream.readLine();
    lineContent = line.split(',');
    if (lineContent.size() != 2 && lineContent.size() != 3) {
        return false;
    }
    stationName_ = lineContent.at(0);
    deviceName_ = lineContent.at(1);
    if (lineContent.size() == 3) {
        standardRevisionYear_ = lineContent.at(2).toInt(&convertedOk);
        if (!convertedOk) {
            errorString_ = errorString_ + "\nCOMTRADE standard revision year must be an integer";
            return false;
        }
    }

    line = stream.readLine();
    lineContent = line.split(',');
    if (lineContent.size() != 3) {
        return false;
    }
    if (!lineContent.at(1).endsWith('A') || !lineContent.at(2).endsWith('D')) {
        errorString_ = errorString_ + "\nNumber of analog channels must be followed by A; Number of analog channels must be followed by D";
        return false;
    }
    int channelsNumber = lineContent.at(0).toInt(&convertedOk);
    if (!convertedOk) {
        errorString_ = errorString_ + "\nTotal number of channels must be an integer";
        return false;
    }
    analogChannelsNumber_ = lineContent[1].remove(lineContent.at(1).size() - 1, 1).toInt(&convertedOk);
    if (!convertedOk) {
        errorString_ = errorString_ + "\nNumber of analog channels must be an integer";
        return false;
    }
    digitalChannelsNumber_ = lineContent[2].remove(lineContent.at(2).size() - 1, 1).toInt(&convertedOk);
    if (!convertedOk) {
        errorString_ = errorString_ + "\nNumber of digital channels must be an integer";
        return false;
    }
    if (channelsNumber != analogChannelsNumber_ + digitalChannelsNumber_) {
        errorString_ = errorString_ + "\nTotal number of channels must be equal to numbers of analog and digital channels combined";
        return false;
    }
    analogChannels_.reserve(analogChannelsNumber_);
    digitalChannels_.reserve(digitalChannelsNumber_);
    return true;
}

bool ComtradeConfig::readChannelsParameters(QTextStream &stream) {
    QString line;
    for (int i = 0; i < analogChannelsNumber_; i++) {
        line = stream.readLine();
        ComtradeAnalogChannel analogChannel = ComtradeAnalogChannel();
        if (!analogChannel.readFromString(line, standardRevisionYear_)) {
            errorString_ = errorString_ + "\nError reading parameters of analog channel " + QString::number(i + 1) + ":\n" + analogChannel.errorString();
            return false;
        }
        analogChannels_.append(analogChannel);
    }
    for (int i = 0; i < digitalChannelsNumber_; i++) {
        line = stream.readLine();
        ComtradeDigitalChannel digitalChannel = ComtradeDigitalChannel();
        if (!digitalChannel.readFromString(line, standardRevisionYear_)) {
            errorString_ = errorString_ + "\nError reading parameters of digital channel " + QString::number(i + 1) + ":\n" + digitalChannel.errorString();
            return false;
        }
        digitalChannels_.append(digitalChannel);
    }
    return true;
}

bool ComtradeConfig::readSamplingParameters(QTextStream &stream) {
    QString line;
    QStringList lineContent;
    bool convertedOk;

    line = stream.readLine();
    lineFrequency_ = line.toDouble(&convertedOk);
    if (!convertedOk) {
        errorString_ = errorString_ + "\nLine frequency must be a real number";
        return false;
    }

    line = stream.readLine();
    samplingRatesNumber_ = line.toInt(&convertedOk);
    if (!convertedOk) {
        errorString_ = errorString_ + "\nNumber of sampling rates must be an integer";
        return false;
    }
    samplingRates_.reserve(samplingRatesNumber_);
    lastSampleNumbers_.reserve(samplingRatesNumber_);

    for (int i = 0; i < samplingRatesNumber_; i++) {
        line = stream.readLine();
        lineContent = line.split(',');
        if (lineContent.size() != 2) {
            return false;
        }
        double samplingRate = lineContent.at(0).toDouble(&convertedOk);
        if (!convertedOk) {
            errorString_ = errorString_ + "\nSampling rate must be a real number";
            return false;
        }
        samplingRates_.append(samplingRate);
        qint64 lastSampleNumber = lineContent.at(1).toLongLong(&convertedOk);
        if (!convertedOk) {
            errorString_ = errorString_ + "\nLast sample number must be an integer";
            return false;
        }
        lastSampleNumbers_.append(lastSampleNumber);
    }
    return true;
}

quint64 ComtradeConfig::convertToTimestamp(const QString &dataLine) {
    std::istringstream lineStream(dataLine.mid(0, dataLine.lastIndexOf('.')).toStdString());
    std::tm tm1;
    lineStream >> std::get_time(&tm1, "%d/%m/%Y,%H:%M:%S");
//    std::strftime(line.toStdString(), "%d/%m/%Y,%H:%M:%S", &tm1)
    std::time_t secondsSinceEpoch = std::mktime(&tm1) - timezone;
    if (secondsSinceEpoch == -1) {
        errorString_ = errorString_ + "\nFail to read date/time in timestamp";
    }
    qDebug() << "std::mktime:" << secondsSinceEpoch;
    bool convertedOk;
    int nanoSeconds = dataLine.mid(dataLine.lastIndexOf('.') + 1, dataLine.size() - dataLine.lastIndexOf('.') - 1).toInt(&convertedOk);
    while (nanoSeconds != 0 && nanoSeconds < 100'000'000) {     // fraction of a second in the file can be from microseconds to nanoseconds
        nanoSeconds *= 10;
    }
    if (!convertedOk) {
        qDebug() << "Fail to read subsecond part of a timestamp";
    }
    qDebug() << "nanoSec" << nanoSeconds;
    quint64 result;
    result = secondsSinceEpoch * quint64(1e9) + nanoSeconds;
    qDebug() << "WITH nanoSec:" << result;

//    TODO: QDateTime works only up to milliseconds
//    QDateTime firstTime = base::Time::fromString(dataLine.mid(0, 23), "dd/MM/yyyy,hh:mm:ss.zzz";
//    if (!firstTime.isValid()) { isValid_ = false;return; }
//    result = base::Time::toDurationFromMSeconds(firstTime.toMSecsSinceEpoch());
    return result;
}

bool ComtradeConfig::readDataFileType(QTextStream &stream) {
    QString lineUpper = stream.readLine().toUpper();
    if (lineUpper == "BINARY") { dataFileType_ = ComtradeDataFileType::binary; }
    else if (lineUpper == "BINARY32") { dataFileType_ = ComtradeDataFileType::binary32; }
    else if (lineUpper == "FLOAT32") { dataFileType_ = ComtradeDataFileType::float32; }
    else if (lineUpper == "ASCII") { dataFileType_ = ComtradeDataFileType::ascii; }
    else {
        errorString_ = errorString_ + "\nUnrecognized data file type";
        return false;
    }
    return true;
}

bool ComtradeConfig::readTimeParameters(QTextStream &stream) {
    if (standardRevisionYear_ == 0) { return true; }
    QString line;
    QStringList lineContent;
    bool convertedOk;

    line = stream.readLine();
    timeStampFactor_ = line.toDouble(&convertedOk);
    if (!convertedOk) {
        errorString_ = errorString_ + "\nTime stamp multiplication factor must be a real number";
        return false;
    }

    if (standardRevisionYear_ < 2013) {
        return true;
    }
    line = stream.readLine();
    lineContent = line.split(',');
    if (lineContent.size() != 2) {
        return false;
    }
    timeCode_ = lineContent.at(0);
    localCode_ = lineContent.at(1);

    line = stream.readLine();
    lineContent = line.split(',');
    if (lineContent.size() != 2) {
        return false;
    }
    timeQualityCode_ = lineContent.at(0).toInt(&convertedOk, 16);
    if (!convertedOk) {
        errorString_ = errorString_ + "\nTime quality indicator code must be a hex integer";
        return false;
    }
    if (lineContent.at(1) == '0') { leapSecondIndicator_ = ComtradeLeapSecondIndicator::zero; }
    else if (lineContent.at(1) == '1') { leapSecondIndicator_ = ComtradeLeapSecondIndicator::added; }
    else if (lineContent.at(1) == '2') { leapSecondIndicator_ = ComtradeLeapSecondIndicator::subtracted; }
    else if (lineContent.at(1) == '3') { leapSecondIndicator_ = ComtradeLeapSecondIndicator::sourceIncapable; }
    else {
        errorString_ = errorString_ + "\nLeap second indicator code must be an integer between 0 and 3";
        return false;
    }
    return true;
}

QString ComtradeConfig::stationName() const {
    return stationName_;
}

QString ComtradeConfig::deviceName() const {
    return deviceName_;
}

int ComtradeConfig::standardRevisionYear() const {
    return standardRevisionYear_;
}

int ComtradeConfig::analogChannelsNumber() const {
    return analogChannelsNumber_;
}

int ComtradeConfig::digitalChannelsNumber() const {
    return digitalChannelsNumber_;
}

QList<ComtradeAnalogChannel> ComtradeConfig::analogChannels() const {
    return analogChannels_;
}

QList<ComtradeDigitalChannel> ComtradeConfig::digitalChannels() const {
    return digitalChannels_;
}

double ComtradeConfig::lineFrequency() const {
    return lineFrequency_;
}

int ComtradeConfig::samplingRatesNumber() const {
    return samplingRatesNumber_;
}

QList<double> ComtradeConfig::samplingRates() const {
    return samplingRates_;
}

QList<qint64> ComtradeConfig::lastSampleNumbers() const {
    return lastSampleNumbers_;
}

quint64 ComtradeConfig::firstValueTime() const {
    return firstValueTime_;
}

quint64 ComtradeConfig::triggerPointTime() const {
    return triggerPointTime_;
}

ComtradeDataFileType ComtradeConfig::dataFileType() const {
    return dataFileType_;
}

double ComtradeConfig::timeStampFactor() const {
    return timeStampFactor_;
}

QString ComtradeConfig::timeCode() const {
    return timeCode_;
}

QString ComtradeConfig::localCode() const {
    return localCode_;
}

int ComtradeConfig::timeQualityCode() const {
    return timeQualityCode_;
}

ComtradeLeapSecondIndicator ComtradeConfig::leapSecondIndicator() const {
    return leapSecondIndicator_;
}

QString ComtradeConfig::errorString() const {
    return errorString_;
}

ComtradeAnalogChannel ComtradeConfig::analogChannel(int channelIndex) const {
    if (channelIndex >= 0 && channelIndex < analogChannels_.size()) {
        return analogChannels_.at(channelIndex);
    }
    return ComtradeAnalogChannel();
}

ComtradeDigitalChannel ComtradeConfig::digitalChannel(int channelIndex) const {
    if (channelIndex >= 0 && channelIndex < analogChannels_.size()) {
        return digitalChannels_.at(channelIndex);
    }
    return ComtradeDigitalChannel();
}
