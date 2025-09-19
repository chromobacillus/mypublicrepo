#ifndef __c430dae2aea5d7356b791bece5bf60d5cd119a49
#define __c430dae2aea5d7356b791bece5bf60d5cd119a49

#include "comtradeconfig.hpp"

/*
 * ComtradeRecord
 * Includes information from config and data files
 *
 * Data file type   Template argument
 * binary           qint16
 * binary32         qint32
 * float32          float
 * */

template <typename DataType> class ComtradeRecordTemplate {
public:
    ComtradeRecordTemplate();
    ComtradeRecordTemplate(const ComtradeConfig &header, const QByteArray &dataFileContent) {
        config_ = header;
        qDebug() << "Header OK";
        readBinaryData(dataFileContent);

        // TODO: remove speed test later
        double totalsum = 0;
        std::chrono::time_point<std::chrono::high_resolution_clock> m_beg = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < config_.analogChannelsNumber(); i++) {
            double sum = 0;
            for (int j = 0; j < analogValues_.at(i).size(); j++) {
                sum += analogValues_.at(i).at(j);
            }
            totalsum += sum;
            qDebug() << "ARRAY" << i << ":" << analogValues_.at(i).size() << "SUM" << sum;
        }
        auto elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - m_beg).count();
        qDebug() << "ELAPSED:" << elapsed;
        qDebug() << "totalsum:" << totalsum;
    }

    QList<QList<DataType>> analogValues() const {
        return analogValues_;
    }

    QList<QList<bool>> discreteValues() const {
        return discreteValues_;
    }

    ComtradeConfig config() const {
        return config_;
    }

private:
    void readBinaryData(const QByteArray &dataFileContent) {
        qDebug() << "readBinaryData()" << dataFileContent.size();
        QDataStream dataFileStream(dataFileContent);
        dataFileStream.setByteOrder(QDataStream::LittleEndian);
        dataFileStream.setFloatingPointPrecision(QDataStream::SinglePrecision);
        int bytesPerSample = (sizeof(DataType) * config_.analogChannelsNumber() + 2 * (config_.discreteChannelsNumber() / 16 + 1) + 8);
        int samplesCount = dataFileContent.size() / bytesPerSample;

        qDebug() << "CHANNELS:" << config_.analogChannelsNumber() << config_.discreteChannelsNumber() << config_.discreteChannelsNumber() / 16 + 1;
        qDebug() << "bytesPerSample:" << bytesPerSample;

        allocateData(samplesCount);
        for (int n = 0; n < samplesCount; n++) {
            qint32 recordNumber;
            qint32 timestamp;
            dataFileStream >> recordNumber;
//            qDebug() << recordNumber;
            dataFileStream >> timestamp;
            DataType analogValue;
            for (int i = 0; i < config_.analogChannelsNumber(); i++) {
                dataFileStream >> analogValue;
                analogValues_[i].append(analogValue);
            }

            if (config_.discreteChannelsNumber() > 0) {
                quint16 discreteData;
                quint16 one = 1;
                for (int i = 0; i < config_.discreteChannelsNumber() / 16 + 1; i++) {
                    dataFileStream >> discreteData;
                    for (int j = 0; j < 15; j++) {
                        if (16 * i + j >= config_.discreteChannelsNumber()) { break; }
                        discreteValues_[16 * i + j].append((discreteData & (one << j)) != 0);
                    }
                }
            }
        }
    }

    void allocateData(int samplesCount) {
        for (int i = 0; i < config_.analogChannelsNumber(); i++) {
            QList<DataType> newList;
            newList.reserve(samplesCount);
            analogValues_.append(newList);
        }
        for (int i = 0; i < config_.discreteChannelsNumber(); i++) {
            QList<bool> newList;
            newList.reserve(samplesCount);
            discreteValues_.append(newList);
        }
    }

private:
    ComtradeConfig config_;
    QList<QList<DataType>> analogValues_;
    QList<QList<bool>> discreteValues_;
};

#endif //__c430dae2aea5d7356b791bece5bf60d5cd119a49
