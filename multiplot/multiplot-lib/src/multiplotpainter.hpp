#ifndef __db7e4801188eae66f852586df7ac58a1db2e5a3d
#define __db7e4801188eae66f852586df7ac58a1db2e5a3d

#include <QLabel>
#include <QScrollArea>
#include <QSplitterHandle>

#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_grid.h>
#include <qwt_plot_magnifier.h>
#include <qwt_plot_marker.h>
#include <qwt_plot_panner.h>
#include <qwt_plot_picker.h>
#include <qwt_plot_rescaler.h>
#include <qwt_scale_engine.h>

#include <QGridLayout>
#include <QVBoxLayout>

#include "comtraderecord.hpp"
#include "multiplotmodel.hpp"

/*
 * ReplottingPanner
 * Enabled dragging of the plot by mouse
 * Works only with left button. Other buttons are ignored.
 * */

class ReplottingPanner : public QwtPlotPanner {
Q_OBJECT
public:
    explicit ReplottingPanner(QWidget *canvas);
    ~ReplottingPanner() override = default;

protected:
    void widgetMousePressEvent(QMouseEvent *mouseEvent) override;
    void widgetMouseReleaseEvent(QMouseEvent *mouseEvent) override;
    void widgetMouseMoveEvent(QMouseEvent *mouseEvent) override;

private:
    bool isPanning_;
    QPoint initialMousePos_;
};

/*
 * MultiplotMagnifier
 * Emits magnified signal when magnified using mousewheel
 * */

class MultiplotMagnifier : public QwtPlotMagnifier {
Q_OBJECT
public:
    explicit MultiplotMagnifier(QWidget *canvas);

signals:
    void magnified();

protected:
    void widgetWheelEvent(QWheelEvent *wheelEvent) override;
};

/*
 * PlotSplitterHandle
 *
 * */

class PlotSplitterHandle : public QSplitterHandle {
Q_OBJECT
public:
    explicit PlotSplitterHandle(Qt::Orientation orientation, QSplitter *parent = nullptr);

protected:
    void paintEvent(QPaintEvent *event) override;
};

/*
 * PlotSplitter
 *
 * */

class PlotSplitter: public QSplitter {
Q_OBJECT
public:
    explicit PlotSplitter(Qt::Orientation orientation, QWidget *parent = nullptr);

protected:
    QSplitterHandle *createHandle() override;
};

/*
 * MultiGraphPainter
 * widget containing multiple QwtPlots with synchronized x axes
 * View part of MultiPlot's Model-View-Controller structure
 * */

class MultiplotPainter : public QWidget {
Q_OBJECT
public:
    enum class PlotType {
        analog = 0,
        digital
    };

public:
    explicit MultiplotPainter(const QSharedPointer<MultiplotModel> &model, const QSharedPointer<ComtradeRecord> &record, QWidget *parent = nullptr);
    ~MultiplotPainter() override;

    void initPlots();

signals:

protected:
    bool eventFilter(QObject *object, QEvent *event) override;
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    void showEvent(QShowEvent *event) override;

private slots:
    void pointSelected(const QPointF &pos);
    void updateDisplayPrimaryOrSecondary();
    void panIt();

private:
    void setDiagramModel(const QSharedPointer<MultiplotModel> &model);
    void setupEmptyPlot(QwtPlot *plot, PlotType plotType);
    void setupCurve(QwtPlot *plot, int curveIndex, PlotType plotType);
    void setupTimeAxis();
    void adjustVerticalMarkers();
    void adjustplotSizes();
    void adjustTimeAxis();
    void setNewXRangeForAllPlots(double xLower, double xUpper);
    void updateSignalValues(int index, PlotType plotType, QPoint globalPoint);
    double signalValueForClosestTime(QwtSeriesData<QPointF> *curveData, double xValue);

private:
    QSharedPointer<MultiplotModel> model_;
    QSharedPointer<ComtradeRecord> comtradeRecord_;
    QList<QwtPlot *> analogPlots_;
    QList<QwtPlot *> digitalPlots_;

    QLabel *timeLabel_ = nullptr;
    QwtPlot *xAxisPlot_ = nullptr;

    QVBoxLayout *outerLayout_;
    QGridLayout *digitalLayout_ = nullptr;

    QScrollArea *analogScroll_ = nullptr;
    QScrollArea *digitalScroll_ = nullptr;
    QWidget *analogWidget_;
    QWidget *digitalWidget_;

    QList<QLabel *> analogLabels_;
    QList<QLabel *> digitalLabels_;
    PlotSplitter *analogSplitter_ = nullptr;
    QwtPlotPicker *lastUsedPicker_;
    QPointF lastSelectedPoint_;

    QList<QwtPlotMarker *> analogVerticalMarkers_;
    QList<QwtPlotMarker *> digitalVerticalMarkers_;

    QWidget *analogVerticalMarker_;
    QWidget *digitalVerticalMarker_;

    bool sizesAdjusted_ = false;

    ReplottingPanner *panner0_ = nullptr;
};

#endif //__db7e4801188eae66f852586df7ac58a1db2e5a3d
