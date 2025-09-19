#ifndef __f54d95c0ab9e12a268f3a80a286ed081d3962241
#define __f54d95c0ab9e12a268f3a80a286ed081d3962241

#include "src/comtradeconfig.hpp"

/*
 * ComtradeRecord
 * Information from whole COMTRADE record (includes config and data files)
 */

class ComtradeRecord {
public:
    ComtradeRecord();

    void readComtradeData(const ComtradeConfig &config, const QByteArray &dataFileContent);

    bool isValid() const;
    void setIsValid(bool newValue);
    QList<QList<double>> rawAnalogValues() const;
    QList<QList<double>> analogValues() const;
    QList<QList<double>> primaryAnalogValues() const;
    QList<QList<double>> secondaryAnalogValues() const;
    QList<QList<int>> digitalValues() const;
    QList<double> timesOfValuesInMs() const;
    ComtradeConfig config() const;
    void setConfig(const ComtradeConfig &config);
    QString errorString() const;

    int analogChannelsNumber() const;
    int digitalChannelsNumber() const;
    ComtradeAnalogChannel analogChannel(int channelIndex) const;
    ComtradeDigitalChannel digitalChannel(int channelIndex) const;

private:
    void readBinaryData(const QByteArray &dataFileContent);
    void readTextData(const QByteArray &dataFileContent);
    void allocateData(int samplesCount);
    void parseTextDataLine(const QString &dataLine);
    void formAnalogValuesFromRaw();
    void formPrimaryAndSecondaryValues();

private:
    bool isValid_;
    ComtradeConfig config_;
    QList<QList<double>> rawAnalogValues_;
    QList<QList<double>> analogValues_;
    QList<QList<double>> primaryAnalogValues_;
    QList<QList<double>> secondaryAnalogValues_;
    QList<QList<int>> digitalValues_;
    QString errorString_;
};

#endif //__f54d95c0ab9e12a268f3a80a286ed081d3962241
