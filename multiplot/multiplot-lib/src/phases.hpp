#ifndef __370a7104612141989f420f3075867900eba90947
#define __370a7104612141989f420f3075867900eba90947

#include <QHash>
#include <QColor>

//namespace electro {

enum PhaseType {
    phaseNone = 0,
    phaseNeutral,
    phaseGround,
    phaseA,
    phaseB,
    phaseC
};

/*
 * PhaseColors
 * Color scheme of electric phases
 * */

class PhaseColors {
public:
    static QHash<PhaseType, QColor> colorsPUE;
};

//QHash<Phases, QColor> phaseColors;
//QHash<Phases, QColor> phaseColors = {};
//static QHash<Phases, QColor> phaseColors;

//}

#endif //__370a7104612141989f420f3075867900eba90947
