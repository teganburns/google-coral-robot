#include <iostream>
#include <fcntl.h>    // For O_RDWR
#include <unistd.h>   // For open(), creat() 
#include <sys/ioctl.h>
#include <termios.h>

extern "C" {
	#include <linux/i2c-dev.h>
	#include <i2c/smbus.h>
}


#define SERVO_MIN 100
#define SERVO_MAX 200
#define SERVO_MID 150

using namespace std;

void error( string message = "NA" ){
	std::cout << "Has Error Dummy" << std::endl;
	cout << "Message: " << message << endl;
}

int i2c_read_byte(){

	int file;
	int adapter_nr = 1; // probably dynamically determined
	char filename[20];
	int addr = 0x8; // The I2C address

	snprintf(filename, 19, "/dev/i2c-%d", adapter_nr);
	file = open(filename, O_RDWR);
	if (file < 0) {
		/* ERROR HANDLING; you can check errno to see what went wrong */
		error();
		exit(1);
	}


	if (ioctl(file, I2C_SLAVE, addr) < 0) {
		/* ERROR HANDLING; you can check errno to see what went wrong */
		error();
		exit(1);
	}

	__u8 reg = 0x08;
	__s32 res;

	/* Using SMBus commands */
	res = i2c_smbus_read_byte(file);
	if (res < 0) {
		/* ERROR HANDLING: i2c transaction failed */
		error();
		exit(1);
	} else {
		/* res contains the read word */
		cout << "res: " << hex << res << endl;
	}

	res = i2c_smbus_write_byte(file, (rand() % 255));
	if (res < 0) {
		/* ERROR HANDLING: i2c transaction failed */
		error();
		exit(1);
	} else {
		/* res contains the read word */
		cout << "res: " << hex << res << endl;
	}


	/* Using SMBus commands */
	res = i2c_smbus_read_byte(file);
	if (res < 0) {
		/* ERROR HANDLING: i2c transaction failed */
		error();
		exit(1);
	} else {
		/* res contains the read word */
		cout << "res: " << hex << res << endl;
	}
	

	return 0;

}

int i2c_word(){

	int file;
	int adapter_nr = 1; // probably dynamically determined
	char filename[20];
	int addr = 0x8; // The I2C address

	__u8 reg = 0x08;
	__s32 res;
	__u8 buf[I2C_SMBUS_BLOCK_MAX];

	snprintf(filename, 19, "/dev/i2c-%d", adapter_nr);
	file = open(filename, O_RDWR);
	if (file < 0) {
		/* ERROR HANDLING; you can check errno to see what went wrong */
		error();
		exit(1);
	}

	if (ioctl(file, I2C_SLAVE, addr) < 0) {
		/* ERROR HANDLING; you can check errno to see what went wrong */
		error();
		exit(1);
	}

	res = i2c_smbus_read_block_data(file, reg, buf);
	if (res < 0) {
		/* ERROR HANDLING: i2c transaction failed */
		error( "ERROR HANDLING: i2c transaction failed. read_block 1" );
		exit(1);
	} else {
		/* res contains the read word */
		cout << "res: " << hex << res << endl;
	}

	res = i2c_smbus_write_word_data(file, addr, (rand() % 32577));
	if (res < 0) {
		/* ERROR HANDLING: i2c transaction failed */
		error( "ERROR HANDLING: i2c transaction failed. write word data" );
		exit(1);
	} else {
		/* res contains the read word */
		cout << "res: " << hex << res << endl;
	}


	res = i2c_smbus_read_block_data(file, reg, buf);
	if (res < 0) {
		/* ERROR HANDLING: i2c transaction failed */
		error( "ERROR HANDLING: i2c transaction failed. read_block 2" );
		exit(1);
	} else {
		/* res contains the read word */
		cout << "res: " << hex << res << endl;
	}
	

	return 0;

}

int i2c_send_byte( char byte_to_send = 0x00 ){

	int file;
	int adapter_nr = 1; // probably dynamically determined
	char filename[20];
	int addr = 0x8; // The I2C address

	__u8 reg = 0x08;
	__s32 res;

	snprintf(filename, 19, "/dev/i2c-%d", adapter_nr);
	file = open(filename, O_RDWR);
	if (file < 0) {
		/* ERROR HANDLING; you can check errno to see what went wrong */
		error();
		exit(1);
	}


	if (ioctl(file, I2C_SLAVE, addr) < 0) {
		/* ERROR HANDLING; you can check errno to see what went wrong */
		error();
		exit(1);
	}


	res = i2c_smbus_write_byte(file, byte_to_send);
	if (res < 0) {
		/* ERROR HANDLING: i2c transaction failed */
		error();
		exit(1);
	}

	return 0;

}


int i2c_print_byte(){

	int file;
	int adapter_nr = 1; // probably dynamically determined
	char filename[20];
	int addr = 0x8; // The I2C address

	__u8 reg = 0x08;
	__s32 res;

	snprintf(filename, 19, "/dev/i2c-%d", adapter_nr);
	file = open(filename, O_RDWR);
	if (file < 0) {
		/* ERROR HANDLING; you can check errno to see what went wrong */
		error();
		exit(1);
	}


	if (ioctl(file, I2C_SLAVE, addr) < 0) {
		/* ERROR HANDLING; you can check errno to see what went wrong */
		error();
		exit(1);
	}

	/* Using SMBus commands */
	res = i2c_smbus_read_byte(file);
	if (res < 0) {
		/* ERROR HANDLING: i2c transaction failed */
		error();
		exit(1);
	} else {
		/* res contains the read word */
		cout << "res(HEX): " << hex << res << "  res(DEC): " << dec << res << endl;
	}

	return 0;

}

int set_servos( int left, int right, bool print = false ) {

	if ( print ) {
		i2c_send_byte( 0x00  );
		i2c_print_byte();
		i2c_send_byte( 0xFF  );
		i2c_print_byte();
		i2c_send_byte( left );
		i2c_print_byte();
		i2c_send_byte( ( SERVO_MAX + SERVO_MIN ) - right );
		i2c_print_byte();
	} else {
		i2c_send_byte( 0x00  );
		i2c_send_byte( 0xFF  );
		i2c_send_byte( left );
		i2c_send_byte( ( SERVO_MAX + SERVO_MIN ) - right );
	}
	return 0;

}

char getch() {
        char buf = 0;
        struct termios old = {0};
        if (tcgetattr(0, &old) < 0)
                perror("tcsetattr()");
        old.c_lflag &= ~ICANON;
        old.c_lflag &= ~ECHO;
        old.c_cc[VMIN] = 1;
        old.c_cc[VTIME] = 0;
        if (tcsetattr(0, TCSANOW, &old) < 0)
                perror("tcsetattr ICANON");
        if (read(0, &buf, 1) < 0)
                perror ("read()");
        old.c_lflag |= ICANON;
        old.c_lflag |= ECHO;
        if (tcsetattr(0, TCSADRAIN, &old) < 0)
                perror ("tcsetattr ~ICANON");
        return (buf);
}

int main(){

	// initialize random seed
	srand (time(NULL));

	set_servos( 150, 150 );
	//sleep(5);

	cout << "Taking Input: ";
	char key = getch();
	while( key != 'q' ) {
		switch ( key ){
			case 'w':
				set_servos( SERVO_MAX, SERVO_MAX );
				break;
			case 's':
				set_servos( SERVO_MIN, SERVO_MIN );
				break;
			case 'a':
				set_servos( SERVO_MIN, SERVO_MAX );
				break;
			case 'd':
				set_servos( SERVO_MAX, SERVO_MIN );
				break;
			case ' ':
				set_servos( SERVO_MID, SERVO_MID );
				break;
			default:
				set_servos( SERVO_MID, SERVO_MID );
		}

		sleep(1);
		key = getch();

	}

	set_servos( SERVO_MID, SERVO_MID );
	std::cout << "done!" << std::endl; 
    
    return 0;
}

