#ifndef __702be1ab449639c757c1d96cd9d86e29e504085d
#define __702be1ab449639c757c1d96cd9d86e29e504085d

#include <QObject>

#include "multiplotdatamodel.hpp"



/*
 * MultiplotModel
 * Model part of MultiPlot's Model-View-Controller structure
 * */

class MultiplotModel : public QObject {
Q_OBJECT
public:
    explicit MultiplotModel(const QSharedPointer<MultiplotDataModel> &model);

    QSharedPointer<ComtradeRecord> record();
    void readRecord(const QString &filePath);

    AnalogChannelScalingIdentifier displayPrimaryOrSecondary() const;
    void setDisplayPrimaryOrSecondary(const AnalogChannelScalingIdentifier newValue);

signals:
    void recordUpdated();
    void displayPrimaryOrSecondaryChanged();

private:
    QSharedPointer<MultiplotDataModel> dataModel_ = nullptr;
    QSharedPointer<ComtradeRecord> record_;

    AnalogChannelScalingIdentifier displayPrimaryOrSecondary_;
};



#endif //__702be1ab449639c757c1d96cd9d86e29e504085d
