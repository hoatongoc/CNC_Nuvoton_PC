#include "motor_control.h"


unsigned short Convert_u8_u16(char high, char low) {
    unsigned short result = (unsigned short)(high<<8|(low & 0x00FF));
    return result;
}

void Convert_u16_u8(unsigned short value, char &high, char &low) {
    high = value >>8;
    low = (char)value;
}








