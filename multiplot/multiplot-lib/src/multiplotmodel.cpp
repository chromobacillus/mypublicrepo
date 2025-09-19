#include "multiplotmodel.hpp"



/*
 * MultiplotModel
 *
 * */

MultiplotModel::MultiplotModel(const QSharedPointer<MultiplotDataModel> &model)
        : dataModel_(model), displayPrimaryOrSecondary_(AnalogChannelScalingIdentifier::primary) {
}

QSharedPointer<ComtradeRecord> MultiplotModel::record() {
    return record_;
}

void MultiplotModel::readRecord(const QString &filePath) {
    if (dataModel_) {
        record_ = QSharedPointer<ComtradeRecord>(new ComtradeRecord(dataModel_->record(filePath)));
        Q_EMIT recordUpdated();
    }
}

AnalogChannelScalingIdentifier MultiplotModel::displayPrimaryOrSecondary() const {
    return displayPrimaryOrSecondary_;
}

void MultiplotModel::setDisplayPrimaryOrSecondary(const AnalogChannelScalingIdentifier newValue) {
    displayPrimaryOrSecondary_ = newValue;
    Q_EMIT displayPrimaryOrSecondaryChanged();
}


