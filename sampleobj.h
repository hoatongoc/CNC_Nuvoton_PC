#ifndef SAMPLEOBJ_H
#define SAMPLEOBJ_H
#include "QObject"
#include "hidapi.h"
#include "motor_control.h"
#include "QTimer"
class SampleObj: public QObject
{
    Q_OBJECT
private:
    QTimer *timer = new QTimer(this);
    int res =0;
    unsigned char send_buf[65];
    unsigned char receive_buf[65];
    float conversionFactor = 1.0f;
    unsigned short current_stepX;
    unsigned short current_stepY;
    unsigned short current_stepZ;

    char sign(short value);

public:

    SampleObj();
    ~SampleObj();
    unsigned char * getBuf();
    //std::vector<int> getBuf();
    void startTimer();
    void processCommand(QString cmd, float cmdval, float Xval, float Yval, float Zval, int line);
    void gotoXYZ(float Xval, float Yval, float Zval, bool isFeedrate);
    void reqCurrentPos();
    void SampleObj::moveMotor(unsigned short x, char dirX, unsigned short speedX, unsigned short y, char dirY, unsigned short speedY, bool isFeedrate);
    void sendWorkingStatus(char status);
    void getStatus();

    unsigned short getCurrent_stepX() const;
    void setCurrent_stepX(unsigned short value);
    unsigned short getCurrent_stepY() const;
    void setCurrent_stepY(unsigned short value);
    unsigned short getCurrent_stepZ() const;
    void setCurrent_stepZ(unsigned short value);
    float convertToPos(unsigned short step,float step_per_mm);
    unsigned short convertToStep(float distance, float step_per_mm);

public slots:
    void getValue();

};



#endif // SAMPLEOBJ_H
