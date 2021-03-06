

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

// main.cpp


#include <SFML/Graphics.hpp>
#include <math.h>
#include <iostream>
#include <chrono>
#include <thread>

#include <rplidar.h>
#include <signal.h>

extern "C" {
	#include <linux/i2c-dev.h>
	#include <i2c/smbus.h>
}

#ifndef _countof
#define _countof(_Array) (int)(sizeof(_Array) / sizeof(_Array[0]))
#endif

#ifndef LIDAR_MAX_VALUE_IN_MILLIMETERS
#define LIDAR_MAX_VALUE_IN_MILLIMETERS 18000
#endif

#ifndef LIDAR_MIN_VALUE_IN_MILLIMETERS
#define LIDAR_MIN_VALUE_IN_MILLIMETERS 200
#endif

#ifndef LIDAR_MAX_VALUE_IN_METERS
#define LIDAR_MAX_VALUE_IN_METERS 18
#endif

#ifndef LIDAR_MIN_VALUE_IN_METERS
#define LIDAR_MIN_VALUE_IN_METERS 0.2
#endif

// Name space
using namespace rp::standalone::rplidar;

//Screen Size
int window_height = 1080; 
int window_width = 1920;

bool checkRPLIDARHealth(RPlidarDriver * drv) {
    u_result     op_result;
    rplidar_response_device_health_t healthinfo;

    op_result = drv->getHealth(healthinfo);
    if (IS_OK(op_result)) { // the macro IS_OK is the preperred way to judge whether the operation is succeed.
        printf("RPLidar health status : %d\n", healthinfo.status);
        if (healthinfo.status == RPLIDAR_STATUS_ERROR) {
            fprintf(stderr, "Error, rplidar internal error detected. Please reboot the device to retry.\n");
            // enable the following code if you want rplidar to be reboot by software
            // drv->reset();
            return false;
        } else {
            return true;
        }

    } else {
        fprintf(stderr, "Error, cannot retrieve the lidar health code: %x\n", op_result);
        return false;
    }
}

std::vector<sf::CircleShape> get_poly_shape( int count, int window_x, int window_y, int base_radius, std::vector< float > theta, std::vector < float > distance, float scale_factor = 1, bool is_circle = true ) {

    //sf::VertexArray poly_circle(sf::LineStrip);
    //sf::VertexArray poly_circle(sf::Points);
    std::vector<sf::CircleShape> poly_circle;
    poly_circle.resize(count);
    int x, y;

    for ( int _pos = 0; _pos < count; _pos++ ) {
        int pos = _pos;

        if ( is_circle ) {

            // Lame attempt to remove errors and make it look pretty
            /*
               if ( distance[pos] <= 0 ) {
               int i = 0;
               while ( distance[pos+i] <= 0  && pos+i < count ) {
               i++;
               }
               distance[pos] = distance[pos+i];
               }

               distance[pos] = round( distance[pos] / 18 );
               theta[pos] = round(theta[pos]);
               */

            x = ( ( window_width / 2 ) + ( base_radius + distance[pos] ) * sin( theta[pos] * M_PI / 180 ) * scale_factor);
            y = ( ( window_height / 2 ) - ( base_radius + distance[pos] ) * cos( theta[pos] * M_PI / 180 ) * scale_factor);

        } else {
            x = ( window_width / count ) * pos;
            y = window_height - distance[pos];
        }

        poly_circle[pos].setRadius(2);
        poly_circle[pos].setOutlineColor(sf::Color::Blue);
        poly_circle[pos].setOutlineThickness(2);
        poly_circle[pos].setPosition( sf::Vector2f( x, y ) );
        poly_circle[pos].setFillColor( sf::Color::Magenta );

    }

    return poly_circle;
}

int main(int argc, const char * argv[]) {
    sf::RenderWindow window(sf::VideoMode(window_width, window_height), "LIDAR Visualizer");
    //window.setFramerateLimit(9);

	Servos servos;
	servos.set(SERVO_MID, SERVO_MID ); // TODO: This fails without warning!!


    const char * opt_com_path = NULL;
    _u32         baudrateArray[2] = {115200, 256000};
    _u32         opt_com_baudrate = 0;
    u_result     op_result;

    bool useArgcBaudrate = false;

	// Print RPLIDAR Version
	std::cout << "RPLIDAR Version: " << RPLIDAR_SDK_VERSION << std::endl;

    // read serial port from the command line...
    if (argc>1) opt_com_path = argv[1]; // or set to a fixed value: e.g. "com3" 

    // read baud rate from the command line if specified...
    if (argc>2) {
        opt_com_baudrate = strtoul(argv[2], NULL, 10);
        useArgcBaudrate = true;
    }

    if (!opt_com_path) {
#ifdef _WIN32
        // use default com port
        opt_com_path = "\\\\.\\com57";
#elif __APPLE__
        opt_com_path = "/dev/tty.SLAB_USBtoUART";
#else
        opt_com_path = "/dev/ttyUSB0";
#endif
    }

    // create the driver instance
    RPlidarDriver * drv = RPlidarDriver::CreateDriver(DRIVER_TYPE_SERIALPORT);
    if (!drv) {
        fprintf(stderr, "insufficent memory, exit\n");
        exit(-2);
    }

    rplidar_response_device_info_t devinfo;
    bool connectSuccess = false;
    // make connection...
    if(useArgcBaudrate) {
        if(!drv)
            drv = RPlidarDriver::CreateDriver(DRIVER_TYPE_SERIALPORT);
        if (IS_OK(drv->connect(opt_com_path, opt_com_baudrate))) {
            op_result = drv->getDeviceInfo(devinfo);
            if (IS_OK(op_result)) {
                connectSuccess = true;
			} else {
                delete drv;
                drv = NULL;
            }
        }
    } else {
        size_t baudRateArraySize = (sizeof(baudrateArray))/ (sizeof(baudrateArray[0]));
        for(size_t i = 0; i < baudRateArraySize; ++i) {
            if(!drv) { drv = RPlidarDriver::CreateDriver(DRIVER_TYPE_SERIALPORT); }
            if(IS_OK(drv->connect(opt_com_path, baudrateArray[i]))) {
                op_result = drv->getDeviceInfo(devinfo);
                if (IS_OK(op_result)) {
                    connectSuccess = true;
                    break;
                } else {
                    delete drv;
                    drv = NULL;
                }
            }
        }
    }
    if (!connectSuccess) {

        fprintf(stderr, "Error, cannot bind to the specified serial port %s.\n"
                , opt_com_path);
        //goto on_finished;
    }

    // start scan...
    drv->startMotor();
    drv->startScan(0,1);
    drv->setMotorPWM(700); 


    // Create Poly Circle
    sf::Vector2u size = window.getSize();
    float scale_factor = 1;

    while (window.isOpen( ) ) {

        std::vector< float > theta;
        std::vector< float > distance;
        std::vector< sf::CircleShape > poly_circle;

        rplidar_response_measurement_node_hq_t nodes[8192];
        size_t   count = _countof(nodes);

        op_result = drv->grabScanDataHq(nodes, count);

        if (IS_OK(op_result)) {
            drv->ascendScanData(nodes, count);
            window.clear(sf::Color::Black);

            for (int pos = 0; pos < count ; ++pos) {
                theta.push_back( round( nodes[pos].angle_z_q14 * 90.f / (1 << 14) * 1000) / 1000 );
                distance.push_back( nodes[pos].dist_mm_q2 / 4.0f );
            }

            //int window_x, int window_y, int base_radius, float theta, float distance, float scale_factor, bool is_circle
            poly_circle = get_poly_shape( count, size.x, size.y, 5, theta, distance, scale_factor );

            sf::Event event;
            while ( window.pollEvent( event ) ) {
                switch ( event.type ) {
                    case sf::Event::Closed:
                        window.close();
                        break;
                    case sf::Event::KeyPressed:
                        std::cout << "Key Pressed" << std::endl;
                        switch ( event.key.code ) {
							case sf::Keyboard::W:
								servos.set( SERVO_MAX, SERVO_MAX );
								break;
							case sf::Keyboard::A:
								servos.set( SERVO_MIN, SERVO_MAX );
								break;
							case sf::Keyboard::S:
								servos.set( SERVO_MIN, SERVO_MIN );
								break;
							case sf::Keyboard::D:
								servos.set( SERVO_MAX, SERVO_MIN );
								break;
							case sf::Keyboard::Q:
								servos.set( SERVO_MID, SERVO_MID );
								break;
                            case sf::Keyboard::RBracket:
                                scale_factor = scale_factor * 0.8;
                                break;
                            case sf::Keyboard::LBracket:
                                scale_factor = scale_factor * 1.2;
                                break;
                            default:
                                std::cout << "Unresolved key pressed: " << event.key.code << std::endl;
                                break;
                        }
                        break;
                    default:
                        break;
                }


            }

                for ( int i = 0; i < poly_circle.size(); i ++ ) window.draw( poly_circle[i] );
                window.display();
        }

            //std::this_thread::sleep_for(std::chrono::milliseconds(600)); 

	}

	drv->stop();
	drv->stopMotor();

	return 0;
}
