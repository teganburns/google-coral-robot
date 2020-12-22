// google-coral-servos.cpp

#include "google-coral-servos.hpp"


void Servos::i2c_send_byte( char byte_to_send ){

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
		//error();
		exit(1);
	}


	if (ioctl(file, I2C_SLAVE, addr) < 0) {
		/* ERROR HANDLING; you can check errno to see what went wrong */
		//error();
		exit(1);
	}


	res = i2c_smbus_write_byte(file, byte_to_send);
	if (res < 0) {
		/* ERROR HANDLING: i2c transaction failed */
		//error();
		exit(1);
	}


}

void Servos::i2c_print_byte(){

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
		//error();
		exit(1);
	}


	if (ioctl(file, I2C_SLAVE, addr) < 0) {
		/* ERROR HANDLING; you can check errno to see what went wrong */
		//error();
		exit(1);
	}

	/* Using SMBus commands */
	res = i2c_smbus_read_byte(file);
	if (res < 0) {
		/* ERROR HANDLING: i2c transaction failed */
		//error();
		exit(1);
	} else {
		/* res contains the read word */
		std::cout << "res(HEX): " << std::hex << res << "  res(DEC): " << std::dec << res << std::endl;
	}

}

void Servos::set( int left, int right ) {

	i2c_send_byte( 0x00  );
	i2c_send_byte( 0xFF  );
	i2c_send_byte( left );
	i2c_send_byte( ( SERVO_MAX + SERVO_MIN ) - right );

}

void Servos::set_print( int left, int right ) {

	i2c_send_byte( 0x00  );
	i2c_print_byte();
	i2c_send_byte( 0xFF  );
	i2c_print_byte();
	i2c_send_byte( left );
	i2c_print_byte();
	i2c_send_byte( ( SERVO_MAX + SERVO_MIN ) - right );
	i2c_print_byte();

}
