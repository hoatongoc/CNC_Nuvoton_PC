#ifndef MOTOR_CONTROL
#define MOTOR_CONTROL

#include "hidapi.h"
#include<QString>

#define MAX_STEP_X 14000
#define MAX_STEP_Y 14000

#define DEF_SPEED 800
#define MAX_SPEED 20000

#define CMD_GET_STATUS  	0xA0
#define CMD_RUN             0xA1
#define CMD_PAUSE 			0xA2
#define CMD_WAIT            0xA3
#define CMD_SETHOME			0xA4
#define CMD_MOVEHOME 		0xA5
#define CMD_SETSTATUS   	0xA6
#define CMD_ROTATE_SERVO 	0xA7
#define CMD_MOVE 			0xAA
#define CMD_CLR             0xAB
#define CMD_DIR 			0xAC
#define CMD_STEP 			0xAD
#define CMD_SPEED			0xAE
#define CMD_RESET			0xAF

#define CW  1
#define CCW 0

#define RUN 1
#define PAUSE 0

#define MC_X 0
#define MC_Y 1
#define SERVO_Z 3

#define NOT_FINISHED 0
#define FINISHED 1
#define WAIT 2


#define MAX 100

#define RESPONSE_OK "ok"
#define RESPONSE_ERROR "error"
#define MM_PER_STEP_X 0.015873 // 1 step = 1/63 mm
#define STEP_PER_MM_X 63 // 1mm = 63 step

#define MM_PER_STEP_Y 0.015625 // 1 step = 1/64 mm
#define STEP_PER_MM_Y 64 // 1mm = 64 step

//extern std::queue<std::vector<float>> g_queue;

typedef struct{
  QString command;
  float value;
} GCode;




extern short motorSpeed;
extern char isRun;
extern hid_device *handle;
extern char finished;
extern bool lock;
unsigned short Convert_u8_u16(char high, char low);
void Convert_u16_u8(unsigned short value, char &high, char &low);





typedef struct {
    unsigned short x;
    char dirX;
    unsigned short y;
    char dirY;
}command;




#endif // MOTOR_CONTROL

