#ifndef __6738e6a13a08405271c4102d2d2636669d571296
#define __6738e6a13a08405271c4102d2d2636669d571296

#include "actionmanager.hpp"

namespace sr { namespace oscviewer {

/*
 * OscViewerActionKey
 * Named keys for all the actions
 * */

enum class OscViewerActionKey {
    openFile = 0,
    displayPrimaryValues,
    displaySecondaryValues
};

/*
 * OscViewerActionManager
 *
 * */

class OscViewerActionManager : public widgets::ActionManager {
Q_OBJECT
public:
    OscViewerActionManager();
    ~OscViewerActionManager() override = default;

    QAction *action(OscViewerActionKey key);
    void setActionEnabled(OscViewerActionKey key, bool enabled);
    bool actionEnabled(OscViewerActionKey key);
    void setActionData(OscViewerActionKey key, QVariant data);
    void setCheckable(OscViewerActionKey key, bool checkable);
    void addCustomAction(int newKey, const QString &text, const QKeySequence &keySequence = QKeySequence());

private:
    void addAction(OscViewerActionKey key, const QString &text, const QKeySequence &keySequence = QKeySequence());
    void setDecoration(OscViewerActionKey key, const QString &text, const QString &tooltip, const QIcon &icon, const QKeySequence &sequence);
};

}}

#endif //__6738e6a13a08405271c4102d2d2636669d571296
