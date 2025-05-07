#ifndef TRANSCEIVERADJUSTMENTS_H
#define TRANSCEIVERADJUSTMENTS_H



#include <QtGlobal>
#include <QWidget>

#include <math.h>

#include "rigidentities.h"


namespace Ui {
class transceiverAdjustments;
}

class transceiverAdjustments : public QWidget
{
    Q_OBJECT

public:
    explicit transceiverAdjustments(QWidget *parent = 0);
    ~transceiverAdjustments();
    void setMaxPassband(quint16 maxHzAllowed);

signals:
    void setIFShift(unsigned char level);
    void setPBTInner(unsigned char level);
    void setPBTOuter(unsigned char level);
    void setPassband(quint16 passbandHz);

public slots:
    void setRig(rigCapabilities rig);
    void updateIFShift(unsigned char level);
    void updatePBTInner(unsigned char level);
    void updatePBTOuter(unsigned char level);
    void updatePassband(quint16 passbandHz);

private slots:

    void on_IFShiftSlider_valueChanged(int value);

    void on_PBTInnerSlider_valueChanged(int value);

    void on_PBTOuterSlider_valueChanged(int value);

    void on_resetPBTbtn_clicked();

    void on_passbandWidthSlider_valueChanged(int value);

private:
    Ui::transceiverAdjustments *ui;
    rigCapabilities rigCaps;
    bool haveRigCaps = false;
    int previousIFShift = 128;
    float maxHz = 10E3;
    quint16 lastKnownPassband = 3500;
};

#endif // TRANSCEIVERADJUSTMENTS_H
