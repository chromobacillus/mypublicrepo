#ifndef __d25ff248ddd3ab1eb7e116f9dad8ab16f40a0544
#define __d25ff248ddd3ab1eb7e116f9dad8ab16f40a0544

#include <QWidget>
#include <QVBoxLayout>

#include "multiplotmodel.hpp"
#include "multiplotcontrols.hpp"
#include "multiplotcontroller.hpp"
#include "multiplotpainter.hpp"

/*
 * MultiplotWidget
 * Inner part of MultiPlot
 * (the widget itself excluding external interface and data model)
 * Includes widgets with plots and control elements as members.
 * */

class MultiplotWidget : public QWidget {
Q_OBJECT
public:
    explicit MultiplotWidget(const QSharedPointer<MultiplotModel> &model, QWidget *parent = nullptr);

private slots:
    void updateRecord();

private:
    void setModel();

private:
    QSharedPointer<MultiplotModel> model_;
    QSharedPointer<MultiplotController> controller_;
    MultiplotControls *controls_;
    MultiplotPainter *painter_;

    QVBoxLayout *layout_;
};

#endif //__d25ff248ddd3ab1eb7e116f9dad8ab16f40a0544
