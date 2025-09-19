#include <QDebug>
#include <QEvent>
#include <QLabel>
#include <QScrollArea>
#include <QScrollBar>
#include <QTimer>
#include <QWheelEvent>
#include <QPen>
#include <QSplitter>
#include <QPainter>

#include <qwt_plot_canvas.h>
#include <qwt_plot_layout.h>
#include <qwt_plot_picker.h>
#include <qwt_plot_marker.h>
#include <qwt_picker_machine.h>
#include <qwt_date_scale_engine.h>
#include <qwt_date_scale_draw.h>
#include <qwt_date.h>
#include <qwt_scale_widget.h>

#include "multiplotpainter.hpp"

/*
 * ReplottingPanner
 * QwtPlotPanner subclass that replots when moving the mouse
 * */

ReplottingPanner::ReplottingPanner(QWidget *canvas) : QwtPlotPanner(canvas), isPanning_(false) {
}

void ReplottingPanner::widgetMousePressEvent(QMouseEvent *mouseEvent) {
    if (!mouseEvent) { return; }

    Qt::MouseButton button;
    Qt::KeyboardModifiers modifiers;
    getMouseButton(button, modifiers);

    if ((mouseEvent->button() != button ) || (mouseEvent->modifiers() != modifiers)) { return; }
    initialMousePos_ = mouseEvent->pos();
    isPanning_ = true;
}

void ReplottingPanner::widgetMouseReleaseEvent(QMouseEvent *mouseEvent) {
    if (!mouseEvent) { return; }

    QPoint pos = mouseEvent->pos();
    if (!isOrientationEnabled(Qt::Horizontal)) { pos.setX(initialMousePos_.x()); }
    if (!isOrientationEnabled(Qt::Vertical)) { pos.setY(initialMousePos_.y()); }

    // Do NOT emit panned signal here because it is emitted automatically
    isPanning_ = false;
}

void ReplottingPanner::widgetMouseMoveEvent(QMouseEvent *mouseEvent) {
    if (!mouseEvent) { return; }

    if (!isPanning_) { return; }
    QPoint pos = mouseEvent->pos();
    if (!isOrientationEnabled(Qt::Horizontal)) { pos.setX(initialMousePos_.x()); }
    if (!isOrientationEnabled(Qt::Vertical)) { pos.setY(initialMousePos_.y()); }

    if (pos != initialMousePos_) {
        Q_EMIT panned(pos.x() - initialMousePos_.x(), pos.y() - initialMousePos_.y());
        initialMousePos_ = pos;
    }
}

/*
 * MultiGraphMagnifier
 * QwtPlotMagnifier sublass that emits QwtPlotMagnifier::magnified signal with every wheel event
 * */

MultiplotMagnifier::MultiplotMagnifier(QWidget *canvas) : QwtPlotMagnifier{canvas} {
}

void MultiplotMagnifier::widgetWheelEvent(QWheelEvent *wheelEvent) {
    QwtPlotMagnifier::widgetWheelEvent(wheelEvent);
    Q_EMIT magnified();
}

/*
 * PlotSplitterHandle
 * QSplitterHandle subclass to make splitter handles visible
 * */

PlotSplitterHandle::PlotSplitterHandle(Qt::Orientation orientation, QSplitter *parent) : QSplitterHandle(orientation, parent) {
}

void PlotSplitterHandle::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    painter.fillRect(event->rect(), QBrush(Qt::lightGray));
}

/*
 * PlotSplitter
 * QSplitter subclass to make splitter handles visible
 * */

PlotSplitter::PlotSplitter(Qt::Orientation orientation, QWidget *parent) : QSplitter(orientation, parent) {
}

QSplitterHandle *PlotSplitter::createHandle() {
    return new PlotSplitterHandle(orientation(), this);
}

/*
 * MultiplotPainter
 * Part of the widget that contains plots for analog and digital channels
 * */

MultiplotPainter::MultiplotPainter(const QSharedPointer<MultiplotModel> &model, const QSharedPointer<ComtradeRecord> &record, QWidget *parent)
        : QWidget{parent}, model_(model), comtradeRecord_(record), lastUsedPicker_(nullptr) {
    setDiagramModel(model);
    outerLayout_ = new QVBoxLayout(this);

    analogSplitter_ = new PlotSplitter(Qt::Vertical, this);
    analogSplitter_->setChildrenCollapsible(false);
    analogSplitter_->setMinimumHeight(200);
    analogSplitter_->setMinimumWidth(200);
    analogScroll_ = new QScrollArea();
    analogScroll_->setWidget(analogSplitter_);

    analogScroll_->setWidgetResizable(true);
    analogScroll_->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    analogScroll_->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    analogScroll_->verticalScrollBar()->installEventFilter(this);

    digitalWidget_ = new QWidget(this);
    digitalLayout_ = new QGridLayout(digitalWidget_);
    digitalLayout_->setVerticalSpacing(0);
    digitalLayout_->setHorizontalSpacing(0);
    digitalScroll_ = new QScrollArea();
    digitalScroll_->setWidget(digitalWidget_);
    digitalScroll_->setWidgetResizable(true);
    digitalScroll_->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    digitalScroll_->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    digitalScroll_->verticalScrollBar()->installEventFilter(this);

    QSplitter *analogDigitalSplitter = new QSplitter(Qt::Vertical, this);
    analogDigitalSplitter->addWidget(analogScroll_);
    analogDigitalSplitter->addWidget(digitalScroll_);
    analogDigitalSplitter->setSizes(QList<int>({INT_MAX, INT_MAX}));

    analogVerticalMarker_ = new QWidget(analogScroll_);
    analogVerticalMarker_->resize(1, 20000);
    analogVerticalMarker_->setStyleSheet("border-style: outset; border-width: 1px; border-color: red;");
    analogVerticalMarker_->setVisible(false);
    digitalVerticalMarker_ = new QWidget(digitalScroll_);
    digitalVerticalMarker_->resize(1, 20000);
    digitalVerticalMarker_->setStyleSheet("border-style: outset; border-width: 1px; border-color: red;");
    digitalVerticalMarker_->setVisible(false);

    outerLayout_->addWidget(analogDigitalSplitter);
    if (record) {
        initPlots();
    }
}

MultiplotPainter::~MultiplotPainter() {
}

void MultiplotPainter::initPlots() {
    comtradeRecord_->timesOfValuesInMs();
    analogSplitter_->setMinimumHeight(qMax(600, comtradeRecord_->analogValues().size() * 200));
    for (int n = 0; n < comtradeRecord_->analogValues().size(); n++) {
        const ComtradeAnalogChannel &channel = comtradeRecord_->analogChannel(n);

        QWidget *rowWidget = new QWidget(analogSplitter_);
        QHBoxLayout *rowLayout = new QHBoxLayout(rowWidget);
        rowLayout->setSpacing(0);
        QLabel *plotLabel = new QLabel(channel.name() + "\r\n" + channel.units() + " ", rowWidget);
        plotLabel->setAlignment(Qt::AlignCenter);
        rowLayout->addWidget(plotLabel);
        plotLabel->setMinimumWidth(150);
        analogLabels_.append(plotLabel);

        QwtPlot *newPlot = new QwtPlot(rowWidget);
        setupEmptyPlot(newPlot, PlotType::analog);
        analogPlots_.append(newPlot);
        rowLayout->addWidget(newPlot);
        rowWidget->setContentsMargins(0, newPlot->axisScaleDraw(QwtPlot::xBottom)->label(0).textSize().height(), 0, 0);
        setupCurve(newPlot, n, PlotType::analog);
    }
    for (int n = 0; n < comtradeRecord_->digitalValues().size(); n++) {
        const ComtradeDigitalChannel &channel = comtradeRecord_->digitalChannel(n);
        QLabel *plotLabel = new QLabel(channel.name(), digitalWidget_);
        plotLabel->setAlignment(Qt::AlignCenter);
        digitalLayout_->addWidget(plotLabel, n, 0);
        digitalLabels_.append(plotLabel);

        QwtPlot *newPlot = new QwtPlot(digitalWidget_);
        setupEmptyPlot(newPlot, PlotType::digital);
        digitalPlots_.append(newPlot);
        digitalLayout_->addWidget(newPlot, n, 1);
        setupCurve(newPlot, n, PlotType::digital);
    }
    digitalLayout_->setColumnMinimumWidth(0, 150);
    setupTimeAxis();
    QTimer::singleShot(100, this, SLOT(panIt()));

}

void MultiplotPainter::panIt() {
    if (digitalScroll_ && digitalLayout_ && xAxisPlot_) {
        QScrollBar *verticalBar = digitalScroll_->verticalScrollBar();
        const QMargins &scrollMargins = digitalScroll_->contentsMargins();
        xAxisPlot_->setContentsMargins(0, 0, scrollMargins.right() + (verticalBar->isVisible() ? verticalBar->width() : 0), 0);
    }
    Q_EMIT panner0_->panned(1, 0);
}

void MultiplotPainter::setupEmptyPlot(QwtPlot *newPlot, PlotType plotType) {
    qobject_cast<QwtPlotCanvas *>(newPlot->canvas())->setFrameStyle(QFrame::NoFrame);
    QBrush backgroundBrush = newPlot->canvasBackground();
    backgroundBrush.setColor(Qt::white);
    newPlot->setCanvasBackground(backgroundBrush);
    newPlot->setAxisMaxMajor(QwtPlot::xBottom, 16);
    newPlot->setAxisLabelRotation(QwtPlot::yLeft, -90);
    newPlot->setAxisLabelAlignment(QwtPlot::yLeft, Qt::AlignVCenter);
    QwtDateScaleEngine *dateScale = new QwtDateScaleEngine();
    newPlot->setAxisScaleEngine(QwtPlot::xBottom, dateScale);
    QwtDateScaleDraw *dateDraw = new QwtDateScaleDraw(Qt::UTC);
    newPlot->setAxisScaleDraw(QwtPlot::xBottom, dateDraw);
    dateDraw->setDateFormat( QwtDate::Millisecond, "hh:mm:ss" );
    dateDraw->setDateFormat( QwtDate::Second, "hh:mm:ss" );
    dateDraw->setDateFormat( QwtDate::Minute, "hh:mm" );
    dateDraw->setDateFormat( QwtDate::Hour, "hh:mm" );
    dateDraw->setSpacing(0);
    dateDraw->enableComponent(QwtAbstractScaleDraw::Ticks, false);
    dateDraw->enableComponent(QwtAbstractScaleDraw::Backbone, false);

    QPalette palette = newPlot->axisWidget(QwtPlot::xBottom)->palette();
    palette.setColor(QPalette::Text, Qt::transparent);
    newPlot->axisWidget(QwtPlot::xBottom)->setPalette(palette);

    QwtPlotGrid *newGrid = new QwtPlotGrid();
    newGrid->attach(newPlot);

    MultiplotMagnifier * xMagnifier = new MultiplotMagnifier(newPlot->canvas());
    xMagnifier->setAxisEnabled(QwtPlot::xBottom, true);
    xMagnifier->setAxisEnabled(QwtPlot::yLeft, false);
    xMagnifier->setMouseButton(Qt::NoButton, Qt::NoModifier);
    xMagnifier->setWheelModifiers(Qt::ShiftModifier);
    xMagnifier->setZoomOutKey(Qt::Key_0, Qt::ControlModifier);
    xMagnifier->setZoomInKey(Qt::Key_9, Qt::ControlModifier);
    connect(xMagnifier, &MultiplotMagnifier::magnified, this, [=]() {
        QwtScaleDiv xDiv = newPlot->axisScaleDiv(QwtPlot::xBottom);
        setNewXRangeForAllPlots(xDiv.lowerBound(), xDiv.upperBound());
    });

    ReplottingPanner *panner = new ReplottingPanner(newPlot->canvas());
    panner->setMouseButton(Qt::LeftButton, Qt::ShiftModifier);
    connect(panner, &QwtPanner::panned, this, [=](int dx, int dy) {
        QwtScaleDiv xDiv = newPlot->axisScaleDiv(QwtPlot::xBottom);
        setNewXRangeForAllPlots(xDiv.lowerBound(), xDiv.upperBound());
    });
    if (!panner0_) {
        panner0_ = panner;
    }

    QwtPlotPicker *picker = new QwtPlotPicker(newPlot->canvas());
    picker->setTrackerMode(QwtPicker::AlwaysOff);
    picker->setStateMachine(new QwtPickerDragPointMachine());
    picker->setMousePattern(QwtPlotPicker::MouseSelect1, Qt::LeftButton);
    connect(picker, SIGNAL(selected(const QPointF &)), this, SLOT(pointSelected(const QPointF &)));
    connect(picker, SIGNAL(moved(const QPointF &)), this, SLOT(pointSelected(const QPointF &)));
    QwtPlotMarker *verticalMarker  = new QwtPlotMarker();
    verticalMarker->setLineStyle(QwtPlotMarker::VLine);
    verticalMarker->setLinePen(Qt::red);
    verticalMarker->setLabelAlignment(Qt::AlignRight | Qt::AlignTop);
    verticalMarker->setVisible(false);
    verticalMarker->attach(newPlot);

    if (plotType == PlotType::analog) {
        newPlot->setAxisVisible(QwtPlot::yLeft, false);     // This is key to X axis proper left alignment
//        newPlot->setAxisVisible(QwtPlot::xBottom, false);
        newPlot->setAxisMaxMajor(QwtPlot::yLeft, 3);
        newPlot->setMinimumWidth(100);
        newPlot->setMinimumHeight(100);

        QwtPlotMagnifier * yMagnifier = new QwtPlotMagnifier(newPlot->canvas());
        yMagnifier->setAxisEnabled(QwtPlot::xBottom, false);
        yMagnifier->setAxisEnabled(QwtPlot::yLeft, true);
        yMagnifier->setMouseButton(Qt::NoButton, Qt::NoModifier);
        yMagnifier->setWheelModifiers(Qt::ControlModifier);
        yMagnifier->setZoomOutKey(Qt::Key_Minus, Qt::ControlModifier);
        yMagnifier->setZoomInKey(Qt::Key_Equal, Qt::ControlModifier);
        analogVerticalMarkers_.append(verticalMarker);
    } else if (plotType == PlotType::digital) {
        newPlot->setAxisVisible(QwtPlot::yLeft, false);     // This is key to X axis proper left alignment
        newPlot->setAxisScale(QwtPlot::yLeft, -0.1, 1.1);
        newPlot->setAxisMaxMajor(QwtPlot::yLeft, 1);
        newPlot->setMinimumWidth(100);
        newPlot->setFixedHeight(40);

        panner->setAxisEnabled(QwtPlot::yLeft, false);
        panner->setOrientations(Qt::Horizontal);
        digitalVerticalMarkers_.append(verticalMarker);
    }
}

void MultiplotPainter::setupCurve(QwtPlot *newPlot, int curveIndex, PlotType plotType) {
    QwtPlotCurve *newCurve = new QwtPlotCurve();
    newCurve->attach(newPlot);
    QPen pen = newCurve->pen();
    if (plotType == PlotType::analog) {
        if (model_->displayPrimaryOrSecondary() == AnalogChannelScalingIdentifier::primary) {
            newCurve->setSamples(model_->record()->timesOfValuesInMs().toVector(), model_->record()->primaryAnalogValues().at(curveIndex).toVector());
        } else {
            newCurve->setSamples(model_->record()->timesOfValuesInMs().toVector(), model_->record()->secondaryAnalogValues().at(curveIndex).toVector());
        }

        PhaseType colorIndex = comtradeRecord_->analogChannel(curveIndex).phaseTypeFromPhaseAndName();
        pen.setColor(PhaseColors::colorsPUE.value(colorIndex));
    } else if (plotType == PlotType::digital) {
        QVector<double> yValues;
        yValues.reserve(comtradeRecord_->digitalValues().at(curveIndex).size());
        for (int intValue : comtradeRecord_->digitalValues().at(curveIndex)) { yValues.append(intValue); }
        newCurve->setSamples(comtradeRecord_->timesOfValuesInMs().toVector(), yValues);
        pen.setColor(Qt::blue);
    }
    pen.setWidth(2);
    newCurve->setPen(pen);
}

void MultiplotPainter::setupTimeAxis() {
    QWidget *rowWidget = new QWidget(this);
    rowWidget->setMaximumHeight(60);
    QHBoxLayout *rowLayout = new QHBoxLayout(rowWidget);
    rowLayout->setSpacing(0);
    timeLabel_ = new QLabel(QString(), rowWidget);
    timeLabel_->setAlignment(Qt::AlignCenter);
    timeLabel_->setMinimumWidth(150);
    rowLayout->addWidget(timeLabel_);

    xAxisPlot_ = new QwtPlot(this);
    xAxisPlot_->setMinimumHeight(50);
    xAxisPlot_->canvas()->setVisible(false);

    xAxisPlot_->setAxisVisible(QwtPlot::yLeft, false);      // This is key to X axis proper left alignment
    xAxisPlot_->setAxisLabelRotation(QwtPlot::yLeft, -90);
    xAxisPlot_->setAxisLabelAlignment(QwtPlot::yLeft, Qt::AlignVCenter);
    QPalette palette = xAxisPlot_->axisWidget(QwtPlot::yLeft)->palette();
    palette.setColor(QPalette::Text, Qt::transparent);
    palette.setColor(QPalette::QPalette::WindowText, Qt::transparent);
    xAxisPlot_->axisWidget(QwtPlot::yLeft)->setPalette(palette);

    xAxisPlot_->setAxisMaxMajor(QwtPlot::xBottom, 16);
    QwtDateScaleEngine *dateScale = new QwtDateScaleEngine();
    xAxisPlot_->setAxisScaleEngine(QwtPlot::xBottom, dateScale);
    QwtDateScaleDraw *dateDraw = new QwtDateScaleDraw(Qt::UTC);
    xAxisPlot_->setAxisScaleDraw(QwtPlot::xBottom, dateDraw);
    dateDraw->setDateFormat(QwtDate::Millisecond, "hh:mm:ss\r\nzzz");
    dateDraw->setDateFormat(QwtDate::Second, "hh:mm:ss\r\n");
    dateDraw->setDateFormat(QwtDate::Minute, "hh:mm\r\n");
    dateDraw->setDateFormat(QwtDate::Hour, "hh:mm\r\n");

    rowLayout->addWidget(xAxisPlot_);
    outerLayout_->addWidget(rowWidget);
}

void MultiplotPainter::setDiagramModel(const QSharedPointer<MultiplotModel> &model) {
    connect(model.data(), &MultiplotModel::displayPrimaryOrSecondaryChanged, this, &MultiplotPainter::updateDisplayPrimaryOrSecondary);
}

bool MultiplotPainter::eventFilter(QObject *object, QEvent *event) {
    if (event->type() == QEvent::Wheel) {
        QWheelEvent *wheelEvent = static_cast<QWheelEvent *>(event);
        if (wheelEvent->modifiers() == Qt::ControlModifier || wheelEvent->modifiers() == Qt::ShiftModifier) {
            return true;
        }
        return QObject::eventFilter(object, event);
    } else {
        return QObject::eventFilter(object, event);
    }
}

void MultiplotPainter::paintEvent(QPaintEvent *event) {
    adjustVerticalMarkers();
    // adjustplotSizes();  // BUG ON WINDOWS
    // adjustTimeAxis();
}

void MultiplotPainter::resizeEvent(QResizeEvent *event) {
    // adjustVerticalMarkers();
    // adjustplotSizes();  // BUG ON WINDOWS
    // adjustTimeAxis();
}

void MultiplotPainter::showEvent(QShowEvent *event) {
    // adjustVerticalMarkers();
    // adjustplotSizes();  // BUG ON WINDOWS
    // adjustTimeAxis();
}

void MultiplotPainter::adjustVerticalMarkers() {
    if (analogVerticalMarkers_.at(0) && analogPlots_.at(0)) {
        QPoint globalPoint = analogPlots_.at(0)->canvas()->mapToGlobal(
                QPoint(analogPlots_.at(0)->transform(QwtPlot::xBottom, analogVerticalMarkers_.at(0)->xValue()), 0));
        analogVerticalMarker_->move(analogScroll_->mapFromGlobal(globalPoint).x(), 0);
        analogVerticalMarker_->setVisible(true);
        digitalVerticalMarker_->move(digitalScroll_->mapFromGlobal(globalPoint).x(), 0);
        digitalVerticalMarker_->setVisible(true);
    }
}

void MultiplotPainter::adjustplotSizes() {
    int max = 0;
    for (auto analogLabel : analogLabels_) {
        if (analogLabel->width() > max) { max = analogLabel->width(); }
    }
    if (digitalLayout_->cellRect(0, 0).right() > max) { max = digitalLayout_->cellRect(0, 0).right(); }
    for (auto analogLabel : analogLabels_) {
        analogLabel->setMinimumWidth(max);
    }
    digitalLayout_->setColumnMinimumWidth(0, max);
    if (timeLabel_) {
        timeLabel_->setMinimumWidth(max);
    }

    if (digitalScroll_ && digitalLayout_ && xAxisPlot_) {
        QScrollBar *verticalBar = digitalScroll_->verticalScrollBar();
        const QMargins &scrollMargins = digitalScroll_->contentsMargins();
        xAxisPlot_->setContentsMargins(0, 0, scrollMargins.right() + (verticalBar->isVisible() ? verticalBar->width() : 0), 0);
    }

    if (analogScroll_ && xAxisPlot_) {
        QScrollBar *verticalBar = analogScroll_->verticalScrollBar();
        const QMargins &scrollMargins = analogScroll_->contentsMargins();
        qDebug() << verticalBar->width();
        xAxisPlot_->setContentsMargins(0, 0, scrollMargins.right() + (verticalBar->isVisible() ? verticalBar->width() : 0), 0);
    }
}

void MultiplotPainter::adjustTimeAxis() {
    if (xAxisPlot_ && analogPlots_.at(0)) {
        QwtScaleDiv xDiv = analogPlots_.at(0)->axisScaleDiv(QwtPlot::xBottom);
        xAxisPlot_->setAxisScale(QwtPlot::xBottom, xDiv.lowerBound(), xDiv.upperBound());
        xAxisPlot_->replot();
    }
}

void MultiplotPainter::pointSelected(const QPointF &pos) {
    QwtPlot *senderPlot = qobject_cast<QwtPlotPicker *>(sender())->plot();
    lastUsedPicker_ = qobject_cast<QwtPlotPicker *>(sender());
    lastSelectedPoint_ = pos;

    for (int n = 0; n < analogPlots_.size(); n++) {
        QwtPlotMarker *marker = analogVerticalMarkers_.at(n);
        marker->setValue(pos.x(), pos.y());
    }
    for (int n = 0; n < digitalPlots_.size(); n++) {
        QwtPlotMarker *marker = digitalVerticalMarkers_.at(n);
        marker->setValue(pos.x(), pos.y());
    }

    QPoint globalPoint = senderPlot->canvas()->mapToGlobal(
            QPoint(senderPlot->transform(QwtPlot::xBottom, pos.x()), senderPlot->transform(QwtPlot::yLeft, pos.y())));
    analogVerticalMarker_->move(analogScroll_->mapFromGlobal(globalPoint).x(), 0);
    analogVerticalMarker_->setVisible(true);
    digitalVerticalMarker_->move(digitalScroll_->mapFromGlobal(globalPoint).x(), 0);
    digitalVerticalMarker_->setVisible(true);
    for (int n = 0; n < analogPlots_.size(); n++) {
        updateSignalValues(n, PlotType::analog, globalPoint);
    }
    for (int n = 0; n < digitalPlots_.size(); n++) {
        updateSignalValues(n, PlotType::digital, globalPoint);
    }
    timeLabel_->setText(QwtDate::toDateTime(pos.x(), Qt::UTC).toString("dd.MM.yyyy\r\nhh:mm:ss.zzz"));
}

void MultiplotPainter::updateSignalValues(int index, PlotType plotType, QPoint globalPoint) {
    if (plotType == PlotType::analog) {
        const ComtradeAnalogChannel &channel = comtradeRecord_->analogChannel(index);
        double selectedTime = analogPlots_.at(index)->invTransform(QwtPlot::xBottom, analogPlots_.at(index)->canvas()->mapFromGlobal(globalPoint).x());
        for (auto curve : analogPlots_.at(index)->itemList(QwtPlotItem::Rtti_PlotCurve)) {
            QwtSeriesData<QPointF> *curveData = static_cast<QwtPlotCurve *>(curve)->data();
            double signalValue = signalValueForClosestTime(curveData, selectedTime);
            analogLabels_.at(index)->setText(channel.name() + "\r\n" + channel.units() + "\r\n" + QString::number(signalValue));
        }
    } else if (plotType == PlotType::digital) {
        const ComtradeDigitalChannel &channel = comtradeRecord_->digitalChannel(index);
        double selectedTime = digitalPlots_.at(index)->invTransform(QwtPlot::xBottom, digitalPlots_.at(index)->canvas()->mapFromGlobal(globalPoint).x());
        for (auto curve : digitalPlots_.at(index)->itemList(QwtPlotItem::Rtti_PlotCurve)) {
            QwtSeriesData<QPointF> *curveData = static_cast<QwtPlotCurve *>(curve)->data();
            double signalValue = signalValueForClosestTime(curveData, selectedTime);
            digitalLabels_.at(index)->setText(channel.name() + "\r\n" + QString::number(signalValue));
        }
    }
}

double MultiplotPainter::signalValueForClosestTime(QwtSeriesData<QPointF> *curveData, double xValue) {
    int minIndex = 0;
    int maxIndex = curveData->size();
    int middleIndex = curveData->size() / 2;
    while (minIndex != middleIndex && maxIndex != middleIndex) {
        if (xValue >= curveData->sample(middleIndex).x()) {
            minIndex = middleIndex;
        } else {
            maxIndex = middleIndex;
        }
        middleIndex = (minIndex + maxIndex) / 2;
    }
    return curveData->sample(middleIndex).y();
}

void MultiplotPainter::setNewXRangeForAllPlots(double xLower, double xUpper) {
    for (int i = 0; i < analogPlots_.size(); i++) {
        analogPlots_.at(i)->setAxisScale(QwtPlot::xBottom, xLower, xUpper);
        analogPlots_.at(i)->replot();
    }
    for (int i = 0; i < digitalPlots_.size(); i++) {
        digitalPlots_.at(i)->setAxisScale(QwtPlot::xBottom, xLower, xUpper);
        digitalPlots_.at(i)->replot();
    }
    xAxisPlot_->setAxisScale(QwtPlot::xBottom, xLower, xUpper);
    xAxisPlot_->replot();
}

void MultiplotPainter::updateDisplayPrimaryOrSecondary() {
    for (int n = 0; n < analogPlots_.size(); n++) {
        for (auto curve : analogPlots_.at(n)->itemList(QwtPlotItem::Rtti_PlotCurve)) {
            if (model_->displayPrimaryOrSecondary() == AnalogChannelScalingIdentifier::primary) {
                static_cast<QwtPlotCurve *>(curve)->setSamples(model_->record()->timesOfValuesInMs().toVector(),
                        model_->record()->primaryAnalogValues().at(n).toVector());
            } else {
                static_cast<QwtPlotCurve *>(curve)->setSamples(model_->record()->timesOfValuesInMs().toVector(),
                        model_->record()->secondaryAnalogValues().at(n).toVector());
            }
        }
        analogPlots_[n]->replot();
    }
    // Recalculate values under selection
    if (lastUsedPicker_) {
        Q_EMIT lastUsedPicker_->selected(lastSelectedPoint_);
    }
}
