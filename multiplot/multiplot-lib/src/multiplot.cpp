#include <QVBoxLayout>

#include "multiplot.hpp"

/*
 * Multiplot
 *
 * */

Multiplot::Multiplot(const QSharedPointer<MultiplotDataModel> &model, QWidget *parent)
    : QWidget{parent}, diagram_(new MultiplotWidget(QSharedPointer<MultiplotModel>(new MultiplotModel(model)))) {
    auto *const layout = new QVBoxLayout(this);
    layout->addWidget(diagram_);
    setWindowTitle(tr("Oscillogram Viewer"));
}
