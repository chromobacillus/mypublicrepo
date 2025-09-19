#include "oscplot.hpp"

double getMin(QList<double> list) {
    double result = 1.6e308;
    for (double value : list) {
        if (value < result) { result = value; }
    }
    return result;
}

double getMax(QList<double> list) {
    double result = -1.6e308;
    for (double value : list) {
        if (value > result) { result = value; }
    }
    return result;
}

CurveBox::CurveBox(QWidget *parent) : QWidget{parent} {
    SimleOscReader *simleOscReader = new SimleOscReader();
//    simleOscReader->readComtradeRecord("/home/iazharov/zharov/frontend_additional/Binary_float32/KSZ_01.cfg");
    simleOscReader->readComtradeRecord("/home/iazharov/zharov/frontend_additional/OSC/RAS_test1.cfg");

    Size = simleOscReader->comtradeRecord_->rawAnalogValues().at(0).size();
    CurvCnt = simleOscReader->comtradeRecord_->config().analogChannelsNumber();

    xMap.setScaleInterval( -0.5, 10.5 );
//    yMap.setScaleInterval( -1.1, 1.1 );

//    xval = new double[Size];
//    yval = new double[Size];
    yMap = new QwtScaleMap[CurvCnt];
    xval = new double*[CurvCnt];
    yval = new double*[CurvCnt];
    for (int i = 0; i < CurvCnt; i++) {
        xval[i] = new double[Size];
        yval[i] = new double[Size];
    }
    for (int i = 0; i < CurvCnt; i++) {
        QList<double> values = simleOscReader->comtradeRecord_->analogValues().at(i);
        yMap[i].setScaleInterval(getMin(values), getMax(values));
        for( int j = 0; j < Size; j++ ) {
            xval[i][j] = double(j) * 10.0 / double( Size - 1 );
            yval[i][j] = values.at(j);
        }
    }
    m_curves = new QwtPlotCurve[CurvCnt];
    populate();
}

CurveBox::~CurveBox() {
    for (int i = 0; i < CurvCnt; ++i) {
        delete[] xval[i];
        delete[] yval[i];
    }
    delete[] xval;
    delete[] yval;
    delete[] m_curves;
}

void CurveBox::populate()
{
    int i = 0;

//    m_curves[i].setSymbol( new QwtSymbol( QwtSymbol::Cross, Qt::NoBrush,
//        QPen( Qt::black ), QSize( 5, 5 ) ) );
//    m_curves[i].setPen( Qt::darkGreen );
//    m_curves[i].setStyle( QwtPlotCurve::Lines );
//    m_curves[i].setCurveAttribute( QwtPlotCurve::Fitted );
//    i++;

//    m_curves[i].setSymbol( new QwtSymbol( QwtSymbol::Ellipse, Qt::yellow,
//        QPen( Qt::blue ), QSize( 5, 5 ) ) );
//    m_curves[i].setPen( Qt::red );
//    m_curves[i].setStyle( QwtPlotCurve::Sticks );
//    i++;

//    m_curves[i].setPen( Qt::darkBlue );
//    m_curves[i].setStyle( QwtPlotCurve::Lines );
//    i++;

//    m_curves[i].setPen( Qt::darkBlue );
//    m_curves[i].setStyle( QwtPlotCurve::Lines );
//    m_curves[i].setRenderHint( QwtPlotItem::RenderAntialiased );
//    i++;

//    m_curves[i].setPen( Qt::darkCyan );
//    m_curves[i].setStyle( QwtPlotCurve::Steps );
//    i++;

//    m_curves[i].setSymbol( new QwtSymbol( QwtSymbol::XCross, Qt::NoBrush,
//        QPen( Qt::darkMagenta ), QSize( 5, 5 ) ) );
//    m_curves[i].setStyle( QwtPlotCurve::NoCurve );
//    i++;

    for( i = 0; i < CurvCnt; i++ ) {
        m_curves[i].setPen( Qt::darkBlue );
        m_curves[i].setStyle( QwtPlotCurve::Lines );
        m_curves[i].setRenderHint( QwtPlotItem::RenderAntialiased );
        m_curves[i].setRawSamples( xval[i], yval[i], Size );
    }
}

void CurveBox::shiftDown( QRect& rect, int offset ) const
{
    rect.translate( 0, offset );
}

void CurveBox::paintEvent( QPaintEvent* event )
{
    QWidget::paintEvent( event );

    QPainter painter( this );
    painter.setClipRect( contentsRect() );

    drawCurves( &painter );
}

void CurveBox::drawCurves( QPainter* painter )
{
    int deltay, i;

    QRect r = contentsRect();

    deltay = r.height() / CurvCnt - 1;

    r.setHeight( deltay );

    //  draw curves
    for ( i = 0; i < CurvCnt; i++ )
    {
        xMap.setPaintInterval( r.left(), r.right() );
        yMap[i].setPaintInterval( r.top(), r.bottom() );

        painter->setRenderHint( QPainter::Antialiasing,
            m_curves[i].testRenderHint( QwtPlotItem::RenderAntialiased ) );
        m_curves[i].draw( painter, xMap, yMap[i], r );

        shiftDown( r, deltay );
    }

    // draw titles
    r = contentsRect();
    painter->setFont( QFont( "Helvetica", 8 ) );

//    const int alignment = Qt::AlignTop | Qt::AlignHCenter;

//    painter->setPen( Qt::black );

//    painter->drawText( 0, r.top(), r.width(), painter->fontMetrics().height(),
//        alignment, "Style: Line/Fitted, Symbol: Cross" );
//    shiftDown( r, deltay );

//    painter->drawText( 0, r.top(), r.width(), painter->fontMetrics().height(),
//        alignment, "Style: Sticks, Symbol: Ellipse" );
//    shiftDown( r, deltay );

//    painter->drawText( 0, r.top(), r.width(), painter->fontMetrics().height(),
//        alignment, "Style: Lines, Symbol: None" );
//    shiftDown( r, deltay );

//    painter->drawText( 0, r.top(), r.width(), painter->fontMetrics().height(),
//        alignment, "Style: Lines, Symbol: None, Antialiased" );
//    shiftDown( r, deltay );

//    painter->drawText( 0, r.top(), r.width(), painter->fontMetrics().height(),
//        alignment, "Style: Steps, Symbol: None" );
//    shiftDown( r, deltay );

//    painter->drawText( 0, r.top(), r.width(), painter->fontMetrics().height(),
//        alignment, "Style: NoCurve, Symbol: XCross" );
}
