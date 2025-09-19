#ifndef __c274664e2693b0ba69f1de5d966e47b59adb5520
#define __c274664e2693b0ba69f1de5d966e47b59adb5520

#include <QWidget>

#include <multiplotdatamodel.hpp>
#include <src/multiplotmodel.hpp>
#include <src/multiplotwidget.hpp>

/*
 * Multiplot
 * Widget for viewing plots from Comtrade files
 * */

class Multiplot : public QWidget {
    Q_OBJECT
public:
    explicit Multiplot(const QSharedPointer<MultiplotDataModel> &model, QWidget *parent = nullptr);

signals:

private:
    MultiplotWidget *diagram_;
};

#endif //__c274664e2693b0ba69f1de5d966e47b59adb5520
