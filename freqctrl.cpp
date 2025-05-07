/*
 * Frequency controller widget (originally from CuteSDR)
 *
 * This file is part of wfview
 *
 * Copyright 2010 Moe Wheatley AE4JY
 * Copyright 2012-2017 Alexandru Csete OZ9AEC
 * Copyright 2024 Phil Taylor M0VSE
 * All rights reserved.
 *
 * This software is released under the "Simplified BSD License".
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */
#include <QDebug>
#include "freqctrl.h"
#include "logcategories.h"

// Manual adjustment of Font size as percent of control height
#define DIGIT_SIZE_PERCENT 90
#define UNITS_SIZE_PERCENT 60

// adjustment for separation between digits
#define SEPRATIO_N 100         // separation rectangle size ratio numerator times 100
#define SEPRATIO_D 3           // separation rectangle size ratio denominator

#define STATUS_TIP \
"Scroll or left-click to increase/decrease digit. " \
    "Right-click to clear digits."

    freqCtrl::freqCtrl(QWidget *parent) :
    QFrame(parent)
{
    setAutoFillBackground(false);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setFocusPolicy(Qt::StrongFocus);
    setMouseTracking(true);
    m_BkColor = QColor(0x1F, 0x1D, 0x1D, 0xFF);
    m_InactiveColor = QColor(0x43, 0x43, 0x43, 0xFF);
    m_DigitColor = QColor(0xFF, 0xFF, 0xFF, 0xFF);
    m_HighlightColor = QColor(0x5A, 0x5A, 0x5A, 0xFF);
    m_UnitsColor = Qt::gray;
    m_freq = 146123456;
    setup(0, 1, 4000000000U, 1, FCTL_UNIT_NONE);
    m_Oldfreq = 0;
    m_LastLeadZeroPos = 0;
    m_LRMouseFreqSel = false;
    m_ActiveEditDigit = -1;
    m_ResetLowerDigits = true;
    m_InvertScrolling = false;
    int fontid = QFontDatabase::addApplicationFont(":/resources/frequency.ttf");
    QString font = QFontDatabase::applicationFontFamilies(fontid).at(0);

    m_UnitsFont  = QFont(font,12,QFont::Normal);
    m_DigitFont  = QFont(font,12,QFont::Normal);
    //m_UnitsFont = QFont("Arial", 12, QFont::Normal);
    //m_DigitFont = QFont("Arial", 12, QFont::Normal);

    setStatusTip(tr(STATUS_TIP));
}

freqCtrl::~freqCtrl()
{
}

QSize freqCtrl::minimumSizeHint() const
{
    return QSize(100, 20);
}

QSize freqCtrl::sizeHint() const
{
    return QSize(100, 20);
}

bool freqCtrl::inRect(QRect &rect, QPointF &point)
{
    if ((point.x() < rect.right()) && (point.x() > rect.x()) &&
        (point.y() < rect.bottom()) && (point.y() > rect.y()))
        return true;
    else
        return false;
}

void freqCtrl::setActiveDigit(int idx)
{
    for (int i = m_DigStart; i < m_NumDigits; i++)
    {
        if (i == idx)
        {
            if (!m_DigitInfo[i].editmode)
            {
                m_DigitInfo[i].editmode = true;
                m_ActiveEditDigit = i;
            }
        }
        else
        {
            // un-highlight the previous digit if moved off it
            if (m_DigitInfo[i].editmode)
            {
                m_DigitInfo[i].editmode = false;
                m_DigitInfo[i].modified = true;
            }
        }
    }

    updateCtrl(false);

}

static int fmax_to_numdigits(qint64 fmax)
{
    if (fmax < 10e6)
        return 7;
    else if (fmax < 100e6)
        return 8;
    else if (fmax < 1e9)
        return 9;
    else if (fmax < 10e9)
        return 10;
    else if (fmax < 100e9)
        return 11;

    return 12;
}

void freqCtrl::setup(int NumDigits, qint64 Minf, qint64 Maxf, int MinStep,
                     FctlUnit unit, std::vector<bandType>* bands)
{
    int       i;
    qint64    pwr = 1;
    m_LastEditDigit = 0;
    m_Oldfreq = -1;

    m_NumDigits = NumDigits ? NumDigits : fmax_to_numdigits(Maxf);

    m_Bands = bands;

    if (m_NumDigits > FCTL_MAX_DIGITS)
        m_NumDigits = FCTL_MAX_DIGITS;

    if (m_NumDigits < FCTL_MIN_DIGITS)
        m_NumDigits = FCTL_MIN_DIGITS;

    m_UnitString = "";
    m_MinStep = MinStep;

    if (m_MinStep == 0)
        m_MinStep = 1;

    m_MinFreq = Minf;
    m_MaxFreq = Maxf;

    if (m_freq < m_MinFreq)
        m_freq = m_MinFreq;

    if (m_freq > m_MaxFreq)
        m_freq = m_MaxFreq;

    for (i = 0; i < m_NumDigits; i++)
    {
        m_DigitInfo[i].weight = pwr;
        m_DigitInfo[i].incval = pwr;
        m_DigitInfo[i].modified = true;
        m_DigitInfo[i].editmode = false;
        m_DigitInfo[i].val = 0;
        pwr *= 10;
    }

    if (m_MaxFreq > pwr)
        m_MaxFreq = pwr - 1;

    m_MaxFreq = m_MaxFreq - m_MaxFreq % m_MinStep;

    if (m_MinFreq > pwr)
        m_MinFreq = 1;

    m_MinFreq = m_MinFreq - m_MinFreq % m_MinStep;
    m_DigStart = 0;

    setUnit(unit);

    for (i = m_NumDigits - 1; i >= 0; i--)
    {
        if (m_DigitInfo[i].weight <= m_MinStep)
        {
            if (m_DigStart == 0)
            {
                m_DigitInfo[i].incval = m_MinStep;
                m_DigStart = i;
            }
            else
            {
                if ((m_MinStep % m_DigitInfo[i + 1].weight) != 0)
                    m_DigStart = i;
                m_DigitInfo[i].incval = 0;
            }
        }
    }

    m_NumSeps = (m_NumDigits - 1) / 3 - m_DigStart / 3;
}

void freqCtrl::setFrequency(qint64 freq)
{
    int       i;
    qint64    acc = 0;
    qint64    rem;
    int       val;



    if (freq == m_Oldfreq)
        return;
/*
 * This needs working on, to restrict to only valid bands!
    if (m_Bands != Q_NULLPTR) {
        // We have bands so make sure the frequency is within at least one of them!
        for (int i=0;i<m_Bands->size();i++)
        {
            if (freq >= m_Bands->at(i).lowFreq && freq <= m_Bands->at(i).highFreq)
            {
                break;
            }

            if (m_Oldfreq >= m_Bands->at(i).lowFreq && m_Oldfreq <= m_Bands->at(i).highFreq)
            {
                // The old frequency WAS in this band but it isn't now!
                if (freq >= m_Bands->at(i).highFreq && i <= m_Bands->size()-1)
                {
                    freq=m_Bands->at(i+1).lowFreq;
                }
                else if (freq < m_Bands->at(i).lowFreq && i > 0)
                {
                    freq=m_Bands->at(i-1).highFreq;
                }
                else
                {
                    // Couldn't match it!
                    freq = m_Oldfreq;
                }
                break;
            }


        }
    } else {
*/
    if (freq < m_MinFreq)
        freq = m_MinFreq;

    if (freq > m_MaxFreq)
        freq = m_MaxFreq;

    m_freq = freq - freq % m_MinStep;
    rem = m_freq;
    m_LeadZeroPos = m_NumDigits;

    for (i = m_NumDigits - 1; i >= m_DigStart; i--)
    {
        val = (int)(rem / m_DigitInfo[i].weight);
        if (m_DigitInfo[i].val != val)
        {
            m_DigitInfo[i].val = val;
            m_DigitInfo[i].modified = true;
        }
        rem = rem - val * m_DigitInfo[i].weight;
        acc += val;
        if ((acc == 0) && (i > m_DecPos))
        {
            m_LeadZeroPos = i;
        }
    }

    // If the sign changed and the frequency is less than 1 unit,
    // redraw the leading zero to get the correct sign.
    if ((m_Oldfreq ^ m_freq) < 0 && m_DigitInfo[m_LeadZeroPos - 1].val == 0)
        m_DigitInfo[m_LeadZeroPos - 1].modified = true;

    // When frequency is negative all non-zero digits that
    // have changed will have a negative sign. This loop will
    // change all digits back to positive, except the one at
    // position m_leadZeroPos-1. If that position is zero,
    // it will be checked in the drawing method, drawDigits().
    /** TBC if this works for all configurations */
    if (m_freq < 0)
    {
        if (m_DigitInfo[m_LeadZeroPos - 1].val > 0)
            m_DigitInfo[m_LeadZeroPos -
                        1].val = -m_DigitInfo[m_LeadZeroPos - 1].val;

        for (i = 0; i < (m_LeadZeroPos - 1); i++)
        {
            if (m_DigitInfo[i].val < 0)
                m_DigitInfo[i].val = -m_DigitInfo[i].val;
        }
    }

    // signal the new frequency to world
    m_Oldfreq = m_freq;
    emit    newFrequency(m_freq);
    updateCtrl(m_LastLeadZeroPos != m_LeadZeroPos);
    m_LastLeadZeroPos = m_LeadZeroPos;
}

void freqCtrl::setDigitColor(QColor col)
{
    m_UpdateAll = true;
    m_DigitColor = col;
    for (int i = m_DigStart; i < m_NumDigits; i++)
        m_DigitInfo[i].modified = true;
    updateCtrl(true);
}

void freqCtrl::setUnit(FctlUnit unit)
{
    m_NumDigitsForUnit = 2;
    switch (unit)
    {
    case FCTL_UNIT_NONE:
        m_NumDigitsForUnit = 0;
        m_DecPos = 0;
        m_UnitString = QString();
        break;
    case FCTL_UNIT_HZ:
        m_DecPos = 0;
        m_UnitString = "Hz ";
        break;
    case FCTL_UNIT_KHZ:
        m_DecPos = 3;
        m_UnitString = "kHz";
        break;
    case FCTL_UNIT_MHZ:
        m_DecPos = 6;
        m_UnitString = "MHz";
        break;
    case FCTL_UNIT_GHZ:
        m_DecPos = 9;
        m_UnitString = "GHz";
        break;
    case FCTL_UNIT_SEC:
        m_DecPos = 6;
        m_UnitString = "Sec";
        break;
    case FCTL_UNIT_MSEC:
        m_DecPos = 3;
        m_UnitString = "mS ";
        break;
    case FCTL_UNIT_USEC:
        m_DecPos = 0;
        m_UnitString = "uS ";
        break;
    case FCTL_UNIT_NSEC:
        m_DecPos = 0;
        m_UnitString = "nS ";
        break;
    }
    m_Unit = unit;
    m_UpdateAll = true;
    updateCtrl(true);
}

void freqCtrl::setBgColor(QColor col)
{
    m_UpdateAll = true;
    m_BkColor = col;

    for (int i = m_DigStart; i < m_NumDigits; i++)
        m_DigitInfo[i].modified = true;

    updateCtrl(true);
}

void freqCtrl::setUnitsColor(QColor col)
{
    m_UpdateAll = true;
    m_UnitsColor = col;
    updateCtrl(true);
}

void freqCtrl::setHighlightColor(QColor col)
{
    m_UpdateAll = true;
    m_HighlightColor = col;
    updateCtrl(true);
}

void freqCtrl::updateCtrl(bool all)
{
    if (all)
    {
        m_UpdateAll = true;
        for (int i = m_DigStart; i < m_NumDigits; i++)
            m_DigitInfo[i].modified = true;
    }
    update();
}

void freqCtrl::resizeEvent(QResizeEvent *)
{
    // qDebug() <<rect.width() << rect.height();
    qreal dpr = devicePixelRatioF();
    m_Pixmap = QPixmap(width() * dpr, height() * dpr); // resize pixmap to current control size
    m_Pixmap.setDevicePixelRatio(dpr);
    m_Pixmap.fill(m_BkColor);
    m_UpdateAll = true;
    updateCtrl(true);
}

void freqCtrl::leaveEvent(QEvent *)
{
    // called when mouse cursor leaves this control so deactivate any highlights
    if (m_ActiveEditDigit >= 0)
    {
        if (m_DigitInfo[m_ActiveEditDigit].editmode)
        {
            m_DigitInfo[m_ActiveEditDigit].editmode = false;
            m_DigitInfo[m_ActiveEditDigit].modified = true;
            m_ActiveEditDigit = -1;
            updateCtrl(false);
        }
    }
}

void freqCtrl::paintEvent(QPaintEvent *)
{
    QPainter    painter(&m_Pixmap);

    if (m_UpdateAll)           // if need to redraw everything
    {
        drawBkGround(painter);
        m_UpdateAll = false;
    }
    // draw any modified digits to the m_MemDC
    drawDigits(painter);
    // now draw pixmap onto screen
    QPainter    scrnpainter(this);
    scrnpainter.drawPixmap(0, 0, m_Pixmap); // blt to the screen(flickers like a candle, why?)
}

void freqCtrl::mouseMoveEvent(QMouseEvent *event)
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QPointF pt = event->localPos();
#else
    QPointF pt = event->position();
#endif
    // find which digit is to be edited
    if (isActiveWindow())
    {
        if (!hasFocus())
            setFocus(Qt::MouseFocusReason);

        for (int i = m_DigStart; i < m_NumDigits; i++)
        {
            if (inRect(m_DigitInfo[i].dQRect, pt))
            {
                setActiveDigit(i);
            }
        }
    }
}

void freqCtrl::mousePressEvent(QMouseEvent *event)
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QPointF pt = event->localPos();
#else
    QPointF pt = event->position();
#endif
    if (event->button() == Qt::LeftButton)
    {
        for (int i = m_DigStart; i < m_NumDigits; i++)
        {
            if (inRect(m_DigitInfo[i].dQRect, pt)) // if in i'th digit
            {
                if (m_LRMouseFreqSel)
                {
                    incFreq();
                }
                else
                {
                    if (pt.y() < m_DigitInfo[i].dQRect.bottom() / 2) // top half?
                        incFreq();
                    else
                        decFreq();                                   // bottom half
                }
            }
        }
    }
    else if (event->button() == Qt::RightButton)
    {
        for (int i = m_DigStart; i < m_NumDigits; i++)
        {
            if (inRect(m_DigitInfo[i].dQRect, pt)) // if in i'th digit
            {
                if (m_LRMouseFreqSel)
                {
                    decFreq();
                }
                else
                {
                    clearFreq();
                }
            }
        }
    }
}

void freqCtrl::wheelEvent(QWheelEvent *event)
{
#if QT_VERSION < QT_VERSION_CHECK(5, 14, 0)
    QPointF   pt = QPointF(event->pos());
#else
    QPointF   pt = event->position();
#endif
    int       delta = m_InvertScrolling ? -event->angleDelta().y() : event->angleDelta().y();
    qreal       numDegrees = delta / 8;
    qreal       offset = numDegrees / 15;


    qreal stepsToScroll = QApplication::wheelScrollLines() * offset;

    if( (scrollWheelOffsetAccumulated > 0) && (offset > 0) ) {
        scrollWheelOffsetAccumulated += stepsToScroll;
    } else if ((scrollWheelOffsetAccumulated < 0) && (offset < 0)) {
        scrollWheelOffsetAccumulated += stepsToScroll;
    } else {
        // Changed direction, zap the old accumulation:
        scrollWheelOffsetAccumulated = stepsToScroll;
        //qInfo() << "Scroll changed direction";
    }

    int numSteps = int(scrollWheelOffsetAccumulated);

    if(!numSteps) {
        // we have not accumulated enough for a complete step.
        // come back later.
        return;
    }

    for (int i = m_DigStart; i < m_NumDigits; i++)
    {
        if (inRect(m_DigitInfo[i].dQRect, pt)) // if in i'th digit
        {
            if (numSteps > 0)
                incFreq();
            else if (numSteps < 0)
                decFreq();
        }
    }
    scrollWheelOffsetAccumulated = 0;
}

void freqCtrl::keyPressEvent(QKeyEvent *event)
{
    // call base class if dont over ride key
    bool      fSkipMsg = false;
    qint64    tmp;

    // qDebug() <<event->key();

    switch (event->key())
    {
    case Qt::Key_0:
    case Qt::Key_1:
    case Qt::Key_2:
    case Qt::Key_3:
    case Qt::Key_4:
    case Qt::Key_5:
    case Qt::Key_6:
    case Qt::Key_7:
    case Qt::Key_8:
    case Qt::Key_9:
        if (m_ActiveEditDigit >= 0)
        {
            if (m_DigitInfo[m_ActiveEditDigit].editmode)
            {
                tmp = (m_freq / m_DigitInfo[m_ActiveEditDigit].weight) % 10;
                m_freq -= tmp * m_DigitInfo[m_ActiveEditDigit].weight;
                m_freq = m_freq + (event->key() - '0') *
                                      m_DigitInfo[m_ActiveEditDigit].weight;
                setFrequency(m_freq);
            }
        }
        moveCursorRight();
        fSkipMsg = true;
        break;
    case Qt::Key_Backspace:
    case Qt::Key_Left:
        if (m_ActiveEditDigit != -1)
        {
            moveCursorLeft();
            fSkipMsg = true;
        }
        break;
    case Qt::Key_Up:
        if (m_ActiveEditDigit != -1)
        {
            incFreq();
            fSkipMsg = true;
        }
        break;
    case Qt::Key_Down:
        if (m_ActiveEditDigit != -1)
        {
            decFreq();
            fSkipMsg = true;
        }
        break;
    case Qt::Key_Right:
        if (m_ActiveEditDigit != -1)
        {
            moveCursorRight();
            fSkipMsg = true;
        }
        break;
    case Qt::Key_Home:
        cursorHome();
        fSkipMsg = true;
        break;
    case Qt::Key_End:
        cursorEnd();
        fSkipMsg = true;
        break;
    default:
        break;
    }
    if (!fSkipMsg)
        QFrame::keyPressEvent(event);
}

void freqCtrl::drawBkGround(QPainter &Painter)
{
    QRect    rect(0, 0, width(), height());

    // qDebug() <<rect;
    int    cellwidth = 100 * rect.width() /
                    (100 * (m_NumDigits + m_NumDigitsForUnit) +
                     (m_NumSeps * SEPRATIO_N) / SEPRATIO_D);
    int    sepwidth = (SEPRATIO_N * cellwidth) / (100 * SEPRATIO_D);
    // qDebug() <<cellwidth <<sepwidth;

    // draw unit text
    if (m_Unit != FCTL_UNIT_NONE)
    {
        m_UnitsRect.setRect(rect.right() - 2 * cellwidth, rect.top(),
                            2 * cellwidth, rect.height());
        Painter.fillRect(m_UnitsRect, m_BkColor); // FIXME: not necessary?
        m_UnitsFont.setPixelSize((UNITS_SIZE_PERCENT * rect.height()) / 100);
        //m_UnitsFont.setFamily("Arial");
        Painter.setFont(m_UnitsFont);
        Painter.setPen(m_UnitsColor);
        Painter.drawText(m_UnitsRect, Qt::AlignHCenter | Qt::AlignVCenter,
                         m_UnitString);
    }

    // draw digits
    m_DigitFont.setPixelSize((DIGIT_SIZE_PERCENT * rect.height()) / 100);
    //m_DigitFont.setFamily("Arial");
    Painter.setFont(m_DigitFont);
    Painter.setPen(m_DigitColor);

    QChar    dgsep = gsep;       // digit group separator

    int     digpos = rect.right() - m_NumDigitsForUnit * cellwidth - 1; // starting digit x position
    for (int i = m_DigStart; i < m_NumDigits; i++)
    {
        if ((i > m_DigStart) && ((i % 3) == 0))
        {
            m_SepRect[i].setCoords(digpos - sepwidth,
                                   rect.top(),
                                   digpos,
                                   rect.bottom());
            Painter.fillRect(m_SepRect[i], m_BkColor);
            digpos -= sepwidth;
            if (m_Unit == FCTL_UNIT_NONE)
            {
                if (m_LeadZeroPos > i)
                    dgsep = dsep;
                else
                    dgsep = gsep;
            }
            else
            {
                if (i == m_DecPos)
                    dgsep = dsep;
                else
                    dgsep = gsep;
            }
            if( (i==m_NumDigits-1) && (m_DigitInfo[i].val==0) ) {
                Painter.drawText(m_SepRect[i], Qt::AlignHCenter | Qt::AlignVCenter,
                                                 " ");
            } else {
                // Only draw the digit separator if we are within a number.
                // This eliminates drawing a comma after the last (MSB) digit.
                Painter.drawText(m_SepRect[i], Qt::AlignHCenter | Qt::AlignVCenter,
                                 dgsep);
            }
        }
        else
        {
            m_SepRect[i].setCoords(0, 0, 0, 0);
        }
        m_DigitInfo[i].dQRect.setCoords(digpos - cellwidth, rect.top(),
                                        digpos, rect.bottom());
        digpos -= cellwidth;
    }
}

//  Draws just the Digits that have been modified
void freqCtrl::drawDigits(QPainter &Painter)
{
    Painter.setFont(m_DigitFont);
    m_FirstEditableDigit = m_DigStart;

    for (int i = m_DigStart; i < m_NumDigits; i++)
    {
        if (m_DigitInfo[i].incval == 0)
            m_FirstEditableDigit++;

        if (m_DigitInfo[i].modified || m_DigitInfo[i].editmode)
        {
            if (m_DigitInfo[i].editmode && m_DigitInfo[i].incval != 0)
                Painter.fillRect(m_DigitInfo[i].dQRect, m_HighlightColor);
            else
                Painter.fillRect(m_DigitInfo[i].dQRect, m_BkColor);

            if (i >= m_LeadZeroPos)
                Painter.setPen(m_InactiveColor);
            else
                Painter.setPen(m_DigitColor);

            if (m_freq < 0 && i == m_LeadZeroPos - 1 && m_DigitInfo[i].val == 0)
                Painter.drawText(m_DigitInfo[i].dQRect,
                                 Qt::AlignHCenter | Qt::AlignVCenter,
                                 QString("-0"));
            else
                Painter.drawText(m_DigitInfo[i].dQRect,
                                 Qt::AlignHCenter | Qt::AlignVCenter,
                                 QString().number(m_DigitInfo[i].val));
            m_DigitInfo[i].modified = false;
        }
    }
}

// Increment just the digit active in edit mode
void freqCtrl::incDigit()
{
    /** FIXME: no longer used? */
    int       tmp;
    qint64    tmpl;

    if (m_ActiveEditDigit >= 0)
    {
        if (m_DigitInfo[m_ActiveEditDigit].editmode)
        {
            if (m_DigitInfo[m_ActiveEditDigit].weight ==
                m_DigitInfo[m_ActiveEditDigit].incval)
            {
                // get the current digit value
                tmp =
                    (int)((m_freq / m_DigitInfo[m_ActiveEditDigit].weight) %
                           10);
                // set the current digit value to zero
                m_freq -= tmp * m_DigitInfo[m_ActiveEditDigit].weight;
                tmp++;
                if (tmp > 9)
                    tmp = 0;
                m_freq = m_freq + (qint64)tmp *
                                      m_DigitInfo[m_ActiveEditDigit].weight;
            }
            else
            {
                tmp =
                    (int)((m_freq / m_DigitInfo[m_ActiveEditDigit + 1].weight) %
                           10);
                tmpl = m_freq + m_DigitInfo[m_ActiveEditDigit].incval;
                if (tmp !=
                    (int)((tmpl / m_DigitInfo[m_ActiveEditDigit + 1].weight) %
                           10))
                {
                    tmpl -= m_DigitInfo[m_ActiveEditDigit + 1].weight;
                }
                m_freq = tmpl;
            }
            setFrequency(m_freq);
        }
    }
}

// Increment the frequency by this digit active in edit mode
void freqCtrl::incFreq()
{
    if (m_ActiveEditDigit >= 0)
    {
        if (m_DigitInfo[m_ActiveEditDigit].editmode)
        {
            m_freq += m_DigitInfo[m_ActiveEditDigit].incval;
            if (m_ResetLowerDigits)
            {
                /* Set digits below the active one to 0 */
                m_freq = m_freq - m_freq %
                                      m_DigitInfo[m_ActiveEditDigit].weight;
            }
            setFrequency(m_freq);
            m_LastEditDigit = m_ActiveEditDigit;
        }
    }
}

// Decrement the digit active in edit mode
void freqCtrl::decDigit()
{
    /** FIXME: no longer used? */
    int       tmp;
    qint64    tmpl;

    if (m_ActiveEditDigit >= 0)
    {
        if (m_DigitInfo[m_ActiveEditDigit].editmode)
        {
            if (m_DigitInfo[m_ActiveEditDigit].weight ==
                m_DigitInfo[m_ActiveEditDigit].incval)
            {
                // get the current digit value
                tmp =
                    (int)((m_freq / m_DigitInfo[m_ActiveEditDigit].weight) %
                           10);
                // set the current digit value to zero
                m_freq -= tmp * m_DigitInfo[m_ActiveEditDigit].weight;
                tmp--;
                if (tmp < 0)
                    tmp = 9;
                m_freq = m_freq + (qint64)tmp *
                                      m_DigitInfo[m_ActiveEditDigit].weight;
            }
            else
            {
                tmp =
                    (int)((m_freq / m_DigitInfo[m_ActiveEditDigit + 1].weight) %
                           10);
                tmpl = m_freq - m_DigitInfo[m_ActiveEditDigit].incval;
                if (tmp !=
                    (int)((tmpl / m_DigitInfo[m_ActiveEditDigit + 1].weight) %
                           10))
                {
                    tmpl += m_DigitInfo[m_ActiveEditDigit + 1].weight;
                }
                m_freq = tmpl;
            }
            setFrequency(m_freq);
        }
    }
}

// Decrement the frequency by this digit active in edit mode
void freqCtrl::decFreq()
{
    if (m_ActiveEditDigit >= 0)
    {
        if (m_DigitInfo[m_ActiveEditDigit].editmode)
        {
            m_freq -= m_DigitInfo[m_ActiveEditDigit].incval;
            if (m_ResetLowerDigits)
            {
                /* digits below the active one are reset to 0 */
                m_freq = m_freq - m_freq %
                                      m_DigitInfo[m_ActiveEditDigit].weight;
            }

            setFrequency(m_freq);
            m_LastEditDigit = m_ActiveEditDigit;
        }
    }
}

// Clear the selected digit and the digits below (i.e. set them to 0)
void freqCtrl::clearFreq()
{
    if (m_ActiveEditDigit >= 0)
    {
        if (m_DigitInfo[m_ActiveEditDigit].editmode)
        {
            m_freq -= m_DigitInfo[m_ActiveEditDigit].val *
                      m_DigitInfo[m_ActiveEditDigit].incval;

            /* digits below the active one are reset to 0 */
            m_freq -= m_freq % m_DigitInfo[m_ActiveEditDigit].weight;

            setFrequency(m_freq);
            m_LastEditDigit = m_ActiveEditDigit;
        }
    }
}

//  Cursor move routines for arrow key editing
void freqCtrl::moveCursorLeft()
{
    if ((m_ActiveEditDigit >= 0) && (m_ActiveEditDigit < m_NumDigits - 1))
    {
        setActiveDigit(m_ActiveEditDigit + 1);
    }
}

void freqCtrl::moveCursorRight()
{
    if (m_ActiveEditDigit > m_FirstEditableDigit)
    {
        setActiveDigit(m_ActiveEditDigit - 1);
    }
}

void freqCtrl::cursorHome()
{
    if (m_ActiveEditDigit >= 0)
    {
        setActiveDigit(m_NumDigits - 1);
    }
}

void freqCtrl::cursorEnd()
{
    if (m_ActiveEditDigit > 0)
    {
        setActiveDigit(m_FirstEditableDigit);
    }
}

void freqCtrl::setFrequencyFocus()
{
    // Select last digit or 5th digit (100s of kHz), whatever is bigger.
    int position = std::max(int(log10(m_freq)), 5);

    if (!hasFocus()) {
        setFocus(Qt::ShortcutFocusReason);
    }

    setActiveDigit(position);
}
