#include "cwsender.h"
#include "ui_cwsender.h"

#include "logcategories.h"

cwSender::cwSender(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::cwSender)
{
    ui->setupUi(this);
    this->setWindowTitle("CW Sender");
    ui->textToSendEdit->setFocus();
    QFont f = QFont("Monospace");
    f.setStyleHint(QFont::TypeWriter);
    ui->textToSendEdit->setFont(f);
    ui->transcriptText->setFont(f);
    ui->textToSendEdit->setFocus();
    ui->statusbar->setToolTipDuration(3000);
    this->setToolTipDuration(3000);
    connect(ui->textToSendEdit->lineEdit(), &QLineEdit::textEdited, this, &cwSender::textChanged);
    this->setObjectName("CW Sender");
    queue = cachingQueue::getInstance();

    connect(this, &cwSender::sendCW, queue, [=](const QString &cwMessage) {
        queue->add(priorityImmediate,queueItem(funcSendCW,QVariant::fromValue<QString>(cwMessage)));
    });

    connect(this, &cwSender::stopCW, queue, [=]() {
        queue->add(priorityImmediate,queueItem(funcSendCW,QVariant::fromValue<QString>(QChar(0xff))));
    });

    connect(this, &cwSender::setBreakInMode, queue, [=](const quint8 &bmode) {
        queue->add(priorityImmediate,queueItem(funcBreakIn,QVariant::fromValue<uchar>(bmode)));
    });


    connect(this, &cwSender::setDashRatio, queue, [=](const quint8& ratio) {
        queue->addUnique(priorityImmediate,queueItem(funcDashRatio,QVariant::fromValue<uchar>(ratio)));
    });

    connect(this, &cwSender::setPitch, queue, [=](const quint16& pitch) {
        queue->addUnique(priorityImmediate,queueItem(funcSendCW,QVariant::fromValue<ushort>(pitch)));
    });

    connect(this, &cwSender::getCWSettings, queue, [=]() {
        queue->add(priorityImmediate,funcKeySpeed);
        queue->add(priorityImmediate,funcBreakIn);
        queue->add(priorityImmediate,funcCwPitch);
        queue->add(priorityImmediate,funcDashRatio);
    });

}

cwSender::~cwSender()
{
    qDebug(logCW()) << "Running CW Sender destructor.";

    if (toneThread != Q_NULLPTR) {
        toneThread->quit();
        toneThread->wait();
        toneThread = Q_NULLPTR;
        tone = Q_NULLPTR;
        /* Finally disconnect all connections */
        //for (auto conn: connections)
        //{
        //    disconnect(conn);
        //}
        //connections.clear();
    }

    delete ui;
}

void cwSender::showEvent(QShowEvent *event)
{
    (void)event;
    //emit getCWSettings();
    QMainWindow::showEvent(event);
}

void cwSender::handleKeySpeed(quint8 wpm)
{
    if (wpm != ui->wpmSpin->value() && (wpm >= ui->wpmSpin->minimum()) && (wpm <= ui->wpmSpin->maximum()))
    {
        qDebug(logCW()) << "Received new key speed" << wpm;
        ui->wpmSpin->blockSignals(true);
        QMetaObject::invokeMethod(ui->wpmSpin, "setValue", Qt::QueuedConnection, Q_ARG(int, wpm));
        ui->wpmSpin->blockSignals(false);
#if (QT_VERSION >= QT_VERSION_CHECK(5,10,0))
        QMetaObject::invokeMethod(tone, [=]() {
            tone->setSpeed(wpm);
        }, Qt::QueuedConnection);
#else
        emit setKeySpeed((quint8)wpm);
#endif

    }
}

void cwSender::handleDashRatio(quint8 ratio)
{
    double calc = double(ratio/10);
    if (calc != ui->dashSpin->value() && (calc >= ui->dashSpin->minimum()) && (ratio <= ui->dashSpin->maximum()))
    {
        ui->dashSpin->blockSignals(true);
        QMetaObject::invokeMethod(ui->dashSpin, "setValue", Qt::QueuedConnection, Q_ARG(double, calc));
        ui->dashSpin->blockSignals(false);
#if (QT_VERSION >= QT_VERSION_CHECK(5,10,0))
        QMetaObject::invokeMethod(tone, [=]() {
            tone->setRatio(ratio);
        }, Qt::QueuedConnection);
#else
        emit setDashRatio(ratio);
#endif
    }
}

void cwSender::handlePitch(quint16 cwPitch) {
    if (cwPitch != ui->pitchSpin->value() && cwPitch >= ui->pitchSpin->minimum() && cwPitch <= ui->pitchSpin->maximum())
    {
        ui->pitchSpin->blockSignals(true);
        QMetaObject::invokeMethod(ui->pitchSpin, "setValue", Qt::QueuedConnection, Q_ARG(int, cwPitch));
        ui->pitchSpin->blockSignals(false);
#if (QT_VERSION >= QT_VERSION_CHECK(5,10,0))
        QMetaObject::invokeMethod(tone, [=]() {
            tone->setFrequency(cwPitch);
        }, Qt::QueuedConnection);
#else
        emit setPitch(cwPitch);
#endif
    }
}

void cwSender::handleBreakInMode(quint8 b)
{
    if(b < 3)
    {
        ui->breakinCombo->blockSignals(true);
        ui->breakinCombo->setCurrentIndex(b);
        ui->breakinCombo->blockSignals(false);
    }
}

void cwSender::handleCurrentModeUpdate(rigMode_t mode)
{
    this->currentMode = mode;
    if( this->currentMode!=modeCW && this->currentMode!=modeCW_R )
    {
        ui->statusbar->showMessage("Note: Mode needs to be set to CW or CW-R to send CW.", 3000);
    }
}

void cwSender::textChanged(QString text)
{
    if (ui->sendImmediateChk->isChecked() && text.size() && text.back() == ' ')
    {
        if( (this->currentMode != modeCW) && (this->currentMode != modeCW_R) )
        {
            ui->statusbar->showMessage("Note: Mode needs to be set to CW or CW-R to send CW.", 3000);
        } else {
            int toSend = text.mid(0, 30).size();
            if (toSend > 0) {
                ui->textToSendEdit->clearEditText();
                ui->transcriptText->moveCursor(QTextCursor::End);
                ui->transcriptText->insertPlainText(text.mid(0, 30).toUpper());
                ui->transcriptText->moveCursor(QTextCursor::End);
                emit sendCW(text.mid(0, 30));
            }
        }
    }
}

void cwSender::on_sendBtn_clicked()
{
    if( (ui->textToSendEdit->currentText().length() > 0) &&
        (ui->textToSendEdit->currentText().length() <= 30) )
    {
        if( (this->currentMode != modeCW) && (this->currentMode != modeCW_R) )
        {
            ui->statusbar->showMessage("Note: Mode needs to be set to CW or CW-R to send CW.", 3000);
        } else {
            QString text = ui->textToSendEdit->currentText();

            ui->transcriptText->moveCursor(QTextCursor::End);
            ui->transcriptText->insertPlainText(ui->textToSendEdit->currentText().toUpper()+"\n");
            ui->transcriptText->moveCursor(QTextCursor::End);
            if (!ui->sendImmediateChk->isChecked())
            {
                ui->textToSendEdit->addItem(ui->textToSendEdit->currentText());
                if (ui->textToSendEdit->count() > 5) {
                    ui->textToSendEdit->removeItem(0);
                }
                ui->textToSendEdit->setCurrentIndex(-1);
            } else {
                ui->textToSendEdit->clearEditText();
                ui->textToSendEdit->clear();
            }

            ui->textToSendEdit->setFocus();
            ui->statusbar->showMessage("Sending CW", 3000);
            emit sendCW(text);
        }
    }

}

void cwSender::on_stopBtn_clicked()
{
    emit stopCW();
    ui->textToSendEdit->setFocus();
    ui->statusbar->showMessage("Stopping CW transmission.", 3000);
}

//void cwSender::on_textToSendEdit_returnPressed()
//{
//    on_sendBtn_clicked();
//}

void cwSender::on_breakinCombo_activated(int brkmode)
{
    // 0 = off, 1 = semi, 2 = full
    emit setBreakInMode((quint8)brkmode);
    ui->textToSendEdit->setFocus();
}

void cwSender::on_wpmSpin_valueChanged(int wpm)
{
    queue->addUnique(priorityImmediate,queueItem(funcKeySpeed,QVariant::fromValue<ushort>(wpm)));
}

void cwSender::on_dashSpin_valueChanged(double ratio)
{
    queue->addUnique(priorityImmediate,queueItem(funcDashRatio,QVariant::fromValue<uchar>(ratio*10.0)));
}

void cwSender::on_pitchSpin_valueChanged(int arg1)
{
    queue->addUnique(priorityImmediate,queueItem(funcCwPitch,QVariant::fromValue<ushort>(arg1)));
}

void cwSender::on_macro1btn_clicked()
{
    processMacroButton(1, ui->macro1btn);
}

void cwSender::on_macro2btn_clicked()
{
    processMacroButton(2, ui->macro2btn);
}

void cwSender::on_macro3btn_clicked()
{
    processMacroButton(3, ui->macro3btn);
}

void cwSender::on_macro4btn_clicked()
{
    processMacroButton(4, ui->macro4btn);
}

void cwSender::on_macro5btn_clicked()
{
    processMacroButton(5, ui->macro5btn);
}

void cwSender::on_macro6btn_clicked()
{
    processMacroButton(6, ui->macro6btn);
}

void cwSender::on_macro7btn_clicked()
{
    processMacroButton(7, ui->macro7btn);
}

void cwSender::on_macro8btn_clicked()
{
    processMacroButton(8, ui->macro8btn);
}

void cwSender::on_macro9btn_clicked()
{
    processMacroButton(9, ui->macro9btn);
}

void cwSender::on_macro10btn_clicked()
{
    processMacroButton(10, ui->macro10btn);
}

void cwSender::on_sidetoneEnableChk_clicked(bool clicked)
{
    ui->sidetoneLevelSlider->setEnabled(clicked);
    if (clicked && toneThread == Q_NULLPTR)
    {
        toneThread = new QThread(this);
        toneThread->setObjectName("sidetone()");

        tone = new cwSidetone(sidetoneLevel, ui->wpmSpin->value(),ui->pitchSpin->value(),ui->dashSpin->value(),this);
        tone->moveToThread(toneThread);
        toneThread->start();

        connect(this,SIGNAL(initTone()),tone,SLOT(init()));
        connect(this,SIGNAL(sidetone(QString)),tone,SLOT(send(QString)));

        connect(toneThread, &QThread::finished, tone,
            [=]() { tone->deleteLater(); });

        connect(this, &cwSender::sendCW, tone, [=](const QString& text) {
               emit sidetone(text);
        });

        connect(this, &cwSender::setKeySpeed, tone,
                [=](const quint8& wpm) { tone->setSpeed(wpm); });

        connect(this, &cwSender::setDashRatio, tone,
                [=](const quint8& ratio) { tone->setRatio(ratio); });

        connect(this, &cwSender::setPitch, tone,
                [=](const quint16& pitch) { tone->setFrequency(pitch); });

        connect(this, &cwSender::setLevel, tone,
                [=](const quint8& level) { tone->setLevel(level); });

        connect(this, &cwSender::stopCW, tone,
                [=]() { tone->stopSending(); });


        emit initTone();

    } else if (!clicked && toneThread != Q_NULLPTR) {
        /* disconnect all connections */
        toneThread->quit();
        toneThread->wait();
        toneThread = Q_NULLPTR;
        tone = Q_NULLPTR;
/*
        for (auto conn: connections)
        {
            disconnect(conn);
        }
        connections.clear();
*/
    }
}

void cwSender::on_sidetoneLevelSlider_valueChanged(int val)
{
    sidetoneLevel = val;
    emit setLevel(val);
}


void cwSender::processMacroButton(int buttonNumber, QPushButton *btn)
{
    if(ui->macroEditChk->isChecked())
    {
        editMacroButton(buttonNumber, btn);
    } else {
        runMacroButton(buttonNumber);
    }
    ui->textToSendEdit->setFocus();
}

void cwSender::runMacroButton(int buttonNumber)
{
    if(macroText[buttonNumber].isEmpty())
        return;   

    if( (this->currentMode==modeCW) || (this->currentMode==modeCW_R) )
    {
        QString outText;
        if(macroText[buttonNumber].contains("%1"))
        {
            outText = macroText[buttonNumber].arg(sequenceNumber, 3, 10, QChar('0'));
            sequenceNumber++;
            ui->sequenceSpin->blockSignals(true);
            QMetaObject::invokeMethod(ui->sequenceSpin, "setValue", Qt::QueuedConnection, Q_ARG(int, sequenceNumber));
            ui->sequenceSpin->blockSignals(false);
        } else {
            outText = macroText[buttonNumber];
        }

        if (ui->cutNumbersChk->isChecked())
        {
            outText.replace("0", "T");
            outText.replace("9", "N");
        }

        ui->transcriptText->moveCursor(QTextCursor::End);
        ui->transcriptText->insertPlainText(outText.toUpper()+"\n");
        ui->transcriptText->moveCursor(QTextCursor::End);

        ui->statusbar->showMessage(QString("Sending CW macro %1").arg(buttonNumber));

        for (int i = 0; i < outText.size(); i = i + 30) {
            emit sendCW(outText.mid(i,30));
        }

        ui->textToSendEdit->setFocus();
    } else {
        ui->statusbar->showMessage("Note: Mode needs to be set to CW or CW-R to send CW.");
    }
}

void cwSender::editMacroButton(int buttonNumber, QPushButton* btn)
{
    bool ok;
    QString promptFirst = QString("Please enter the text for macro %1,\n"
        "up to 60 characters.\n").arg(buttonNumber);
    QString promptSecond = QString("You may use \"%1\" to insert a sequence number.");
    QString prompt = promptFirst+promptSecond;

    QString newMacroText = QInputDialog::getText(this, "Macro Edit",
                                                 prompt,
                                                 QLineEdit::Normal, macroText[buttonNumber], &ok).toUpper();
    if(!ok)
        return;

    if (newMacroText.length() > 60)
    {
        QMessageBox msgBox;
        msgBox.setText(QString("The text entered was too long \n"
            "(max length is 60 characters).\n"
            "Your input was %1 characters.").arg(newMacroText.length()));
        msgBox.exec();
        this->raise();
        return;
    }

    macroText[buttonNumber] = newMacroText;
    setMacroButtonText(newMacroText, btn);
}

void cwSender::setMacroButtonText(QString btnText, QPushButton *btn)
{
    if(btn==Q_NULLPTR)
        return;
    if(btnText.isEmpty())
        return;

    QString shortBtnName;
    if(btnText.length() <= 8)
    {
        shortBtnName = btnText;
    } else {
        shortBtnName = btnText.left(7);
        shortBtnName.append("…");
    }
    btn->setText(shortBtnName);
}

void cwSender::on_sequenceSpin_valueChanged(int newSeq)
{
    sequenceNumber = newSeq;
    ui->textToSendEdit->setFocus();
}

bool cwSender::getCutNumbers()
{
    return ui->cutNumbersChk->isChecked();
}

bool cwSender::getSendImmediate()
{
    return ui->sendImmediateChk->isChecked();
}

bool cwSender::getSidetoneEnable()
{
    return ui->sidetoneEnableChk->isChecked();
}

int cwSender::getSidetoneLevel()
{
    return ui->sidetoneLevelSlider->value();
}

void cwSender::setCutNumbers(bool val)
{
    ui->cutNumbersChk->setChecked(val);
}

void cwSender::setSendImmediate(bool val)
{
    ui->sendImmediateChk->setChecked(val);
}

void cwSender::setSidetoneEnable(bool val)
{
    ui->sidetoneEnableChk->setChecked(val);
    on_sidetoneEnableChk_clicked(val);
}

void cwSender::setSidetoneLevel(int val)
{
    QMetaObject::invokeMethod(ui->sidetoneLevelSlider, "setValue", Qt::QueuedConnection, Q_ARG(int, val));
}

QStringList cwSender::getMacroText()
{
    // This is for preference saving:
    QStringList mlist;
    for(int i=1; i < 11; i++)
    {
        mlist << macroText[i];
    }
    return mlist;
}

void cwSender::setMacroText(QStringList macros)
{
    if(macros.length() != 10)
    {
        qCritical(logCW()) << "Macro list must be exactly 10. Rejecting macro text load.";
        return;
    }

    for(int i=0; i < 10; i++)
    {
        macroText[i+1] = macros.at(i);
    }

    setMacroButtonText(macroText[1], ui->macro1btn);
    setMacroButtonText(macroText[2], ui->macro2btn);
    setMacroButtonText(macroText[3], ui->macro3btn);
    setMacroButtonText(macroText[4], ui->macro4btn);
    setMacroButtonText(macroText[5], ui->macro5btn);
    setMacroButtonText(macroText[6], ui->macro6btn);
    setMacroButtonText(macroText[7], ui->macro7btn);
    setMacroButtonText(macroText[8], ui->macro8btn);
    setMacroButtonText(macroText[9], ui->macro9btn);
    setMacroButtonText(macroText[10], ui->macro10btn);
}
