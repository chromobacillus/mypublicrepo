#ifndef __cb43d6e9c1a52fcf398d1b76eebb7d1ea9cd4d1c
#define __cb43d6e9c1a52fcf398d1b76eebb7d1ea9cd4d1c

#include <QObject>

#include "comtraderecord.hpp"

/*
 * MultiplotDataModel
 * Interface class for data model for external use. Subclass it and use to provide data to viewer.
 * */

class MultiplotDataModel : public QObject
{
    Q_OBJECT
public:
    ~MultiplotDataModel() override = default;

    virtual ComtradeRecord record(const QString &filePath) = 0;

signals:

protected:
    MultiplotDataModel() = default;
};

#endif // __cb43d6e9c1a52fcf398d1b76eebb7d1ea9cd4d1c
