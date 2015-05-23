#include "sampleobj.h"

SampleObj::SampleObj()
{
    memset(receive_buf, 0, sizeof receive_buf);
    memset(send_buf, 0, sizeof send_buf);//initial all member of array = 0
}

SampleObj::~SampleObj() {
    delete timer;
}

unsigned char * SampleObj::getBuf() {
    return receive_buf;
}


void SampleObj::getValue() {
        getStatus();
    }

void SampleObj::setCurrent_stepX(unsigned short value)
{
    current_stepX = value;
}

void SampleObj::setCurrent_stepY(unsigned short value)
{
    current_stepY = value;
}
void SampleObj::setCurrent_stepZ(unsigned short value)
{
    current_stepZ = value;
}

unsigned short SampleObj::getCurrent_stepX() const {
    return current_stepX;
}
unsigned short SampleObj::getCurrent_stepY() const {
    return current_stepY;
}
unsigned short SampleObj::getCurrent_stepZ() const {
    return current_stepZ;
}




void SampleObj::startTimer() {
    connect(timer,SIGNAL(timeout()),this,SLOT(getValue()));
    timer->start(33);
}



void SampleObj::sendWorkingStatus(char status) {
    if(handle) {
        send_buf[0] = 0x0;
        send_buf[1] = 0x13;
        send_buf[2] = 0x04;
        send_buf[3] = CMD_SETSTATUS;
        send_buf[4] = status;
        send_buf[5] = 0x0;
        send_buf[6] = 0x0;
        send_buf[7] = 0x0;
        send_buf[8] = 0x0;
        send_buf[9] = 0x0;
        res = hid_write(handle, send_buf, 65);
    }

}
void SampleObj::getStatus(){
    if(handle) {
        send_buf[0] = 0x0;
        send_buf[1] = 0x13;
        send_buf[2] = 0x04;
        send_buf[3] = CMD_GET_STATUS;
        send_buf[4] = 0x0;
        send_buf[5] = 0x0;
        send_buf[6] = 0x0;
        send_buf[7] = 0x0;
        send_buf[8] = 0x0;
        send_buf[9] = 0x0;
        res = hid_write(handle, send_buf, 65);
        res = hid_read(handle, receive_buf, 65);
    if(res>0) {
        qDebug("%x %x %x %x %x %x %x %x %x %x %x",receive_buf[2],receive_buf[3],receive_buf[4],receive_buf[5],receive_buf[6],receive_buf[7]
                ,receive_buf[8],receive_buf[9],receive_buf[10],receive_buf[11],receive_buf[12]);
    }
    }
}


void SampleObj::processCommand(QString cmd, float cmdval, float Xval, float Yval, float Zval, int line)
{
    if (!QString::compare(cmd,"G"))
    {
        switch ((int)cmdval) {
        case 0: //Rapid Motion
            gotoXYZ(Xval, Yval, Zval, 0);
            break;
        case 1: //Linear Motion
            gotoXYZ(Xval, Yval, Zval, 1);
            break;
        {
//        case 02: //Circular move CW
//            //example: G02 X## Y## I## J## (XY=end point, IJ=center point)
//            //example: G02 X## Y## R±## (R=size of radius arc to swing.  R+ if radius < 180°, R- if radius is > 180°)
//            break;
//        case 03: //Circular move CCW
//            break;
//        case 04: //Dwell time  example: G04 P## (P=time to dwell. P2000 is 2 seconds)
//            //delay
//            delay(1000);
//            break;
//        case 10: //Zero offset shift
//            //example: G10 X## Y## Z##(X=shift dist. Y=shift dist. Z=shift dist.)
//            break;
//        case 11: //Zero offset shift cancel
//            //example: G11
//            break;
//        case 15: //Turn Polar Coordinates OFF, return to Cartesian Coordinates
//            break;
//        case 16: //Turn Polar Coordinates ON
//            break;
//        case 20: //Inches for Units
//            conversionFactor = 25.4f;  // 1 for mm 25.4 for inches
//            break;
//        case 21: //mm for Units
//            conversionFactor = 1.0f;  // 1 for mm 25.4 for inches
//            break;
//        case 28: //go home.
//            gotoXYZ(0, 0, 0, 0);
//            break;
//        case 30://go home via an intermediate point.
//            gotoXYZ(Xval, Yval, Zval, 0);
//            gotoXYZ(0, 0, 0, 0);
//            break;
//        case 81: // drilling operation
//            //drill();
//            break;
//        case 90://Absolute Positioning DEFAULT
//          //abs_mode = true;
//            break;
//        case 91://Incremental Positioning
//          //abs_mode = false;
//            break;
        }
        default:
            break;
        }
    }
    else if (!QString::compare(cmd,"M"))
    {
        //write later
    }
    else if (!QString::compare(cmd,"N"))
    {
        //Skip line number
    }
    else
    {
        //warn msg
        // remove unsupport Gcode line
    }
}

void SampleObj::gotoXYZ(float Xval, float Yval, float Zval, bool isFeedrate)
{
  unsigned short step_x = convertToStep(Xval,STEP_PER_MM_X);
  unsigned short step_y = convertToStep(Yval,STEP_PER_MM_Y);
  short dX;
  short dY;
  if(Xval >=0)
    dX = step_x - current_stepX; //delta step x
  else dX = step_x;
  if(Yval >=0)
    dY = step_y - current_stepY; //delta step y
  else dY = step_y;
  char dir_x = sign(dX);  // dir  0 <---> CCW, lùi ; 1 <---> CW, tiến
  char dir_y = sign(dY);
  moveMotor(abs(dX),dir_x,abs(dY), dir_y, isFeedrate);
}





void SampleObj::moveMotor(unsigned short x, char dirX,unsigned short y, char dirY, bool isFeedrate) // thêm bool isFeedrate
{

    char lowX;
    char lowY;
    char highX;
    char highY;
    Convert_u16_u8(x,highX,lowX);
    Convert_u16_u8(y,highY,lowY);

    if(handle) {
        send_buf[0] = 0x0;
        send_buf[1] = 0x13;
        send_buf[2] = 0x04;
        send_buf[3] = CMD_STEP;
        send_buf[4] = highX;
        send_buf[5] = lowX;
        send_buf[6] = dirX;
        send_buf[7] = highY;
        send_buf[8] = lowY;
        send_buf[9] = dirY;
        res = hid_write(handle, send_buf, 65);
    }
    //bool isFeedrate   0 <---> MAXRATE, 1 <---> FEEDRATE
    //dựa vào biến bool này mà ông cho phép motor chạy tốc độ bao nhiêu

}



float SampleObj::convertToPos(unsigned short step,float step_per_mm) {
    return step/step_per_mm;
}

unsigned short SampleObj::convertToStep(float distance, float step_per_mm) {
    return distance*conversionFactor*step_per_mm;
}


char SampleObj::sign(short value) {
    if(value >=0 ) return 1;
    else return 0;
}