#ifndef __4e958e318e73cf3b221c57228f3ee52a9b262bcc
#define __4e958e318e73cf3b221c57228f3ee52a9b262bcc

#include <QWidget>

#include "multiplotmodel.hpp"
#include "multiplotcontroller.hpp"

/*
 * MultiplotControls
 * Widget that contains control elements (buttons, menus etc.)
 * */

class MultiplotControls : public QWidget {
Q_OBJECT
public:
    explicit MultiplotControls(const QSharedPointer<MultiplotController> &controller, const QSharedPointer<MultiplotModel> &model, QWidget *parent = nullptr);
};

#endif //__4e958e318e73cf3b221c57228f3ee52a9b262bcc
