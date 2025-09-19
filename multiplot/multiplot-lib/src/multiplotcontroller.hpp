#ifndef __e4b3b5808d06d2a5960ad73e4f60b3446c435a26
#define __e4b3b5808d06d2a5960ad73e4f60b3446c435a26

#include <QObject>

#include "multiplotmodel.hpp"

/*
 * MultiplotActionKey
 * Named keys for all the actions
 * */

enum class MultiplotActionKey {
    openFile = 0,
    displayPrimaryValues,
    displaySecondaryValues,
    showHelp
};

/*
 * MultiplotAction
 * Base class for all the user action handlers in the viewer
 * */

class MultiplotAction {
public:
    virtual ~MultiplotAction() = default;

    virtual void operator()(const QVariant &data) {}

protected:
    MultiplotAction() = default;
};

class OpenFileAction : public MultiplotAction {
public:
    explicit OpenFileAction(const QSharedPointer<MultiplotModel> &model) : MultiplotAction(), model_(model) {}
    ~OpenFileAction() override = default;

    void operator()(const QVariant &data) override;

private:
    QSharedPointer<MultiplotModel> model_;
};

class DisplayPrimaryValuesAction : public MultiplotAction {
public:
    explicit DisplayPrimaryValuesAction(const QSharedPointer<MultiplotModel> &model) : MultiplotAction(), model_(model) {}
    ~DisplayPrimaryValuesAction() override = default;

    void operator()(const QVariant &data) override;

private:
    QSharedPointer<MultiplotModel> model_;
};

class DisplaySecondaryValuesAction : public MultiplotAction {
public:
    explicit DisplaySecondaryValuesAction(const QSharedPointer<MultiplotModel> &model) : MultiplotAction(), model_(model) {}
    ~DisplaySecondaryValuesAction() override = default;

    void operator()(const QVariant &data) override;

private:
    QSharedPointer<MultiplotModel> model_;
};

class ShowHelpAction : public MultiplotAction {
public:
    explicit ShowHelpAction(const QSharedPointer<MultiplotModel> &model) : MultiplotAction(), model_(model) {}
    ~ShowHelpAction() override = default;

    void operator()(const QVariant &data) override;

private:
    QSharedPointer<MultiplotModel> model_;
};

/*
 * MultiplotContoller
 * Controller part of MultiPlot's Model-View-Controller structure
 * */

class MultiplotController : public QObject {
Q_OBJECT
public:
    MultiplotController(const QSharedPointer<MultiplotModel> &model, QObject *parent = nullptr);
    ~MultiplotController() override = default;

public slots:
    void requestContextMenu(const QPoint &pos);
    void actionTrigger(int key, bool checked);

private:
    QSharedPointer<MultiplotAction> action(MultiplotAction *action);

private:
    QSharedPointer<MultiplotModel> model_;

    QMap<MultiplotActionKey, QSharedPointer<MultiplotAction>> actions_;

};

#endif //__e4b3b5808d06d2a5960ad73e4f60b3446c435a26
