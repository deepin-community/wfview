#include "meter.h"
#include <QDebug>

meter::meter(QWidget *parent) : QWidget(parent)
{
    //QPainter painter(this);

    // Colors from qdarkstylesheet:
    //    $COLOR_BACKGROUND_LIGHT: #505F69;
    //    $COLOR_BACKGROUND_NORMAL: #32414B;
    //    $COLOR_BACKGROUND_DARK: #19232D;
    //    $COLOR_FOREGROUND_LIGHT: #F0F0F0; // grey
    //    $COLOR_FOREGROUND_NORMAL: #AAAAAA; // grey
    //    $COLOR_FOREGROUND_DARK: #787878; // grey
    //    $COLOR_SELECTION_LIGHT: #148CD2;
    //    $COLOR_SELECTION_NORMAL: #1464A0;
    //    $COLOR_SELECTION_DARK: #14506E;


    // Colors I found that I liked from VFD images:
    //      3FB7CD
    //      3CA0DB
    //
    // Text in qdarkstylesheet seems to be #EFF0F1

    if(drawLabels)
    {
        mXstart = 32;
    } else {
        mXstart = 0;
    }

    meterType = meterNone;

    currentColor = colorFromString("#148CD2");
    currentColor = currentColor.darker();

    peakColor = colorFromString("#3CA0DB");
    peakColor = peakColor.lighter();

    averageColor = colorFromString("#3FB7CD");

    lowTextColor = colorFromString("#eff0f1");
    lowLineColor = lowTextColor;

    avgLevels.resize(averageBalisticLength, 0);
    peakLevels.resize(peakBalisticLength, 0);


    combo = new QComboBox(this);
    combo->addItem("None", meterNone);
    combo->addItem("SWR", meterSWR);
    combo->addItem("ALC", meterALC);
    combo->addItem("Compression", meterComp);
    combo->addItem("Voltage", meterVoltage);
    combo->addItem("Current", meterCurrent);
    combo->addItem("Center", meterCenter);
    combo->addItem("TxRxAudio", meterAudio);
    combo->addItem("RxAudio", meterRxAudio);
    combo->addItem("TxAudio", meterTxMod);

    combo->blockSignals(false);
    connect(combo, SIGNAL(activated(int)), this, SLOT(acceptComboItem(int)));
    //connect(combo, SIGNAL(currentIndexChanged(int)), this, SLOT(acceptComboItem(int)));

    this->setToolTip("");
    combo->hide();
    this->installEventFilter(this);
}

void meter::setCompReverse(bool reverse) {
    this->reverseCompMeter = reverse;
}

void meter::setColors(QColor current, QColor peakScale, QColor peakLevel,
                      QColor average, QColor lowLine,
                      QColor lowText)
{
    currentColor = current;

    peakColor = peakLevel; // color for the peak level indicator
    highLineColor = peakScale; // color for the red side of the scale
    highTextColor = peakScale; // color for the red side of the scale's text

    averageColor = average;

    midScaleColor = QColor(Qt::yellow);
    centerTuningColor = QColor(Qt::green);

    lowLineColor = lowLine;
    lowTextColor = lowText;
    this->update();
}

void meter::clearMeterOnPTTtoggle()
{
    // When a meter changes type, such as the fixed S -- TxPo meter,
    // there is automatic clearing. However, some meters do not switch on their own,
    // and thus we are providing this clearing method. We are careful
    // not to clear meters that don't make sense to clear (such as Vd and Id)


    if( (meterType == meterALC) || (meterType == meterSWR)
            || (meterType == meterComp) || (meterType == meterTxMod)
            || (meterType == meterCenter ))
    {
        clearMeter();
    }
}

void meter::clearMeter()
{
    current = 0;
    average = 0;
    peak = 0;

    avgLevels.clear();
    peakLevels.clear();
    avgLevels.resize(averageBalisticLength, 0);
    peakLevels.resize(peakBalisticLength, 0);

    peakPosition = 0;
    avgPosition = 0;
    // re-draw scale:
    update();
}

void meter::setMeterType(meter_t m_type_req)
{
    if(m_type_req == meterType)
        return;

    //if( (m_type_req == meterS) || (m_type_req == meterPower) ) {
    //    this->setToolTip("");
    //} else {
    this->setToolTip("Double-click to select meter type.");
    //}

    combo->blockSignals(true);
    combo->clear();
    switch (m_type_req)
    {
    case meterS:
    case meterPower:
    case meterdBm:
    case meterdBu:
    case meterdBuEMF:
        combo->addItem("S-Meter", meterS);
        combo->addItem("dBu Meter", meterdBu);
        combo->addItem("dBu EMF", meterdBuEMF);
        combo->addItem("dBm Meter", meterdBm);
        break;
    default:
        combo->addItem("None", meterNone);
        combo->addItem("SWR", meterSWR);
        combo->addItem("ALC", meterALC);
        combo->addItem("Compression", meterComp);
        combo->addItem("Voltage", meterVoltage);
        combo->addItem("Current", meterCurrent);
        combo->addItem("Center", meterCenter);
        combo->addItem("TxRxAudio", meterAudio);
        combo->addItem("RxAudio", meterRxAudio);
        combo->addItem("TxAudio", meterTxMod);
        break;
    }

    int m_index = combo->findData(m_type_req);
    combo->setCurrentIndex(m_index);
    combo->blockSignals(false);

    meterType = m_type_req;
    // clear average and peak vectors:
    this->clearMeter();
}

void meter::blockMeterType(meter_t mtype) {
    // TODO: Code to block off duplicate meter types from the selection menu
    enableAllComboBoxItems(combo);
    qDebug() << "Asked to block meter of type: " << getMeterDebug(mtype);
    if(mtype != meterNone) {
        int m_index = combo->findData(mtype);
        setComboBoxItemEnabled(combo, m_index, false);
    }
}

meter_t meter::getMeterType()
{
    return meterType;
}

void meter::setMeterShortString(QString s)
{
    meterShortString = s;
}

QString meter::getMeterShortString()
{
    return meterShortString;
}

void meter::acceptComboItem(int item) {
    qDebug() << "Meter selected combo item number: " << item;

    meter_t meterTypeRequested = static_cast<meter_t>(combo->itemData(item).toInt());
    if(meterType != meterTypeRequested) {
        qDebug() << "Changing meter to type: " << getMeterDebug(meterTypeRequested) << ", with item index: " << item;
        emit configureMeterSignal(meterTypeRequested);
    }

    combo->hide();
    freezeDrawing = false;
}

void meter::handleDoubleClick() {
    freezeDrawing = true;
    combo->show();
}

bool meter::eventFilter(QObject *object, QEvent *event) {

    if( (freezeDrawing) && (event->type() == QEvent::MouseButtonPress) ) {
        combo->hide();
        freezeDrawing = false;
        return true;
    }

    if( (freezeDrawing) && (event->type() == QEvent::MouseButtonDblClick) ) {
        combo->hide();
        freezeDrawing = false;
        return true;
    }

    if(event->type() == QEvent::MouseButtonDblClick) {
        //if( !(meterType == meterS || meterType == meterPower)) {
        handleDoubleClick();
        //}
        return true;
    }

    (void)object;
    return false;
}

void meter::paintEvent(QPaintEvent *)
{
    if(freezeDrawing)
        return;

    QPainter painter(this);
    // This next line sets up a canvis within the
    // space of the widget, and gives it coordinates.
    // The end effect, is that the drawing functions will all
    // scale to the window size.
    painter.setRenderHint(QPainter::SmoothPixmapTransform);
    painter.setFont(QFont(this->fontInfo().family(), fontSize));
    widgetWindowHeight = this->height();
    painter.setWindow(QRect(0, 0, 255+mXstart+15, widgetWindowHeight));
    barHeight = widgetWindowHeight / 2;

    switch(meterType)
    {
        case meterS:
            label = "S";
            peakRedLevel = 120; // S9+
            drawScaleS(&painter);
            break;
        case meterPower:
            label = "PWR";
            peakRedLevel = 210; // 100%
            drawScalePo(&painter);
            break;
        case meterALC:
            label = "ALC";
            peakRedLevel = 100;
            drawScaleALC(&painter);
            break;
        case meterSWR:
            label = "SWR";
            peakRedLevel = 100; // SWR 2.5
            drawScaleSWR(&painter);
            break;
        case meterCenter:
            label = "CTR";
            peakRedLevel = 256; // No need for red here
            drawScaleCenter(&painter);
            break;
        case meterVoltage:
            label = "Vd";
            peakRedLevel = 241;
            drawScaleVd(&painter);
            break;
        case meterCurrent:
            label = "Id";
            peakRedLevel = 120;
            drawScaleId(&painter);
            break;
        case meterComp:
            label = "CMP(dB)";
            peakRedLevel = 100;
            if(reverseCompMeter) {
                drawScaleCompInverted(&painter);
            } else {
                drawScaleComp(&painter);
            }
            break;
        case meterNone:
            label = tr("Double-click to set meter");
            drawLabel(&painter);
            return;
            break;
        case meterAudio:
            label = "dBfs";
            peakRedLevel = 241;
            drawScale_dBFs(&painter);
            break;
        case meterRxAudio:
            label = "Rx(dBfs)";
            peakRedLevel = 241;
            drawScale_dBFs(&painter);
            break;
        case meterTxMod:
            label = "Tx(dBfs)";
            peakRedLevel = 241;
            drawScale_dBFs(&painter);
            break;
        case meterdBu:
            label = "dBu";
            peakRedLevel = 255;
            drawScaledB(&painter,0,80,20);
            break;
        case meterdBuEMF:
            label = "dBu(EMF)";
            peakRedLevel = 255;
            drawScaledB(&painter,0,85,20);
            break;
        case meterdBm:
            label = "dBm";
            peakRedLevel = 255;
            drawScaledB(&painter,-100,-20,20);
            break;
        default:
            label = "DN";
            peakRedLevel = 241;
            drawScaleRaw(&painter);
            break;
    }

    // Current: the most-current value.
    // Draws a bar from start to value.
    painter.setPen(currentColor);
    painter.setBrush(currentColor);

    if(meterType == meterCenter)
    {
        painter.drawRect(mXstart+128,mYstart,current-128,barHeight);

        // Average:
        painter.setPen(averageColor);
        painter.setBrush(averageColor);
        painter.drawRect(mXstart+average-1,mYstart,1,barHeight); // bar is 1 pixel wide, height = meter start?

        // Peak:
        painter.setPen(peakColor);
        painter.setBrush(peakColor);
        if((peak > 191) || (peak < 63))
        {
            painter.setBrush(Qt::red);
            painter.setPen(Qt::red);
        }

        painter.drawRect(mXstart+peak-1,mYstart,1,barHeight);

    } else if ( (meterType == meterAudio) ||
                (meterType == meterTxMod) ||
                (meterType == meterRxAudio))
    {
        // Log scale but still 0-255:
        int logCurrent = (int)((1-audiopot[255-current])*255);
        int logAverage = (int)((1-audiopot[255-average])*255);
        int logPeak = (int)((1-audiopot[255-peak])*255);

        // Current value:
        // X, Y, Width, Height
        painter.drawRect(mXstart,mYstart,logCurrent,barHeight);

        // Average:
        painter.setPen(averageColor);
        painter.setBrush(averageColor);
        painter.drawRect(mXstart+logAverage-1,mYstart,1,barHeight); // bar is 1 pixel wide, height = meter start?

        // Peak:
        painter.setPen(peakColor);
        painter.setBrush(peakColor);
        if(peak > peakRedLevel)
        {
            painter.setBrush(Qt::red);
            painter.setPen(Qt::red);
        }

        painter.drawRect(mXstart+logPeak-1,mYstart,2,barHeight);

    } else if (meterType==meterdBu || meterType==meterdBuEMF || meterType==meterdBm) {

        // Current value:
        // X, Y, Width, Height
        float myValue=flCurrent;

        if (meterType == meterdBm) {
            if (flCurrent<-100.0)
                myValue=0.0;
            else if (flCurrent < 0.0)
                myValue=flCurrent+100.0;
        } else if (flCurrent < 0.0)
        {
            myValue = 0.0;
        }

        myValue=(float(256.0/80.0))*myValue;


        // Absolute ranges for each meter:


//        myValue = myValue*5.0;

        painter.drawRect(mXstart,mYstart,myValue,barHeight);

        // Average:
        painter.setPen(averageColor);
        painter.setBrush(averageColor);
        painter.drawRect(mXstart+average-1,mYstart,1,barHeight); // bar is 1 pixel wide, height = meter start?

        // Peak:
        painter.setPen(peakColor);
        painter.setBrush(peakColor);
        if(peak > peakRedLevel)
        {
            painter.setBrush(Qt::red);
            painter.setPen(Qt::red);
        }
        painter.drawRect(mXstart+peak-1,mYstart,2,barHeight);
        drawValue(&painter,flCurrent);
    } else {

        // Current value:
        // X, Y, Width, Height
        if(meterType==meterComp && reverseCompMeter) {
            painter.drawRect(255+mXstart,mYstart,-current,barHeight);
        } else {
            painter.drawRect(mXstart,mYstart,current,barHeight);
        }

        // Average:
        painter.setPen(averageColor);
        painter.setBrush(averageColor);
        if(meterType==meterComp && reverseCompMeter) {
            painter.drawRect(255+mXstart-average,mYstart,-1,barHeight); // bar is 1 pixel wide, height = meter start?
        } else {
            painter.drawRect(mXstart+average-1,mYstart,1,barHeight); // bar is 1 pixel wide, height = meter start?
        }
        // Peak:
        painter.setPen(peakColor);
        painter.setBrush(peakColor);
        if(peak > peakRedLevel)
        {
            painter.setBrush(Qt::red);
            painter.setPen(Qt::red);
        }
        if(meterType==meterComp && reverseCompMeter) {
            painter.drawRect(255+mXstart-peak+1,mYstart,-1,barHeight);
        } else {
            painter.drawRect(mXstart+peak-1,mYstart,2,barHeight);
        }
    }


    if(drawLabels)
    {
        drawLabel(&painter);
    }


    haveUpdatedData = false;
}

void meter::drawLabel(QPainter *qp)
{
    qp->setPen(lowTextColor);
    qp->drawText(0,scaleTextYstart, label );
}

void meter::drawValue(QPainter *qp, float value)
{
    qp->setPen(lowTextColor);
    uchar prec=1;
    if (value >= 100.0 || value <= -100.0)
        prec=0;
    qp->drawText(0,scaleTextYstart+20, QString("%0").arg(value,0,'f',prec,'0'));

}

void meter::setLevel(float current)
{
    this->flCurrent = current;
    haveUpdatedData = true;
    this->update();
}

void meter::setLevel(int current)
{
    this->current = current;

    avgLevels[(avgPosition++)%averageBalisticLength] = current;
    peakLevels[(peakPosition++)%peakBalisticLength] = current;

    int sum=0;

    for(unsigned int i=0; i < (unsigned int)std::min(avgPosition, (int)avgLevels.size()); i++)
    {
        sum += avgLevels.at(i);
    }
    this->average = sum / std::min(avgPosition, (int)avgLevels.size());

    this->peak = 0;

    for(unsigned int i=0; i < peakLevels.size(); i++)
    {
        if( peakLevels.at(i) >  this->peak)
            this->peak = peakLevels.at(i);
    }

    haveUpdatedData = true;
    this->update();
}

void meter::setLevels(int current, int peak)
{
    this->current = current;
    this->peak = peak;

    avgLevels[(avgPosition++)%averageBalisticLength] = current;
    int sum=0;
    for(unsigned int i=0; i < (unsigned int)std::min(avgPosition, (int)avgLevels.size()); i++)
    {
        sum += avgLevels.at(i);
    }
    this->average = sum / std::min(avgPosition, (int)avgLevels.size());

    haveUpdatedData = true;
    this->update(); // place repaint event on the event queue
}

void meter::setLevels(int current, int peak, int average)
{
    this->current = current;
    this->peak = peak;
    this->average = average;

    haveUpdatedData = true;
    this->update(); // place repaint event on the event queue
}

void meter::updateDrawing(int num)
{
    fontSize = num;
    length = num;
}

// The drawScale functions draw the numbers and number unerline for each type of meter

void meter::drawScaleRaw(QPainter *qp)
{
    qp->setPen(lowTextColor);
    //qp->setFont(QFont("Arial", fontSize));
    int i=mXstart;
    for(; i<mXstart+256; i+=25)
    {
        qp->drawText(i,scaleTextYstart, QString("%1").arg(i) );
    }

    // Now the lines:
    qp->setPen(lowLineColor);

    // Line: X1, Y1 -->to--> X2, Y2
    qp->drawLine(mXstart,scaleLineYstart,peakRedLevel+mXstart,scaleLineYstart);
    qp->setPen(highLineColor);
    qp->drawLine(peakRedLevel+mXstart,scaleLineYstart,255+mXstart,scaleLineYstart);
}

void meter::drawScaledB(QPainter *qp, int start, int end,int step) {
    qp->setPen(lowTextColor);

    //qp->setFont(QFont("Arial", fontSize));
    int y=start;
    int range=(float(256.0/(end-start))*step);

    //double step = static_cast<double>(end - start) / (count - 1);

    for(int i=mXstart; i<mXstart+256; i+=range)
    {
        if (meterType != meterdBuEMF || (meterType == meterdBuEMF && y != 0))
            qp->drawText(i,scaleTextYstart, QString("%1").arg(y) );
        y=y+step;
    }

    for(int i=mXstart; i<mXstart+256; i+=(range/2))
    {
        qp->drawLine(i,scaleTextYstart, i, scaleTextYstart+5);
    }


    // Now the lines:
    qp->setPen(lowLineColor);

    // Line: X1, Y1 -->to--> X2, Y2
    qp->drawLine(mXstart,scaleLineYstart,peakRedLevel+mXstart,scaleLineYstart);
    qp->setPen(highLineColor);
    qp->drawLine(peakRedLevel+mXstart,scaleLineYstart,255+mXstart,scaleLineYstart);
}

void meter::drawScale_dBFs(QPainter *qp)
{
    qp->setPen(lowTextColor);
    peakRedLevel = 193;

    if(meterType==meterAudio)
        qp->drawText(20+mXstart,scaleTextYstart, QString("-30"));
    qp->drawText(38+mXstart+2,scaleTextYstart, QString("-24"));
    qp->drawText(71+mXstart,scaleTextYstart, QString("-18"));
    qp->drawText(124+mXstart,scaleTextYstart, QString("-12"));
    qp->drawText(193+mXstart,scaleTextYstart, QString("-6"));
    qp->drawText(255+mXstart,scaleTextYstart, QString("0"));

    // Low ticks:
    qp->setPen(lowLineColor);
    qp->drawLine(20+mXstart,scaleTextYstart, 20+mXstart, scaleTextYstart+5);
    qp->drawLine(38+mXstart,scaleTextYstart, 38+mXstart, scaleTextYstart+5);
    qp->drawLine(71+mXstart,scaleTextYstart, 71+mXstart, scaleTextYstart+5);
    qp->drawLine(124+mXstart,scaleTextYstart, 124+mXstart, scaleTextYstart+5);


    // High ticks:
    qp->setPen(highLineColor);
    qp->drawLine(193+mXstart,scaleTextYstart, 193+mXstart, scaleTextYstart+5);
    qp->drawLine(255+mXstart,scaleTextYstart, 255+mXstart, scaleTextYstart+5);

    // Now the lines:
    qp->setPen(lowLineColor);

    // Line: X1, Y1 -->to--> X2, Y2
    qp->drawLine(mXstart,scaleLineYstart,peakRedLevel+mXstart,scaleLineYstart);
    qp->setPen(highLineColor);
    qp->drawLine(peakRedLevel+mXstart,scaleLineYstart,255+mXstart,scaleLineYstart);
}

void meter::drawScaleVd(QPainter *qp)
{
    qp->setPen(lowTextColor);
    //qp->setFont(QFont("Arial", fontSize));

    // 7300/9700 and others:
    int midPointDn = 13;
    int midPointVd = 10;

    // 705:
    //int midPointDn = 75;
    //int midPointVd = 5;

    int highPointDn = 241;
    int highPointVd = 16;
    float VdperDn = (float)(highPointVd-midPointVd) / float(highPointDn-midPointDn);

    int i=mXstart;
    for(; i<mXstart+midPointDn; i+=midPointDn/1)
    {
        qp->drawText(i,scaleTextYstart, QString("%1").arg( (int)((i-mXstart) * (float(midPointVd) / float(midPointDn)) )) );
    }

    for(; i<mXstart+255; i+= (highPointDn-midPointDn) / (highPointVd-midPointVd))
    {
        qp->drawText(i,scaleTextYstart, QString("%1").arg( (int) std::round( ((i-mXstart-midPointDn) * (VdperDn) ) + (midPointVd) )));
        qp->drawLine(i,scaleTextYstart, i, scaleTextYstart+5);
    }

    // Now the lines:
    qp->setPen(lowLineColor);

    // Line: X1, Y1 -->to--> X2, Y2
    qp->drawLine(mXstart,scaleLineYstart,peakRedLevel+mXstart,scaleLineYstart);
    qp->setPen(highLineColor);
    qp->drawLine(peakRedLevel+mXstart,scaleLineYstart,255+mXstart,scaleLineYstart);

}

void meter::drawScaleCenter(QPainter *qp)
{
    // No known units
    qp->setPen(lowLineColor);
    qp->drawText(60+mXstart,scaleTextYstart, QString("-"));

    qp->setPen(centerTuningColor);
    // Attempt to draw the zero at the actual center
    qp->drawText(128-2+mXstart,scaleTextYstart, QString("0"));

    qp->setPen(lowLineColor);
    qp->drawText(195+mXstart,scaleTextYstart, QString("+"));


    qp->setPen(lowLineColor);
    qp->drawLine(mXstart,scaleLineYstart,128-32+mXstart,scaleLineYstart);

    qp->setPen(centerTuningColor);
    qp->drawLine(128-32+mXstart,scaleLineYstart,128+32+mXstart,scaleLineYstart);

    qp->setPen(lowLineColor);
    qp->drawLine(128+32+mXstart,scaleLineYstart,255+mXstart,scaleLineYstart);
}


void meter::drawScalePo(QPainter *qp)
{
    //From the manual: "0000=0% to 0143=50% to 0213=100%"
    float dnPerWatt = 143.0f / 50.0f;

    qp->setPen(lowTextColor);
    //qp->setFont(QFont("Arial", fontSize));

    int i=mXstart;
    // 13.3 DN per s-unit:
    int p=0;
    for(; i<mXstart+143; i+=(int)(10*dnPerWatt))
    {
        // Stop just before the next 10w spot
        if(i<mXstart+140)
            qp->drawText(i,scaleTextYstart, QString("%1").arg(10*(p++)) );
    }

    // Modify current scale position:

    // Here, P is now 60 watts:
    // Higher scale:
    //i = i - (int)(10*dnPerWatt); // back one tick first. Otherwise i starts at 178. **Not used?**
    //qDebug() << "meter i: " << i;
    dnPerWatt = (213-143.0f) / 50.0f; // 1.4 dn per watt
    // P=5 here.
    qp->setPen(midScaleColor);
    int k=0;
    for(i=mXstart+143; i<mXstart+213; i+=(5*dnPerWatt))
    {
        k = 50+(( i-mXstart-143 ) / dnPerWatt);
        if(k==40||k==50||k==65||k==80)
            qp->drawText(i,scaleTextYstart, QString("%1").arg(k) );
    }

    // Now we're out past 100:
    qp->setPen(highTextColor);

    for(i=mXstart+213; i<mXstart+255; i+=(10*dnPerWatt))
    {
        k = 50+(( i-mXstart-143 ) / dnPerWatt);
        if(k==100)
            qp->drawText(i,scaleTextYstart, QString("%1").arg(k) );
    }

    // Now the lines:
    qp->setPen(lowLineColor);

    // Line: X1, Y1 -->to--> X2, Y2
    qp->drawLine(mXstart,scaleLineYstart,213+mXstart,scaleLineYstart);
    qp->setPen(highLineColor);
    qp->drawLine(213+mXstart,scaleLineYstart,255+mXstart,scaleLineYstart);

    (void)qp;
}

void meter::drawScaleRxdB(QPainter *qp)
{
    (void)qp;
}

void meter::drawScaleALC(QPainter *qp)
{
    // From the manual: 0000=Minimum to 0120=Maximum

    qp->setPen(lowTextColor);
    //qp->setFont(QFont("Arial", fontSize));
    int i=mXstart;
    int alc=0;
    for(; i<mXstart+100; i += (20))
    {
        qp->drawText(i,scaleTextYstart, QString("%1").arg(alc) );
        qp->drawLine(i,scaleTextYstart, i, scaleTextYstart+5);
        alc +=20;
    }

    qp->setPen(highTextColor);

    for(; i<mXstart+120; i+=(int)(10*i))
    {
        qp->drawText(i,scaleTextYstart, QString("+%1").arg(alc) );
        qp->drawLine(i,scaleTextYstart, i, scaleTextYstart+5);
        alc +=10;
    }

    qp->setPen(lowLineColor);
    qp->drawLine(mXstart,scaleLineYstart,100+mXstart,scaleLineYstart);
    qp->setPen(highLineColor);
    qp->drawLine(100+mXstart,scaleLineYstart,255+mXstart,scaleLineYstart);

    (void)qp;
}

void meter::drawScaleComp(QPainter *qp)
{
    //
    // 0000=0 dB, 0130=15 dB,0241=30 dB
    //

    qp->setPen(lowTextColor);

    int midPointDn = 130;
    int midPointdB = 15;

    int highPointDn = 241;
    int highPointdB = 30;
    float dBperDn = (float)(highPointdB-midPointdB) / float(highPointDn-midPointDn);

    int i=mXstart;
    i+=midPointDn/4; // skip the 0 for cleaner label space
    for(; i<mXstart+midPointDn; i+=midPointDn/4)
    {
        qp->drawText(i,scaleTextYstart, QString("%1").arg( (int)((i-mXstart) * (float(midPointdB) / float(midPointDn)) )) );
        qp->drawLine(i,scaleTextYstart, i, scaleTextYstart+5);
    }

    i = midPointDn+60;
    for(; i<mXstart+255; i+= 30)
    {
        qp->drawText(i,scaleTextYstart, QString("%1").arg( (int) std::round( ((i-mXstart-midPointDn) * (dBperDn) ) + (midPointdB) )));
        qp->drawLine(i,scaleTextYstart, i, scaleTextYstart+5);
    }

    // Now the lines:
    qp->setPen(lowLineColor);

    // Line: X1, Y1 -->to--> X2, Y2
    qp->drawLine(mXstart,scaleLineYstart,peakRedLevel+mXstart,scaleLineYstart);
    qp->setPen(highLineColor);
    qp->drawLine(peakRedLevel+mXstart,scaleLineYstart,255+mXstart,scaleLineYstart);
}

void meter::drawScaleCompInverted(QPainter *qp) {
    // inverted scale
    //
    // 0000=0 dB, 0130=15 dB,0241=30 dB
    //
    // rev DN  :        255 ------------------------------------- 0
    // Geometry:  0---mXstart------------------------------------255+mXstart-----
    // norm DN :         0  -------------------------------------255

    qp->setPen(lowTextColor);

    int midPointDn = 130;
    int midPointdB = 15;

    int highPointDn = 241;
    int highPointdB = 30;
    float dBperDn = (float)(highPointdB-midPointdB) / float(highPointDn-midPointDn);

    int i=mXstart;
    //i+=midPointDn/4; // skip the 0 for cleaner label space

    // Vertical graticules and text

    // drawLine expects two sets of coordinates dictating start and end position.

    // numbers 0-14:
    for(; i<mXstart+midPointDn; i+=midPointDn/4)
    {
        qp->drawText(255+mXstart-i+(midPointDn/4),scaleTextYstart, QString("%1").arg( (int)((i-mXstart) * (float(midPointdB) / float(midPointDn)) )) );
        qp->drawLine(255+mXstart-i+(midPointDn/4),scaleTextYstart, 255+mXstart-i+(midPointDn/4), scaleTextYstart+5);
    }

    // numbers 19-30
    i = midPointDn+60;
    // The "-30" blocks the last digit which runs over the label text
    for(; i<mXstart+255-30; i+= 30)
    {
        qp->drawText(255+mXstart+32-i,scaleTextYstart, QString("%1").arg( (int) std::round( ((i-mXstart-midPointDn) * (dBperDn) ) + (midPointdB) )));
        qp->drawLine(255+mXstart+32-i,scaleTextYstart, 255+mXstart+32-i, scaleTextYstart+5);
    }

    // Now the lines:
    qp->setPen(lowLineColor);

    // Line: X1, Y1 -->to--> X2, Y2
    qp->drawLine(255+mXstart,scaleLineYstart,255-peakRedLevel+mXstart,scaleLineYstart);
    qp->setPen(highLineColor);
    qp->drawLine(255-peakRedLevel+mXstart,scaleLineYstart,mXstart,scaleLineYstart);
}

void meter::drawScaleSWR(QPainter *qp)
{
    // From the manual:
    // 0000=SWR1.0,
    // 0048=SWR1.5,
    // 0080=SWR2.0,
    // 0120=SWR3.0

    qp->setPen(lowTextColor);
    qp->drawText(mXstart,scaleTextYstart, QString("1.0"));
    qp->drawText(24+mXstart,scaleTextYstart, QString("1.3"));
    qp->drawText(48+mXstart,scaleTextYstart, QString("1.5"));    
    qp->drawText(80+mXstart,scaleTextYstart, QString("2.0"));
    qp->drawText(100+mXstart,scaleTextYstart, QString("2.5"));
    qp->setPen(highTextColor);
    qp->drawText(120+mXstart,scaleTextYstart, QString("3.0"));

    qp->setPen(lowLineColor);
    qp->drawLine(  0+mXstart,scaleTextYstart,  0+mXstart, scaleTextYstart+5);
    qp->drawLine( 24+mXstart,scaleTextYstart, 24+mXstart, scaleTextYstart+5);
    qp->drawLine( 48+mXstart,scaleTextYstart, 48+mXstart, scaleTextYstart+5);
    qp->drawLine( 80+mXstart,scaleTextYstart, 80+mXstart, scaleTextYstart+5);
    qp->drawLine(100+mXstart,scaleTextYstart,100+mXstart, scaleTextYstart+5); // does not draw?
    qp->setPen(highLineColor);
    qp->drawLine(120+mXstart,scaleTextYstart,120+mXstart, scaleTextYstart+5);


    qp->setPen(lowLineColor);
    qp->drawLine(mXstart,scaleLineYstart,100+mXstart,scaleLineYstart);
    qp->setPen(highLineColor);
    qp->drawLine(100+mXstart,scaleLineYstart,255+mXstart,scaleLineYstart);
}

void meter::drawScaleId(QPainter *qp)
{
    // IC-7300:
    // 0000=0, 0097=10, 0146=15, 0241=25
    //

    qp->setPen(lowTextColor);
    //qp->setFont(QFont("Arial", fontSize));

    // 7300/9700 and others:
    int midPointDn = 97;
    int midPointId = 10;

    int highPointDn = 146;
    int highPointId = 15;
    float IdperDn = (float)(highPointId-midPointId) / float(highPointDn-midPointDn);

    int i=mXstart;
    for(; i<mXstart+midPointDn; i+=midPointDn/4)
    {
        qp->drawText(i,scaleTextYstart, QString("%1").arg( (int)((i-mXstart) * (float(midPointId) / float(midPointDn)) )) );
        qp->drawLine(i,scaleTextYstart, i, scaleTextYstart+5);
    }

    for(; i<mXstart+255; i+= 4*(highPointDn-midPointDn) / (highPointId-midPointId))
    {
        qp->drawText(i,scaleTextYstart, QString("%1").arg( (int) std::round( ((i-mXstart-midPointDn) * (IdperDn) ) + (midPointId) )));
        qp->drawLine(i,scaleTextYstart, i, scaleTextYstart+5);
    }

    // Now the lines:
    qp->setPen(lowLineColor);

    // Line: X1, Y1 -->to--> X2, Y2
    qp->drawLine(mXstart,scaleLineYstart,peakRedLevel+mXstart,scaleLineYstart);
    qp->setPen(highLineColor);
    qp->drawLine(peakRedLevel+mXstart,scaleLineYstart,255+mXstart,scaleLineYstart);
}

void meter::drawScaleS(QPainter *qp)
{
    //
    // 0000=S0, 0120=S9, 0241=S9+60dB
    //  120 / 9 = 13.333 steps per S-unit

    qp->setPen(lowTextColor);
    //qp->setFont(QFont("Arial", fontSize));
    int i=mXstart;
    // 13.3 DN per s-unit:
    int s=0;
    for(; i<mXstart+120; i+=13)
    {
        qp->drawText(i,scaleTextYstart, QString("%1").arg(s++) );
        qp->drawLine(i,scaleTextYstart, i, scaleTextYstart+5);
    }

    // 2 DN per 1 dB now:
    // 20 DN per 10 dB
    // 40 DN per 20 dB

    // Modify current scale position:
    s = 20;
    i+=20;

    qp->setPen(highTextColor);

    for(; i<mXstart+255; i+=40)
    {
        qp->drawText(i,scaleTextYstart, QString("+%1").arg(s) );
        qp->drawLine(i,scaleTextYstart, i, scaleTextYstart+5);
        s = s + 20;
    }

    qp->setPen(lowLineColor);
    qp->drawLine(mXstart,scaleLineYstart,peakRedLevel+mXstart,scaleLineYstart);
    qp->setPen(highLineColor);
    qp->drawLine(peakRedLevel+mXstart,scaleLineYstart,255+mXstart,scaleLineYstart);
}

void meter::muteSingleComboItem(QComboBox *comboBox, int index) {
    enableAllComboBoxItems(comboBox);
    setComboBoxItemEnabled(comboBox, index, false);
}

void meter::enableAllComboBoxItems(QComboBox *combobox, bool en) {
    for(int i=0; i < combobox->count(); i++) {
        setComboBoxItemEnabled(combobox, i, en);
    }
}

void meter::setComboBoxItemEnabled(QComboBox * comboBox, int index, bool enabled)
{
    auto * model = qobject_cast<QStandardItemModel*>(comboBox->model());
    assert(model);
    if(!model) return;

    auto * item = model->item(index);
    assert(item);
    if(!item) return;
    item->setEnabled(enabled);
}

