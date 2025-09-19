#include "phases.hpp"

/*
 * PhaseColors
 *
 * */

QHash<PhaseType, QColor> PhaseColors::colorsPUE = {
        {PhaseType::phaseNone, Qt::black},
        {PhaseType::phaseNeutral, Qt::blue},
        {PhaseType::phaseA, "#ffc800"},
        {PhaseType::phaseB, Qt::green},
        {PhaseType::phaseC, Qt::red}
        };
