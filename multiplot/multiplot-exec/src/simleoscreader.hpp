#ifndef __a03ff93d403e6eafc980de9e9f537820e2a54ca6
#define __a03ff93d403e6eafc980de9e9f537820e2a54ca6

#include "comtraderecord.hpp"

#include <QObject>

class SimleOscReader : public QObject
{
    Q_OBJECT
public:
    explicit SimleOscReader(QObject *parent = nullptr);
    void readComtradeRecord(const QString &address);
    ComtradeRecord comtradeRecord() const;

private:
    ComtradeRecord comtradeRecord_;
};

#endif // __a03ff93d403e6eafc980de9e9f537820e2a54ca6
