#include <QMessageBox>
#include <QVBoxLayout>

#include "multiplotwidget.hpp"
#include "comtraderecord.hpp"
#include "multiplotcontroller.hpp"

/*
 * MultiplotWidget
 *
 * */

MultiplotWidget::MultiplotWidget(const QSharedPointer<MultiplotModel> &model, QWidget *parent)
    : QWidget{parent}, model_(model), controller_(QSharedPointer<MultiplotController>(new MultiplotController(model))),
    controls_(new MultiplotControls(controller_, model, this)), painter_(new MultiplotPainter(model, nullptr, this)) {
    layout_ = new QVBoxLayout(this);
    layout_->addWidget(controls_);
    layout_->addWidget(painter_);
    setModel();
}

void MultiplotWidget::setModel() {
    connect(model_.data(), &MultiplotModel::recordUpdated, this, &MultiplotWidget::updateRecord);
}

void MultiplotWidget::updateRecord() {
    if (model_->record()->isValid()) {
        layout_->removeWidget(painter_);
        delete painter_;
        painter_ = new MultiplotPainter(model_, model_->record(), this);
        layout_->addWidget(painter_);
    } else {
        QMessageBox::warning(nullptr, tr("Error reading COMTRADE file"), model_->record()->errorString() + model_->record()->config().errorString());
    }
}
