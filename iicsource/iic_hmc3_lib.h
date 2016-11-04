
const int HMC5883L_I2C_ADDR = 0x1E;

void selectDevice(int fd, int addr, char * name);

void writeToDevice(int fd, int reg, int val);

int initializeCompass();

/*
 Data to store result
 fd to feed file description for device
return if 0 failed read data, 1 success read data
*/
int readCompassData(short* Data, int fd);

/*
get heading data in radian
*/
float getHeading(int fd);

