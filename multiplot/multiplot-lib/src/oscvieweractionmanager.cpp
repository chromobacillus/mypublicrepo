#include "oscvieweractionmanager.hpp"

namespace sr { namespace oscviewer {

/*
 * OscViewerActionManager
 *
 * */

OscViewerActionManager::OscViewerActionManager() : widgets::ActionManager() {
    QString actionText = tr("Open file");
    addAction(OscViewerActionKey::openFile, actionText);
    setDecoration(OscViewerActionKey::openFile, actionText, actionText, QPixmap(), QKeySequence());
    actionText = tr("Display primary values");
    addAction(OscViewerActionKey::displayPrimaryValues, actionText);
    setDecoration(OscViewerActionKey::displayPrimaryValues, actionText, actionText, QPixmap(), QKeySequence());
    actionText = tr("Display secondary values");
    addAction(OscViewerActionKey::displaySecondaryValues, actionText);
    setDecoration(OscViewerActionKey::displaySecondaryValues, actionText, actionText, QPixmap(), QKeySequence());
}

QAction *OscViewerActionManager::action(OscViewerActionKey key) {
    return ActionManager::action(static_cast<int>(key));
}

void OscViewerActionManager::setActionEnabled(OscViewerActionKey key, bool enabled) {
    ActionManager::setEnable(static_cast<int>(key), enabled);
}

bool OscViewerActionManager::actionEnabled(OscViewerActionKey key) {
    return ActionManager::isEnabled(static_cast<int>(key));
}

void OscViewerActionManager::setActionData(OscViewerActionKey key, QVariant data) {
    ActionManager::setData(static_cast<int>(key), data);
}

void OscViewerActionManager::setCheckable(OscViewerActionKey key, bool checkable) {
    ActionManager::setCheckable(static_cast<int>(key), checkable);
}

void OscViewerActionManager::addCustomAction(int newKey, const QString &text, const QKeySequence &keySequence) {
    addAction(static_cast<OscViewerActionKey>(newKey), text, keySequence);
    setDecoration(static_cast<OscViewerActionKey>(newKey), text, text, QPixmap(), QKeySequence());
}

void OscViewerActionManager::addAction(OscViewerActionKey key, const QString &text, const QKeySequence &keySequence) {
    ActionManager::createAction(static_cast<int>(key), text);
}

void OscViewerActionManager::setDecoration(OscViewerActionKey key, const QString &text, const QString &tooltip, const QIcon &icon,
        const QKeySequence &sequence) {
    setText(static_cast<int>(key), text);
    setTooltip(static_cast<int>(key), tooltip);
    setIcon(static_cast<int>(key), icon);
    setShortcut(static_cast<int>(key), sequence);
}

}}
