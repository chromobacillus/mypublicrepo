#include <QFileDialog>
#include <QMessageBox>

#include "multiplotcontroller.hpp"

void OpenFileAction::operator()(const QVariant &data) {
    if (!model_) { return; }

    QString fileName = QFileDialog::getOpenFileName();
    if (fileName != QString()) {
        model_->readRecord(fileName);
    }
}

void DisplayPrimaryValuesAction::operator()(const QVariant &data) {
    if (!model_) { return; }

    model_->setDisplayPrimaryOrSecondary(AnalogChannelScalingIdentifier::primary);

}

void DisplaySecondaryValuesAction::operator()(const QVariant &data) {
    if (!model_) { return; }

    model_->setDisplayPrimaryOrSecondary(AnalogChannelScalingIdentifier::secondary);
}

void ShowHelpAction::operator()(const QVariant &data) {
    if (!model_) { return; }

    QMessageBox::information(nullptr, "Help",
            "CTRL + MouseWheel: Vertical zoom\nSHIFT + MouseWheel: Horizontal zoom\nSHIFT + drag: Scroll horizontally");
}

/*
 * MultiplotContoller
 *
 * */

MultiplotController::MultiplotController(const QSharedPointer<MultiplotModel> &model, QObject *parent)
    : QObject{parent}, model_(model) {
    actions_[MultiplotActionKey::openFile] = action(new OpenFileAction(model));
    actions_[MultiplotActionKey::displayPrimaryValues] = action(new DisplayPrimaryValuesAction(model));
    actions_[MultiplotActionKey::displaySecondaryValues] = action(new DisplaySecondaryValuesAction(model));
    actions_[MultiplotActionKey::showHelp] = action(new ShowHelpAction(model));
}

void MultiplotController::requestContextMenu(const QPoint &pos) {
    if (!model_) { return; }

//    auto *const contextMenu = new MultiplotContextMenu(model_);
//    contextMenu->popup(QCursor::pos());
}

void MultiplotController::actionTrigger(int key, bool checked) {
    Q_UNUSED(checked)

    const QSharedPointer<MultiplotAction> &executor = actions_.value(static_cast<MultiplotActionKey>(key), nullptr);
    if (!executor) { return; }

    (*executor)(QVariant());
}

QSharedPointer<MultiplotAction> MultiplotController::action(MultiplotAction *action) {
    return QSharedPointer<MultiplotAction>(action);
}
