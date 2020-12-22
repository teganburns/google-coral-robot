// google-coral-servos.hpp

#include <fcntl.h>    // For O_RDWR
#include <unistd.h>   // For open(), creat() 
#include <sys/ioctl.h>
#include <termios.h>
#include <cstdio>
#include <stdlib.h>
#include <iostream>

extern "C" {
	#include <linux/i2c-dev.h>
	#include <i2c/smbus.h>
}

#define SERVO_MIN 100
#define SERVO_MAX 200
#define SERVO_MID 150


class Servos {

	private:
		void i2c_send_byte( char = 0x00 );
		void i2c_print_byte();

	public:
		void set( int left, int right );
		void set_print( int left, int right );

};


