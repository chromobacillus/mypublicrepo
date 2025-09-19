#ifndef __9da6d45d2144a96453179cfb76de5d602db9d5ba
#define __9da6d45d2144a96453179cfb76de5d602db9d5ba

#include <chrono>
#include <iomanip>

#include <QtCore/QList>
#include <QDebug>
#include <QDataStream>
#include <QTextStream>

#include "src/phases.hpp"

enum class AnalogChannelScalingIdentifier {
    primary = 0,
    secondary = 1
};

/*
 * ComtradeChannel
 * Abstract class to store data used by analog and digital channels
 * */

class ComtradeChannel {
public:
    ComtradeChannel();

public:
    bool isValid() const;
    QString name() const;
    QString phase() const;
    QString ccbm() const;
    PhaseType phaseTypeFromPhase() const;
    PhaseType phaseTypeFromPhaseAndName() const;
    QString errorString() const;

protected:
    int channelNumber_;
    QString name_;
    QString phase_;
    QString ccbm_;
    QString errorString_;
};

/*
 * ComtradeAnalogChannel
 * Class for storing analog channel data.
 * Analog channel values are effectively floating point numbers
 * */

class ComtradeAnalogChannel : public ComtradeChannel {
public:
    ComtradeAnalogChannel();

    bool readFromString(const QString &dataLine, const int standardRevisionYear);

    QString units() const;
    double multiplier() const;
    double offsetAdder() const;
    double timeSkewMs() const;
    double min() const;
    double max() const;
    double primaryFactor() const;
    double secondaryFactor() const;
    AnalogChannelScalingIdentifier scalingIdentifier() const;

private:
    QString units_;             // Ampers, volts etc.
    double multiplier_;         // Actual value = raw value * multiplier + offsetAdder
    double offsetAdder_;        // Actual value = raw value * multiplier + offsetAdder
    double timeSkewMs_;
    double min_;                //
    double max_;                //
    double primaryFactor_;      // Used for converting between primary and secondary values (they differ if reporting device is a transformer)
    double secondaryFactor_;    // Used for converting between primary and secondary values (they differ if reporting device is a transformer)
    AnalogChannelScalingIdentifier scalingIdentifier_;
};

/*
 * ComtradeDigitalChannel
 * Class for storing digital channel data.
 * Digital channel values are effectively boolean (0 or 1)
 * */

class ComtradeDigitalChannel : public ComtradeChannel {
public:
    ComtradeDigitalChannel();

    bool readFromString(const QString &dataLine, const int standardRevisionYear);

private:
    bool normalState_;      // Normal (default) state of the channel
};

/*
 * ComtradeDataFileType
 *
 * */

enum class ComtradeDataFileType {
    ascii = 0,
    binary,
    binary32,
    float32
};

/*
 * ComtradeLeapSecondIndicator
 *
 * */

enum class ComtradeLeapSecondIndicator {
    zero = 0,
    added,
    subtracted,
    sourceIncapable
};

/*
 * ComtradeConfig
 * Information from COMTRADE config file
 * */

class ComtradeConfig {
public:
    ComtradeConfig();

    bool readFromByteArray(const QByteArray &headerFileContent);

    QString stationName() const;
    QString deviceName() const;
    int standardRevisionYear() const;
    int analogChannelsNumber() const;
    int digitalChannelsNumber() const;
    QList<ComtradeAnalogChannel> analogChannels() const;
    QList<ComtradeDigitalChannel> digitalChannels() const;
    double lineFrequency() const;
    int samplingRatesNumber() const;
    QList<double> samplingRates() const;
    QList<qint64> lastSampleNumbers() const;
    // base::timestamp firstValueTime() const;
    // base::timestamp triggerPointTime() const;
    quint64 firstValueTime() const;
    quint64 triggerPointTime() const;
    ComtradeDataFileType dataFileType() const;
    double timeStampFactor() const;
    QString timeCode() const;
    QString localCode() const;
    int timeQualityCode() const;
    ComtradeLeapSecondIndicator leapSecondIndicator() const;
    QString errorString() const;

    ComtradeAnalogChannel analogChannel(int channelIndex) const;
    ComtradeDigitalChannel digitalChannel(int channelIndex) const;

private:
    bool readParametersBeforeChannels(QTextStream &stream);
    bool readChannelsParameters(QTextStream &stream);
    quint64 convertToTimestamp(const QString &dataLine);
    bool readSamplingParameters(QTextStream &stream);
    bool readDataFileType(QTextStream &stream);
    bool readTimeParameters(QTextStream &stream);

private:
    QString stationName_;
    QString deviceName_;
    int standardRevisionYear_;
    int analogChannelsNumber_;
    int digitalChannelsNumber_;
    QList<ComtradeAnalogChannel> analogChannels_;
    QList<ComtradeDigitalChannel> digitalChannels_;
    double lineFrequency_;
    int samplingRatesNumber_;
    QList<double> samplingRates_;
    QList<qint64> lastSampleNumbers_;
    quint64 firstValueTime_;
    quint64 triggerPointTime_;
    ComtradeDataFileType dataFileType_;
    double timeStampFactor_;
    QString timeCode_;
    QString localCode_;
    int timeQualityCode_;
    ComtradeLeapSecondIndicator leapSecondIndicator_;
    QString errorString_;
};

#endif //__9da6d45d2144a96453179cfb76de5d602db9d5ba
