#include <QDebug>
#include <QLabel>
#include <QPushButton>
#include <QSlider>
#include <QHBoxLayout>

#include "multiplotcontrols.hpp"

/*
 * MultiplotControls
 *
 * */

MultiplotControls::MultiplotControls(const QSharedPointer<MultiplotController> &controller, const QSharedPointer<MultiplotModel> &model, QWidget *parent)
    : QWidget{parent} {
    auto *const layout = new QHBoxLayout(this);

    QPushButton *openButton = new QPushButton(tr("Open file"), this);
    openButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    layout->addWidget(openButton);
    connect(openButton, &QPushButton::clicked, this, [=]() {
        controller->actionTrigger(static_cast<int>(MultiplotActionKey::openFile), true);
    });

    QLabel *primaryLabel = new QLabel(tr("Primary"));
    primaryLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    layout->addWidget(primaryLabel);

    QSlider *primSecSlider = new QSlider();
    primSecSlider->setOrientation(Qt::Horizontal);
    primSecSlider->setMinimum(0);
    primSecSlider->setMaximum(1);
    primSecSlider->setTickPosition(QSlider::NoTicks);
    primSecSlider->setFixedWidth(30);
    connect(primSecSlider, &QSlider::valueChanged, this, [=]() {
        if (primSecSlider->value() == 0) {
            controller->actionTrigger(static_cast<int>(MultiplotActionKey::displayPrimaryValues), true);
        } else {
            controller->actionTrigger(static_cast<int>(MultiplotActionKey::displaySecondaryValues), true);
        }
    });
    layout->addWidget(primSecSlider);

    QLabel *secondaryLabel = new QLabel(tr("Secondary"));
    secondaryLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    layout->addWidget(secondaryLabel);

    QPushButton *helpButton = new QPushButton(tr("Help"), this);
    helpButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    connect(helpButton, &QPushButton::clicked, this, [=]() {
        controller->actionTrigger(static_cast<int>(MultiplotActionKey::showHelp), true);
    });
    layout->addWidget(helpButton);

    QWidget *filler = new QWidget();
    layout->addWidget(filler);
}
