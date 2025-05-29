#include "bandbuttons.h"
#include "ui_bandbuttons.h"

bandbuttons::bandbuttons(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::bandbuttons)
{
    ui->setupUi(this);
    ui->bandStkLastUsedBtn->setVisible(false);
    ui->bandStkVoiceBtn->setVisible(false);
    ui->bandStkDataBtn->setVisible(false);
    ui->bandStkCWBtn->setVisible(false);
    this->setWindowTitle("Band Switcher");
    this->setObjectName("bandButtons");
    queue = cachingQueue::getInstance();
    connect(queue, SIGNAL(rigCapsUpdated(rigCapabilities*)), this, SLOT(receiveRigCaps(rigCapabilities*)));
    connect(queue,SIGNAL(cacheUpdated(cacheItem)),this,SLOT(receiveCache(cacheItem)));

}

bandbuttons::~bandbuttons()
{
    delete ui;
}

int bandbuttons::getBSRNumber()
{
    return ui->bandStkPopdown->currentIndex()+1;
}

void bandbuttons::receiveCache(cacheItem item)
{
    // Only used to receive initial frequency!
    bool sub=false;
    switch (item.command)
    {
#if defined __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wimplicit-fallthrough"
#endif
    case funcUnselectedFreq:
        sub=true;
    case funcFreq:
    case funcSelectedFreq:
    case funcFreqGet:
    case funcFreqTR:
        // Here we will process incoming frequency.
        {
        if(!rigCaps) {
            return;
        }
        if (requestedBand == bandUnknown) {
            if (ui->SubBandCheck->isChecked() == bool(item.value.value<freqt>().VFO)) {
                    quint64 freq = quint64(item.value.value<freqt>().Hz);
                    for (auto &b: rigCaps->bands)
                    {
                        // Highest frequency band is always first!
                        if (freq >= b.lowFreq && freq <= b.highFreq)
                        {
                            // This frequency is contained within this band!
                            qInfo() << "Band Buttons found current band:" << b.name;
                            requestedBand = b.band;
                            break;
                        }
                    }
                }
            }
        }
        if (sub == ui->SubBandCheck->isChecked()) {
            currentFrequency = item.value.value<freqt>();
        }
        break;
    case funcUnselectedMode:
        sub=true;
    case funcMode:
    case funcSelectedMode:
    case funcModeGet:
    case funcModeTR:
        {
        if(!rigCaps) {
            return;
        }
        if (sub == ui->SubBandCheck->isChecked()) {
            currentMode = item.value.value<modeInfo>();
        }
            break;
        }
#if defined __GNUC__
#pragma GCC diagnostic pop
#endif
        case funcBandStackReg:
            if(!rigCaps) {
                return;
            }
            currentBSR = item.value.value<bandStackType>();

    default:
        break;
    }
}

void bandbuttons::receiveRigCaps(rigCapabilities* rc)
{
    this->rigCaps = rc;
    qDebug(logGui()) << "Accepting new rigcaps into band buttons.";

    if (rc != Q_NULLPTR) {
        qDebug(logGui()) << "Bands in this rigcaps: ";
        for(size_t i=0; i < rigCaps->bands.size(); i++)
        {
            qDebug(logGui()) << "band[" << i << "]: " << (quint8)rigCaps->bands[i].band;
        }

        for(size_t i=0; i < 20; i++)
        {
            qDebug(logGui()) << "bsr[" << i << "]: " << (quint8)rigCaps->bsr[i];
        }
    } else {
        requestedBand = bandUnknown;
    }

    setUIToRig();
}

void bandbuttons::setUIToRig()
{
    // Turn off each button first:
    hideButton(ui->band3cmbtn);
    hideButton(ui->band6cmbtn);
    hideButton(ui->band13cmbtn);
    hideButton(ui->band23cmbtn);
    hideButton(ui->band70cmbtn);
    hideButton(ui->band2mbtn);
    hideButton(ui->bandAirbtn);
    hideButton(ui->bandWFMbtn);
    hideButton(ui->band4mbtn);
    hideButton(ui->band6mbtn);

    hideButton(ui->band10mbtn);
    hideButton(ui->band12mbtn);
    hideButton(ui->band15mbtn);
    hideButton(ui->band17mbtn);
    hideButton(ui->band20mbtn);
    hideButton(ui->band30mbtn);
    hideButton(ui->band40mbtn);
    hideButton(ui->band60mbtn);
    hideButton(ui->band80mbtn);
    hideButton(ui->band160mbtn);

    hideButton(ui->band630mbtn);
    hideButton(ui->band2200mbtn);
    hideButton(ui->bandGenbtn);

    if (rigCaps != Q_NULLPTR) {
        for (auto &band: rigCaps->bands)
        {
            switch(band.band)
            {
                case(band3cm):
                    showButton(ui->band3cmbtn);
                    break;
                case(band6cm):
                    showButton(ui->band6cmbtn);
                    break;
                case(band13cm):
                    showButton(ui->band13cmbtn);
                    break;
                case(band23cm):
                    showButton(ui->band23cmbtn);
                    break;
                case(band70cm):
                    showButton(ui->band70cmbtn);
                    break;
                case(band2m):
                    showButton(ui->band2mbtn);
                    break;
                case(bandAir):
                    showButton(ui->bandAirbtn);
                    break;
                case(bandWFM):
                    showButton(ui->bandWFMbtn);
                    break;
                case(band4m):
                    showButton(ui->band4mbtn);
                    break;
                case(band6m):
                    showButton(ui->band6mbtn);
                    break;
                case(band10m):
                    showButton(ui->band10mbtn);
                    break;
                case(band12m):
                    showButton(ui->band12mbtn);
                    break;
                case(band15m):
                    showButton(ui->band15mbtn);
                    break;
                case(band17m):
                    showButton(ui->band17mbtn);
                    break;
                case(band20m):
                    showButton(ui->band20mbtn);
                    break;
                case(band30m):
                    showButton(ui->band30mbtn);
                    break;
                case(band40m):
                    showButton(ui->band40mbtn);
                    break;
                case(band60m):
                    showButton(ui->band60mbtn);
                    break;
                case(band80m):
                    showButton(ui->band80mbtn);
                    break;
                case(band160m):
                    showButton(ui->band160mbtn);
                    break;
                case(band630m):
                    showButton(ui->band630mbtn);
                    break;
                case(band2200m):
                    showButton(ui->band2200mbtn);
                    break;
                case(bandGen):
                    showButton(ui->bandGenbtn);
                    break;

                default:
                    break;
            }
        }
    }
}

void bandbuttons::showButton(QPushButton *b)
{
    b->setVisible(true);
}

void bandbuttons::hideButton(QPushButton *b)
{
    b->setHidden(true);
}

void bandbuttons::bandStackBtnClick(availableBands band)
{
    if(rigCaps != Q_NULLPTR)
    {
        for (auto &b: rigCaps->bands)
        {
            if (b.band == band)
            {
                if(b.bsr == 0)
                {
                    qDebug(logGui()) << "requested to drop to band that does not have a BSR.";
                    jumpToBandWithoutBSR(band);
                } else {
                    queue->add(priorityImmediate,queueItem(funcBandStackReg,
                        QVariant::fromValue<bandStackType>(bandStackType(b.bsr,ui->bandStkPopdown->currentIndex()+1)),false,ui->SubBandCheck->isChecked()));
                }
                requestedBand = band;
                break;
            }
        }
    } else {
        qWarning(logGui()) << "bandbuttons, Asked to go to a band but do not have rigCaps yet.";
    }
}

void bandbuttons::jumpToBandWithoutBSR(availableBands band)
{
    // Sometimes we do not have a BSR for these bands:
    if (rigCaps != Q_NULLPTR)
    {
        for (auto &b: rigCaps->bands)
        {
            if (b.band == band)
            {
                freqt f;
                f.Hz = (b.lowFreq+b.highFreq)/2.0;
                f.MHzDouble = f.Hz/1000000.0;
                f.VFO = activeVFO;
                queue->add(priorityImmediate,queueItem(funcFreq,QVariant::fromValue<freqt>(f),false,ui->SubBandCheck->isChecked()));
                break;
            }
        }
    }
}

void bandbuttons::on_band2200mbtn_clicked()
{
    bandStackBtnClick(band2200m);
}

void bandbuttons::on_band630mbtn_clicked()
{
    bandStackBtnClick(band630m);
}

void bandbuttons::on_band160mbtn_clicked()
{
    bandStackBtnClick(band160m);
}

void bandbuttons::on_band80mbtn_clicked()
{
    bandStackBtnClick(band80m);
}

void bandbuttons::on_band60mbtn_clicked()
{
    bandStackBtnClick(band60m);
}

void bandbuttons::on_band40mbtn_clicked()
{
    bandStackBtnClick(band40m);
}

void bandbuttons::on_band30mbtn_clicked()
{
    bandStackBtnClick(band30m);
}

void bandbuttons::on_band20mbtn_clicked()
{
    bandStackBtnClick(band20m);
}

void bandbuttons::on_band17mbtn_clicked()
{
    bandStackBtnClick(band17m);
}

void bandbuttons::on_band15mbtn_clicked()
{
    bandStackBtnClick(band15m);
}

void bandbuttons::on_band12mbtn_clicked()
{
    bandStackBtnClick(band12m);
}

void bandbuttons::on_band10mbtn_clicked()
{
    bandStackBtnClick(band10m);
}

void bandbuttons::on_band6mbtn_clicked()
{
    bandStackBtnClick(band6m);
}

void bandbuttons::on_band4mbtn_clicked()
{
    bandStackBtnClick(band4m);
}

void bandbuttons::on_band2mbtn_clicked()
{
    bandStackBtnClick(band2m);
}

void bandbuttons::on_band70cmbtn_clicked()
{
    bandStackBtnClick(band70cm);
}

void bandbuttons::on_band23cmbtn_clicked()
{
    bandStackBtnClick(band23cm);
}

void bandbuttons::on_band13cmbtn_clicked()
{
    bandStackBtnClick(band13cm);
}

void bandbuttons::on_band6cmbtn_clicked()
{
    bandStackBtnClick(band6cm);
}

void bandbuttons::on_band3cmbtn_clicked()
{
    bandStackBtnClick(band3cm);
}

void bandbuttons::on_bandWFMbtn_clicked()
{
    bandStackBtnClick(bandWFM);
}

void bandbuttons::on_bandAirbtn_clicked()
{
    bandStackBtnClick(bandAir);
}

void bandbuttons::on_bandGenbtn_clicked()
{
    bandStackBtnClick(bandGen);
}

void bandbuttons::on_bandSetBtn_clicked()
{
    if(rigCaps != Q_NULLPTR)
    {
        qInfo() << "Setting BSR to current freq/mode, first find band that contains frequency:" << currentFrequency.MHzDouble;
        // First find which band we are in
        for (auto &band: rigCaps->bands)
        {
            if (band.region == "" || band.region == region) {
                if (band.bsr != 0 && currentFrequency.Hz >= band.lowFreq && currentFrequency.Hz <= band.highFreq)
                {
                   // qInfo() << "Found band" << band.name;
                    // This frequency is within this band
                    bandStackType bs=currentBSR;
                    bs.band = band.bsr;
                    bs.data = currentMode.data;
                    bs.freq = currentFrequency;
                    bs.filter = currentMode.filter;
                    bs.regCode = ui->bandStkPopdown->currentIndex()+1;
                    bs.mode = currentMode.reg;
                    // If we haven't received a tone yet, use default.
                    if (bs.tone.tone == 0) {
                        bs.tone.tone = 885;
                        bs.tsql.tone = 885;
                    }
                    queue->add(priorityImmediate,queueItem(funcBandStackReg, QVariant::fromValue<bandStackType>(bs),false,ui->SubBandCheck->isChecked()));
                    break;
                }
            }
        }
    } else {
        qWarning(logGui()) << "bandbuttons, Asked to go to a band but do not have rigCaps yet.";
    }
}
