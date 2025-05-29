#include "repeatersetup.h"
#include "ui_repeatersetup.h"

repeaterSetup::repeaterSetup(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::repeaterSetup)
{
    ui->setupUi(this);

    this->setObjectName("RepeaterSetup");
    queue = cachingQueue::getInstance();
    connect(queue, SIGNAL(rigCapsUpdated(rigCapabilities*)), this, SLOT(receiveRigCaps(rigCapabilities*)));

    ui->autoTrackLiveBtn->setEnabled(false); // until we set split enabled.
    ui->warningFMLabel->setVisible(false);
    // populate the CTCSS combo box:
    populateTones();

    // populate the DCS combo box:
    populateDTCS();
}

repeaterSetup::~repeaterSetup()
{
    // Trying this for more consistent destruction
    delete ui;

}

void repeaterSetup::populateTones()
{
    ui->rptToneCombo->addItem("67.0", quint16(670));
    ui->rptToneCombo->addItem("69.3", quint16(693));
    ui->rptToneCombo->addItem("71.9", quint16(719));
    ui->rptToneCombo->addItem("74.4", quint16(744));
    ui->rptToneCombo->addItem("77.0", quint16(770));
    ui->rptToneCombo->addItem("79.7", quint16(797));
    ui->rptToneCombo->addItem("82.5", quint16(825));
    ui->rptToneCombo->addItem("85.4", quint16(854));
    ui->rptToneCombo->addItem("88.5", quint16(885));
    ui->rptToneCombo->addItem("91.5", quint16(915));
    ui->rptToneCombo->addItem("94.8", quint16(948));
    ui->rptToneCombo->addItem("97.4", quint16(974));
    ui->rptToneCombo->addItem("100.0", quint16(1000));
    ui->rptToneCombo->addItem("103.5", quint16(1035));
    ui->rptToneCombo->addItem("107.2", quint16(1072));
    ui->rptToneCombo->addItem("110.9", quint16(1109));
    ui->rptToneCombo->addItem("114.8", quint16(1148));
    ui->rptToneCombo->addItem("118.8", quint16(1188));
    ui->rptToneCombo->addItem("123.0", quint16(1230));
    ui->rptToneCombo->addItem("127.3", quint16(1273));
    ui->rptToneCombo->addItem("131.8", quint16(1318));
    ui->rptToneCombo->addItem("136.5", quint16(1365));
    ui->rptToneCombo->addItem("141.3", quint16(1413));
    ui->rptToneCombo->addItem("146.2", quint16(1462));
    ui->rptToneCombo->addItem("151.4", quint16(1514));
    ui->rptToneCombo->addItem("156.7", quint16(1567));
    ui->rptToneCombo->addItem("159.8", quint16(1598));
    ui->rptToneCombo->addItem("162.2", quint16(1622));
    ui->rptToneCombo->addItem("165.5", quint16(1655));
    ui->rptToneCombo->addItem("167.9", quint16(1679));
    ui->rptToneCombo->addItem("171.3", quint16(1713));
    ui->rptToneCombo->addItem("173.8", quint16(1738));
    ui->rptToneCombo->addItem("177.3", quint16(1773));
    ui->rptToneCombo->addItem("179.9", quint16(1799));
    ui->rptToneCombo->addItem("183.5", quint16(1835));
    ui->rptToneCombo->addItem("186.2", quint16(1862));
    ui->rptToneCombo->addItem("189.9", quint16(1899));
    ui->rptToneCombo->addItem("192.8", quint16(1928));
    ui->rptToneCombo->addItem("196.6", quint16(1966));
    ui->rptToneCombo->addItem("199.5", quint16(1995));
    ui->rptToneCombo->addItem("203.5", quint16(2035));
    ui->rptToneCombo->addItem("206.5", quint16(2065));
    ui->rptToneCombo->addItem("210.7", quint16(2107));
    ui->rptToneCombo->addItem("218.1", quint16(2181));
    ui->rptToneCombo->addItem("225.7", quint16(2257));
    ui->rptToneCombo->addItem("229.1", quint16(2291));
    ui->rptToneCombo->addItem("233.6", quint16(2336));
    ui->rptToneCombo->addItem("241.8", quint16(2418));
    ui->rptToneCombo->addItem("250.3", quint16(2503));
    ui->rptToneCombo->addItem("254.1", quint16(2541));
}

void repeaterSetup::populateDTCS()
{
    ui->rptDTCSCombo->addItem("023", quint16(23));
    ui->rptDTCSCombo->addItem("025", quint16(25));
    ui->rptDTCSCombo->addItem("026", quint16(26));
    ui->rptDTCSCombo->addItem("031", quint16(31));
    ui->rptDTCSCombo->addItem("032", quint16(32));
    ui->rptDTCSCombo->addItem("036", quint16(36));
    ui->rptDTCSCombo->addItem("043", quint16(43));
    ui->rptDTCSCombo->addItem("047", quint16(47));
    ui->rptDTCSCombo->addItem("051", quint16(51));
    ui->rptDTCSCombo->addItem("053", quint16(53));
    ui->rptDTCSCombo->addItem("054", quint16(54));
    ui->rptDTCSCombo->addItem("065", quint16(65));
    ui->rptDTCSCombo->addItem("071", quint16(71));
    ui->rptDTCSCombo->addItem("072", quint16(72));
    ui->rptDTCSCombo->addItem("073", quint16(73));
    ui->rptDTCSCombo->addItem("074", quint16(74));

    ui->rptDTCSCombo->addItem("114", quint16(114));
    ui->rptDTCSCombo->addItem("115", quint16(115));
    ui->rptDTCSCombo->addItem("116", quint16(116));
    ui->rptDTCSCombo->addItem("122", quint16(122));
    ui->rptDTCSCombo->addItem("125", quint16(125));
    ui->rptDTCSCombo->addItem("131", quint16(131));
    ui->rptDTCSCombo->addItem("132", quint16(132));
    ui->rptDTCSCombo->addItem("134", quint16(134));
    ui->rptDTCSCombo->addItem("143", quint16(143));
    ui->rptDTCSCombo->addItem("145", quint16(145));
    ui->rptDTCSCombo->addItem("152", quint16(152));
    ui->rptDTCSCombo->addItem("155", quint16(155));
    ui->rptDTCSCombo->addItem("156", quint16(156));
    ui->rptDTCSCombo->addItem("162", quint16(162));
    ui->rptDTCSCombo->addItem("165", quint16(165));
    ui->rptDTCSCombo->addItem("172", quint16(172));
    ui->rptDTCSCombo->addItem("174", quint16(174));

    ui->rptDTCSCombo->addItem("205", quint16(205));
    ui->rptDTCSCombo->addItem("212", quint16(212));
    ui->rptDTCSCombo->addItem("223", quint16(223));
    ui->rptDTCSCombo->addItem("225", quint16(225));
    ui->rptDTCSCombo->addItem("226", quint16(226));
    ui->rptDTCSCombo->addItem("243", quint16(243));
    ui->rptDTCSCombo->addItem("244", quint16(244));
    ui->rptDTCSCombo->addItem("245", quint16(245));
    ui->rptDTCSCombo->addItem("246", quint16(246));
    ui->rptDTCSCombo->addItem("251", quint16(251));
    ui->rptDTCSCombo->addItem("252", quint16(252));
    ui->rptDTCSCombo->addItem("255", quint16(255));
    ui->rptDTCSCombo->addItem("261", quint16(261));
    ui->rptDTCSCombo->addItem("263", quint16(263));
    ui->rptDTCSCombo->addItem("265", quint16(265));
    ui->rptDTCSCombo->addItem("266", quint16(266));
    ui->rptDTCSCombo->addItem("271", quint16(271));
    ui->rptDTCSCombo->addItem("274", quint16(274));

    ui->rptDTCSCombo->addItem("306", quint16(306));
    ui->rptDTCSCombo->addItem("311", quint16(311));
    ui->rptDTCSCombo->addItem("315", quint16(315));
    ui->rptDTCSCombo->addItem("325", quint16(325));
    ui->rptDTCSCombo->addItem("331", quint16(331));
    ui->rptDTCSCombo->addItem("332", quint16(332));
    ui->rptDTCSCombo->addItem("343", quint16(343));
    ui->rptDTCSCombo->addItem("346", quint16(346));
    ui->rptDTCSCombo->addItem("351", quint16(351));
    ui->rptDTCSCombo->addItem("356", quint16(356));
    ui->rptDTCSCombo->addItem("364", quint16(364));
    ui->rptDTCSCombo->addItem("365", quint16(365));
    ui->rptDTCSCombo->addItem("371", quint16(371));

    ui->rptDTCSCombo->addItem("411", quint16(411));
    ui->rptDTCSCombo->addItem("412", quint16(412));
    ui->rptDTCSCombo->addItem("413", quint16(413));
    ui->rptDTCSCombo->addItem("423", quint16(423));
    ui->rptDTCSCombo->addItem("431", quint16(431));
    ui->rptDTCSCombo->addItem("432", quint16(432));
    ui->rptDTCSCombo->addItem("445", quint16(445));
    ui->rptDTCSCombo->addItem("446", quint16(446));
    ui->rptDTCSCombo->addItem("452", quint16(452));
    ui->rptDTCSCombo->addItem("454", quint16(454));
    ui->rptDTCSCombo->addItem("455", quint16(455));
    ui->rptDTCSCombo->addItem("462", quint16(462));
    ui->rptDTCSCombo->addItem("464", quint16(464));
    ui->rptDTCSCombo->addItem("465", quint16(465));
    ui->rptDTCSCombo->addItem("466", quint16(466));

    ui->rptDTCSCombo->addItem("503", quint16(503));
    ui->rptDTCSCombo->addItem("506", quint16(506));
    ui->rptDTCSCombo->addItem("516", quint16(516));
    ui->rptDTCSCombo->addItem("523", quint16(512));
    ui->rptDTCSCombo->addItem("526", quint16(526));
    ui->rptDTCSCombo->addItem("532", quint16(532));
    ui->rptDTCSCombo->addItem("546", quint16(546));
    ui->rptDTCSCombo->addItem("565", quint16(565));

    ui->rptDTCSCombo->addItem("606", quint16(606));
    ui->rptDTCSCombo->addItem("612", quint16(612));
    ui->rptDTCSCombo->addItem("624", quint16(624));
    ui->rptDTCSCombo->addItem("627", quint16(627));
    ui->rptDTCSCombo->addItem("631", quint16(631));
    ui->rptDTCSCombo->addItem("632", quint16(632));
    ui->rptDTCSCombo->addItem("654", quint16(654));
    ui->rptDTCSCombo->addItem("662", quint16(662));
    ui->rptDTCSCombo->addItem("664", quint16(664));

    ui->rptDTCSCombo->addItem("703", quint16(703));
    ui->rptDTCSCombo->addItem("712", quint16(712));
    ui->rptDTCSCombo->addItem("723", quint16(723));
    ui->rptDTCSCombo->addItem("731", quint16(731));
    ui->rptDTCSCombo->addItem("732", quint16(732));
    ui->rptDTCSCombo->addItem("734", quint16(734));
    ui->rptDTCSCombo->addItem("746", quint16(746));
    ui->rptDTCSCombo->addItem("754", quint16(754));
}

void repeaterSetup::receiveDuplexMode(duplexMode_t dm)
{
    currentdm = dm;
    ui->splitEnableChk->blockSignals(true);
    switch(dm)
    {
        case dmSplitOff:
            ui->splitOffBtn->setChecked(true);
            ui->autoTrackLiveBtn->setDisabled(true);
            break;
        case dmSplitOn:
            ui->splitEnableChk->setChecked(true);
            ui->rptSimplexBtn->setChecked(false);
            ui->rptDupPlusBtn->setChecked(false);
            ui->autoTrackLiveBtn->setEnabled(true);
            ui->rptDupMinusBtn->setChecked(false);
            break;
        case dmSimplex:
            ui->rptSimplexBtn->setChecked(true);
            //ui->splitEnableChk->setChecked(false);
            ui->autoTrackLiveBtn->setDisabled(true);
            break;
        case dmDupPlus:
            ui->rptDupPlusBtn->setChecked(true);
            //ui->splitEnableChk->setChecked(false);
            ui->autoTrackLiveBtn->setDisabled(true);
            break;
        case dmDupMinus:
            ui->rptDupMinusBtn->setChecked(true);
            //ui->splitEnableChk->setChecked(false);
            ui->autoTrackLiveBtn->setDisabled(true);
            break;
        default:
            qDebug() << "Did not understand duplex/split/repeater value of " << (quint8)dm;
            break;
    }
    ui->splitEnableChk->blockSignals(false);
}

void repeaterSetup::handleRptAccessMode(rptAccessTxRx_t tmode)
{
    // ratrXY
    // X = Transmit (T)one or (N)one or (D)CS
    // Y = Receive (T)sql or (N)one or (D)CS
    switch(tmode)
    {
    case ratrNN:
        ui->toneNone->setChecked(true);
        break;
    case ratrTT:
    case ratrNT:
        ui->toneTSQL->setChecked(true);
        break;
    case ratrTN:
        ui->toneTone->setChecked(true);
        break;
    case ratrDD:
        ui->toneDTCS->setChecked(true);
        break;
    case ratrTONEoff:
        ui->toneTone->setChecked(false);
        break;
    case ratrTONEon:
        ui->toneTone->setChecked(true);
        break;
    case ratrTSQLoff:
        ui->toneTSQL->setChecked(false);
        break;
    case ratrTSQLon:
        ui->toneTSQL->setChecked(true);
        break;
    default:
        break;
    }
    if( !ui->toneTSQL->isChecked() && !ui->toneTone->isChecked() && !ui->toneDTCS->isChecked())
    {
        ui->toneNone->setChecked(true);
    }
}

void repeaterSetup::handleTone(quint16 tone)
{
    qDebug(logRptr()) << "Handling incoming tone request of tone = " << tone;
    int tindex = ui->rptToneCombo->findData(tone);
    ui->rptToneCombo->setCurrentIndex(tindex);
}

void repeaterSetup::handleTSQL(quint16 tsql)
{
    // TODO: Consider a second combo box for the TSQL
    qDebug(logRptr()) << "Handling incoming TSQL request of tone = " << tsql;
    int tindex = ui->rptToneCombo->findData(tsql);
    ui->rptToneCombo->setCurrentIndex(tindex);
}

void repeaterSetup::handleDTCS(quint16 dcode, bool tinv, bool rinv)
{
    qDebug(logRptr()) << "Handling incoming DTCS request of dcode = " << dcode;
    int dindex = ui->rptDTCSCombo->findData(dcode);
    ui->rptDTCSCombo->setCurrentIndex(dindex);
    ui->rptDTCSInvertTx->setChecked(tinv);
    ui->rptDTCSInvertRx->setChecked(rinv);
}

void repeaterSetup::handleUpdateCurrentMainFrequency(freqt mainfreq)
{
    if(mainfreq.VFO == inactiveVFO)
        return;

    haveReceivedFrequency = true;
    if(amTransmitting)
        return;

    // Track if autoTrack enabled, split enabled, and there's a split defined.
    if(ui->autoTrackLiveBtn->isChecked() && (currentdm == dmSplitOn) && !ui->splitOffsetEdit->text().isEmpty())
    {
        if(currentMainFrequency.Hz != mainfreq.Hz)
        {
            this->currentMainFrequency = mainfreq;
            if(!ui->splitTransmitFreqEdit->hasFocus())
            {
                if(usedPlusSplit)
                {
                    on_splitPlusButton_clicked();
                } else {
                    on_splitMinusBtn_clicked();
                }
            }
        }
        if(ui->setSplitRptrToneChk->isChecked())
        {
            // TODO, not really needed if the op
            // just sets the tone when needed, as it will do both bands.
        }
    }
    this->currentMainFrequency = mainfreq;
}

void repeaterSetup::handleUpdateCurrentMainMode(modeInfo m)
{
    // Used to set the secondary VFO to the same mode
    // (generally FM)
    // NB: We don't accept values during transmit as they
    // may represent the inactive VFO
    if(!amTransmitting)
    {
        this->currentModeMain = m;
        this->modeTransmitVFO = m;
        this->modeTransmitVFO.VFO = inactiveVFO;
        if(m.mk == modeFM)
            ui->warningFMLabel->setVisible(false);
        else
            ui->warningFMLabel->setVisible(true);
    }
}

void repeaterSetup::handleRptOffsetFrequency(freqt f)
{
    // Called when a new offset is available from the radio.
    QString offsetstr = QString::number(f.Hz / double(1E6), 'f', 4);

    if(!ui->rptrOffsetEdit->hasFocus())
    {
        ui->rptrOffsetEdit->setText(offsetstr);
        currentOffset = f;
    }
}

void repeaterSetup::handleTransmitStatus(bool amTransmitting)
{
    this->amTransmitting = amTransmitting;
}

void repeaterSetup::showEvent(QShowEvent *event)
{
    if(haveRig) {
        queue->add(priorityImmediate,funcSplitStatus,false,false);
        if(rigCaps->commands.contains(funcToneSquelchType))
            queue->add(priorityImmediate,funcReadFreqOffset,false,false);
        QMainWindow::showEvent(event);
    }
    (void)event;
}

void repeaterSetup::on_splitEnableChk_clicked()
{
    queue->add(priorityImmediate,queueItem(funcSplitStatus,QVariant::fromValue<duplexMode_t>(dmSplitOn),false));
    ui->autoTrackLiveBtn->setEnabled(true);

    if(ui->autoTrackLiveBtn->isChecked() && !ui->splitOffsetEdit->text().isEmpty())
    {
        if(usedPlusSplit)
        {
            on_splitPlusButton_clicked();
        } else {
            on_splitMinusBtn_clicked();
        }
    }
}

void repeaterSetup::on_splitOffBtn_clicked()
{
    queue->add(priorityImmediate,queueItem(funcSplitStatus,QVariant::fromValue<duplexMode_t>(dmSplitOff),false));
    ui->autoTrackLiveBtn->setDisabled(true);
}

void repeaterSetup::on_rptSimplexBtn_clicked()
{
    // Simplex
    //queue->add(priorityImmediate,queueItem(funcSplitStatus,QVariant::fromValue<duplexMode_t>(dmSplitOn),false));
    //if(rigCaps->commands.contains(funcToneSquelchType))
    //{
        //queue->add(priorityImmediate,queueItem(funcSplitStatus,QVariant::fromValue<duplexMode_t>(dmDupAutoOff),false));
        queue->add(priorityImmediate,queueItem(funcSplitStatus,QVariant::fromValue<duplexMode_t>(dmSimplex),false));
    //}
}

void repeaterSetup::on_rptDupPlusBtn_clicked()
{
    // DUP+
    //queue->add(priorityImmediate,queueItem(funcSplitStatus,QVariant::fromValue<duplexMode_t>(dmDupAutoOff),false));
    queue->add(priorityImmediate,queueItem(funcSplitStatus,QVariant::fromValue<duplexMode_t>(dmDupPlus),false));
}

void repeaterSetup::on_rptDupMinusBtn_clicked()
{
    // DUP-
    //queue->add(priorityImmediate,queueItem(funcSplitStatus,QVariant::fromValue<duplexMode_t>(dmDupAutoOff),false));
    queue->add(priorityImmediate,queueItem(funcSplitStatus,QVariant::fromValue<duplexMode_t>(dmDupMinus),false));
}

void repeaterSetup::on_rptAutoBtn_clicked()
{
    // Auto Rptr (enable this feature)
    // TODO: Hide an AutoOff button somewhere for non-US users
    queue->add(priorityImmediate,queueItem(funcSplitStatus,QVariant::fromValue<duplexMode_t>(dmDupAutoOn),false));
}

void repeaterSetup::on_rptToneCombo_activated(int tindex)
{
    quint16 tone=0;
    tone = (quint16)ui->rptToneCombo->itemData(tindex).toUInt();
    toneInfo rt;
    rt.tone = tone;
    bool updateSub = ui->setSplitRptrToneChk->isEnabled() && ui->setSplitRptrToneChk->isChecked();
    if(ui->toneTone->isChecked())
    {
        queue->add(priorityImmediate,queueItem(funcToneFreq,QVariant::fromValue<toneInfo>(rt),false, rt.useSecondaryVFO));
        if(updateSub)
        {
            rt.useSecondaryVFO = true;
            queue->add(priorityImmediate,queueItem(funcToneFreq,QVariant::fromValue<toneInfo>(rt),false, rt.useSecondaryVFO));
        }

    } else if (ui->toneTSQL->isChecked()) {
        queue->add(priorityImmediate,queueItem(funcToneFreq,QVariant::fromValue<toneInfo>(rt),false, rt.useSecondaryVFO));
        if(updateSub)
        {
            rt.useSecondaryVFO = true;
            queue->add(priorityImmediate,queueItem(funcToneFreq,QVariant::fromValue<toneInfo>(rt),false, rt.useSecondaryVFO));
        }
    }
}

void repeaterSetup::on_rptDTCSCombo_activated(int index)
{
    toneInfo tone;
    tone.tinv = ui->rptDTCSInvertTx->isChecked();
    tone.rinv = ui->rptDTCSInvertRx->isChecked();
    tone.tone = (quint16)ui->rptDTCSCombo->itemData(index).toUInt();
    queue->add(priorityImmediate,queueItem(funcDTCSCode,QVariant::fromValue<toneInfo>(tone),false));
}

void repeaterSetup::setRptAccessMode(rptrAccessData rd)
{
    if (rigCaps->commands.contains(funcToneSquelchType)) {
        queue->add(priorityImmediate,queueItem(funcToneSquelchType,QVariant::fromValue<rptrAccessData>(rd),false));
    } else {
        if(rd.accessMode == ratrTN) {
            // recuring=false, vfo if rd.useSEcondaryVFO
            queue->add(priorityImmediate,queueItem(funcRepeaterTone, QVariant::fromValue<bool>(true), false, rd.useSecondaryVFO));
        } else if (rd.accessMode == ratrTT) {
            queue->add(priorityImmediate,queueItem(funcRepeaterTSQL, QVariant::fromValue<bool>(true), false, rd.useSecondaryVFO));
        } else if (rd.accessMode == ratrNN) {
            queue->add(priorityImmediate,queueItem(funcRepeaterTone, QVariant::fromValue<bool>(false), false, rd.useSecondaryVFO));
            queue->add(priorityImmediate,queueItem(funcRepeaterTSQL, QVariant::fromValue<bool>(false), false, rd.useSecondaryVFO));
        }
    }
}

void repeaterSetup::on_toneNone_clicked()
{
    rptAccessTxRx_t rm;
    rptrAccessData rd;
    rm = ratrNN;
    rd.accessMode = rm;
    setRptAccessMode(rd);
    bool updateSub = ui->setSplitRptrToneChk->isEnabled() && ui->setSplitRptrToneChk->isChecked();

    if(updateSub)
    {
        rd.useSecondaryVFO = true;
        setRptAccessMode(rd);
    }
}

void repeaterSetup::on_toneTone_clicked()
{
    rptAccessTxRx_t rm;
    rptrAccessData rd;
    rm = ratrTN;
    rd.accessMode = rm;
    toneInfo rt;
    rt.tone = (quint16)ui->rptToneCombo->currentData().toUInt();
    setRptAccessMode(rd);
    queue->add(priorityImmediate,queueItem(funcToneFreq,QVariant::fromValue<toneInfo>(rt),false, rt.useSecondaryVFO));

    bool updateSub = ui->setSplitRptrToneChk->isEnabled() && ui->setSplitRptrToneChk->isChecked();

    if(updateSub)
    {
        rd.useSecondaryVFO = true;
        rt.useSecondaryVFO = true;
        setRptAccessMode(rd);
        queue->add(priorityImmediate,queueItem(funcToneFreq,QVariant::fromValue<toneInfo>(rt),false, rt.useSecondaryVFO));
    }
}

void repeaterSetup::on_toneTSQL_clicked()
{
    rptAccessTxRx_t rm;
    rptrAccessData rd;
    rm = ratrTT;
    toneInfo rt;
    rt.tone = (quint16)ui->rptToneCombo->currentData().toUInt();
    rd.accessMode = rm;
    setRptAccessMode(rd);
    queue->add(priorityImmediate,queueItem(funcTSQLFreq,QVariant::fromValue<toneInfo>(rt),false, rt.useSecondaryVFO));
    bool updateSub = ui->setSplitRptrToneChk->isEnabled() && ui->setSplitRptrToneChk->isChecked();

    if(updateSub)
    {
        rd.useSecondaryVFO = true;
        rt.useSecondaryVFO = true;
        setRptAccessMode(rd);
        queue->add(priorityImmediate,queueItem(funcTSQLFreq,QVariant::fromValue<toneInfo>(rt),false, rt.useSecondaryVFO));
    }
}

void repeaterSetup::on_toneDTCS_clicked()
{
    rptrAccessData rd;

    rd.accessMode = ratrDD;
    setRptAccessMode(rd);
    toneInfo tone;
    tone.tinv = ui->rptDTCSInvertTx->isChecked();
    tone.rinv = ui->rptDTCSInvertRx->isChecked();
    tone.tone = (quint16)ui->rptDTCSCombo->currentData().toUInt();
    queue->add(priorityImmediate,queueItem(funcDTCSCode,QVariant::fromValue<toneInfo>(tone),false));
    // TODO: DTCS with subband
}

quint64 repeaterSetup::getFreqHzFromKHzString(QString khz)
{
    // This function takes a string containing a number in KHz,
    // and creates an accurate quint64 in Hz.
    quint64 fhz = 0;
    bool ok = true;
    if(khz.isEmpty())
    {
        qWarning() << "KHz offset was empty!";
        return fhz;
    }
    if(khz.contains("."))
    {
        // "600.245" becomes "600"
        khz.chop(khz.indexOf("."));
    }
    fhz = 1E3 * khz.toUInt(&ok);
    if(!ok)
    {
        qWarning() << "Could not understand user KHz text";
    }
    return fhz;
}

quint64 repeaterSetup::getFreqHzFromMHzString(QString MHz)
{
    // This function takes a string containing a number in KHz,
    // and creates an accurate quint64 in Hz.
    quint64 fhz = 0;
    bool ok = true;
    if(MHz.isEmpty())
    {
        qWarning() << "MHz string was empty!";
        return fhz;
    }
    if(MHz.contains("."))
    {
        int decimalPtIndex = MHz.indexOf(".");
        // "29.623"
        // indexOf(".") = 2
        // length = 6
        // We want the right 4xx 3 characters.
        QString KHz = MHz.right(MHz.length() - decimalPtIndex - 1);
        MHz.chop(MHz.length() - decimalPtIndex);
        if(KHz.length() != 6)
        {
            QString zeros = QString("000000");
            zeros.chop(KHz.length());
            KHz.append(zeros);
        }
        //qInfo() << "KHz string: " << KHz;
        fhz = MHz.toUInt(&ok) * 1E6; if(!ok) goto handleError;
        fhz += KHz.toUInt(&ok) * 1; if(!ok) goto handleError;
        //qInfo() << "Fhz: " << fhz;
    } else {
        // Frequency was already MHz (unlikely but what can we do?)
        fhz = MHz.toUInt(&ok) * 1E6; if(!ok) goto handleError;
    }
    return fhz;

handleError:
    qWarning() << "Could not understand user MHz text " << MHz;
    return 0;
}

void repeaterSetup::on_splitPlusButton_clicked()
{
    quint64 hzOffset = getFreqHzFromKHzString(ui->splitOffsetEdit->text());
    quint64 txfreqhz;
    freqt f;
    QString txString;
    if(!haveReceivedFrequency) {
        qWarning(logRptr()) << "Cannot compute offset without first receiving a frequency.";
        return;
    }
    if(hzOffset)
    {
        txfreqhz = currentMainFrequency.Hz + hzOffset;
        f.Hz = txfreqhz;
        f.VFO = inactiveVFO;
        f.MHzDouble = f.Hz/1E6;
        txString = QString::number(f.Hz / double(1E6), 'f', 6);
        ui->splitTransmitFreqEdit->setText(txString);
        usedPlusSplit = true;

    } else {
        qWarning(logRptr()) << "Cannot set split using supplied offset value.";
        return;
    }
    if(ui->splitEnableChk->isChecked() || ui->quickSplitChk->isChecked()) {
        if (rigCaps->commands.contains(funcFreq) && rigCaps->numReceiver > 1)
            queue->add(priorityImmediate,queueItem(funcFreq,QVariant::fromValue<freqt>(f),false,1));
        else if (rigCaps->commands.contains(funcUnselectedFreq))
            queue->add(priorityImmediate,queueItem(funcUnselectedFreq,QVariant::fromValue<freqt>(f),false));
        queue->add(priorityImmediate,queueItem(funcModeSet,QVariant::fromValue<modeInfo>(modeTransmitVFO)));
    } else {
        qWarning(logRptr()) << "Not setting transmit frequency until split mode is enabled.";
    }
}

void repeaterSetup::on_splitMinusBtn_clicked()
{
    quint64 hzOffset = getFreqHzFromKHzString(ui->splitOffsetEdit->text());
    quint64 txfreqhz;
    freqt f;
    QString txString;
    if(!haveReceivedFrequency) {
        qWarning(logRptr()) << "Cannot compute offset without first receiving a frequency.";
        return;
    }
    if(hzOffset)
    {
        txfreqhz = currentMainFrequency.Hz - hzOffset;
        f.Hz = txfreqhz;
        f.VFO = inactiveVFO;
        f.MHzDouble = f.Hz/1E6;
        txString = QString::number(f.Hz / double(1E6), 'f', 6);
        ui->splitTransmitFreqEdit->setText(txString);
        usedPlusSplit = false;
    } else {
        qWarning(logRptr()) << "Cannot set split using supplied offset value.";
        return;
    }

    if(ui->splitEnableChk->isChecked() || ui->quickSplitChk->isChecked()) {
        if (rigCaps->commands.contains(funcFreq) && rigCaps->numReceiver > 1)
            queue->add(priorityImmediate,queueItem(funcFreq,QVariant::fromValue<freqt>(f),false,1));
        else if (rigCaps->commands.contains(funcUnselectedFreq))
            queue->add(priorityImmediate,queueItem(funcUnselectedFreq,QVariant::fromValue<freqt>(f),false));
        queue->add(priorityImmediate,queueItem(funcModeSet,QVariant::fromValue<modeInfo>(modeTransmitVFO),false));
    } else {
        qWarning(logRptr()) << "Not setting transmit frequency until split mode is enabled.";
        return;
    }
}

void repeaterSetup::on_splitTxFreqSetBtn_clicked()
{
    if(!haveReceivedFrequency) {
        qWarning(logRptr()) << "Cannot compute offset without first receiving a frequency.";
        return;
    }
    quint64 fHz = getFreqHzFromMHzString(ui->splitTransmitFreqEdit->text());
    freqt f;
    if(fHz)
    {
        f.Hz = fHz;
        f.VFO = inactiveVFO;
        f.MHzDouble = f.Hz/1E6;
        if (rigCaps->commands.contains(funcFreq) && rigCaps->numReceiver > 1)
            queue->add(priorityImmediate,queueItem(funcFreq,QVariant::fromValue<freqt>(f),false,1));
        else if (rigCaps->commands.contains(funcUnselectedFreq))
            queue->add(priorityImmediate,queueItem(funcUnselectedFreq,QVariant::fromValue<freqt>(f),false));
        queue->add(priorityImmediate,queueItem(funcModeSet,QVariant::fromValue<modeInfo>(modeTransmitVFO),false,0));
    }
}

void repeaterSetup::on_splitTransmitFreqEdit_returnPressed()
{
    this->on_splitTxFreqSetBtn_clicked();
    ui->splitTransmitFreqEdit->clearFocus();
}

void repeaterSetup::on_selABtn_clicked()
{
    vfo_t v = vfoA;
    queue->add(priorityImmediate,queueItem(funcSelectVFO,QVariant::fromValue<vfo_t>(v),false));
}

void repeaterSetup::on_selBBtn_clicked()
{
    vfo_t v = vfoB;
    queue->add(priorityImmediate,queueItem(funcSelectVFO,QVariant::fromValue<vfo_t>(v),false));
}

void repeaterSetup::on_aEqBBtn_clicked()
{
    queue->add(priorityImmediate,funcVFOEqualAB,false,false);
}

void repeaterSetup::on_swapABBtn_clicked()
{
    queue->add(priorityImmediate,funcVFOSwapAB,false,false);
}

void repeaterSetup::on_selMainBtn_clicked()
{
    vfo_t v = vfoMain;
    queue->add(priorityImmediate,queueItem(funcSelectVFO,QVariant::fromValue<vfo_t>(v),false));
}

void repeaterSetup::on_selSubBtn_clicked()
{
    vfo_t v = vfoSub;
    queue->add(priorityImmediate,queueItem(funcSelectVFO,QVariant::fromValue<vfo_t>(v),false));
}

void repeaterSetup::on_mEqSBtn_clicked()
{
    queue->add(priorityImmediate,funcVFOEqualMS,false,false);
}

void repeaterSetup::on_swapMSBtn_clicked()
{
    queue->add(priorityImmediate,funcVFOSwapMS,false,false);
}

void repeaterSetup::on_setToneSubVFOBtn_clicked()
{
    // Perhaps not needed
    // Set the secondary VFO to the selected tone
    // TODO: DTCS
    toneInfo rt;
    rt.tone = (quint16)ui->rptToneCombo->currentData().toUInt();
    rt.useSecondaryVFO = true;
    queue->add(priorityImmediate,queueItem(funcToneFreq,QVariant::fromValue<toneInfo>(rt),false, rt.useSecondaryVFO));
}

void repeaterSetup::on_setRptrSubVFOBtn_clicked()
{
    // Perhaps not needed
    // Set the secondary VFO to the selected repeater mode
    rptrAccessData rd;
    rd.useSecondaryVFO = true;

    if(ui->toneTone->isChecked())
        rd.accessMode=ratrTN;
    if(ui->toneNone->isChecked())
        rd.accessMode=ratrNN;
    if(ui->toneTSQL->isChecked())
        rd.accessMode=ratrTT;
    if(ui->toneDTCS->isChecked())
        rd.accessMode=ratrDD;

    setRptAccessMode(rd);
}

void repeaterSetup::on_rptrOffsetSetBtn_clicked()
{
    freqt f;
    f.Hz = getFreqHzFromMHzString(ui->rptrOffsetEdit->text());
    f.MHzDouble = f.Hz/1E6;
    f.VFO=activeVFO;
    if(f.Hz != 0)
    {
        queue->add(priorityImmediate,queueItem(funcSendFreqOffset,QVariant::fromValue<freqt>(f),false));
    } else {
        qWarning() << "Could not convert frequency text of repeater offset to integer.";
    }
    ui->rptrOffsetEdit->clearFocus();
}

void repeaterSetup::on_rptrOffsetEdit_returnPressed()
{
    this->on_rptrOffsetSetBtn_clicked();
}

void repeaterSetup::on_setSplitRptrToneChk_clicked(bool checked)
{
    if(checked)
    {
        on_setRptrSubVFOBtn_clicked();
        on_setToneSubVFOBtn_clicked();
    }
}

void repeaterSetup::on_quickSplitChk_clicked(bool checked)
{
    queue->add(priorityImmediate,queueItem(funcQuickSplit,QVariant::fromValue<bool>(checked),false));
}

void repeaterSetup::receiveRigCaps(rigCapabilities* rig)
{
    qInfo() << "Receiving rigcaps into repeater setup.";
    this->rigCaps = rig;
    if (rig != Q_NULLPTR)
    {
        qInfo() << "repeaterSetup got rigcaps for:" << rig->modelName;

        if(rig->commands.contains(funcRepeaterTone)) {
            ui->rptToneCombo->setDisabled(false);
            ui->toneTone->setDisabled(false);
        } else {
            ui->rptToneCombo->setDisabled(true);
            ui->toneTone->setDisabled(true);
        }

        if(rig->commands.contains(funcRepeaterTSQL)) {
            ui->toneTSQL->setDisabled(false);
        } else {
            ui->toneTSQL->setDisabled(true);
        }

        if(rig->commands.contains(funcToneSquelchType))
        {
            ui->rptToneCombo->setDisabled(false);
            ui->toneTone->setDisabled(false);
            ui->toneTSQL->setDisabled(false);
        }

        if(rig->commands.contains(funcRepeaterDTCS))
        {
            ui->rptDTCSCombo->setDisabled(false);
            ui->toneDTCS->setDisabled(false);
            ui->rptDTCSInvertRx->setDisabled(false);
            ui->rptDTCSInvertTx->setDisabled(false);
        } else {
            ui->rptDTCSCombo->setDisabled(true);
            ui->toneDTCS->setDisabled(true);
            ui->rptDTCSInvertRx->setDisabled(true);
            ui->rptDTCSInvertTx->setDisabled(true);
        }
        if(rig->commands.contains(funcVFOEqualAB))
        {
            ui->selABtn->setDisabled(false);
            ui->selBBtn->setDisabled(false);
            ui->aEqBBtn->setDisabled(false);
            ui->swapABBtn->setDisabled(false);
        } else {
            ui->selABtn->setDisabled(true);
            ui->selBBtn->setDisabled(true);
            ui->aEqBBtn->setDisabled(true);
            ui->swapABBtn->setDisabled(true);
        }
        if(rig->commands.contains(funcVFOEqualMS))
        {
            ui->selMainBtn->setDisabled(false);
            ui->selSubBtn->setDisabled(false);
            ui->mEqSBtn->setDisabled(false);
            ui->swapMSBtn->setDisabled(false);
        } else {
            ui->selMainBtn->setDisabled(true);
            ui->selSubBtn->setDisabled(true);
            ui->mEqSBtn->setDisabled(true);
            ui->swapMSBtn->setDisabled(true);
        }
        if(rig->commands.contains(funcVFOEqualAB) && rig->commands.contains(funcVFOEqualMS))
        {
            // Rigs that have both AB and MS
            // do not have a swap AB command.
            //ui->swapABBtn->setDisabled(true);
        }

        bool mainSub = rig->commands.contains(funcVFOMainSelect);
        if(mainSub)
        {
            ui->setRptrSubVFOBtn->setEnabled(true);
            ui->setToneSubVFOBtn->setEnabled(true);
            ui->setSplitRptrToneChk->setEnabled(true);
        } else {
            ui->setRptrSubVFOBtn->setDisabled(true);
            ui->setToneSubVFOBtn->setDisabled(true);
            ui->setSplitRptrToneChk->setDisabled(true);
        }

        bool rpt = rig->commands.contains(funcToneSquelchType);
        ui->rptAutoBtn->setEnabled(rpt);
        ui->rptDupMinusBtn->setEnabled(rpt);
        ui->rptDupPlusBtn->setEnabled(rpt);
        ui->rptSimplexBtn->setEnabled(rpt);
        ui->rptrOffsetEdit->setEnabled(rpt);
        ui->rptrOffsetSetBtn->setEnabled(rpt);
        ui->setToneSubVFOBtn->setEnabled(mainSub);
        ui->setRptrSubVFOBtn->setEnabled(mainSub);
        ui->quickSplitChk->setVisible(rig->commands.contains(funcQuickSplit));
    }
}

void repeaterSetup::receiveQuickSplit(bool on)
{
    ui->quickSplitChk->blockSignals(true);
    ui->quickSplitChk->setChecked(on);
    ui->quickSplitChk->blockSignals(false);
}
