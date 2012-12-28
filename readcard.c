#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <i2c-dev.h>
#include "i2cbusses.h"
#include "appconfig.h"

/**
 * Try and read a card from the device.
 * cardid = buffer to put card id into
 * cardidsz = size of cardid buffer
 * if the card was read successfully the return value will be
 * 0 and the size of the read id from the card will be stored in
 * cardidsz.
 */
int ReadCard(unsigned char* cardid, size_t* cardidsz) {
	int file;
	int i2cbus = 0;
	int res;
	int i;
	char filename[128];
	unsigned char write_buffer[2];
	unsigned char read_buffer[0x10];
	struct i2c_rdwr_ioctl_data i2c_rdwr;
	struct i2c_msg actual_msgs[2];

	file = open_i2c_dev(i2cbus, filename, sizeof(filename), 0);

	// Select Device
	if (ioctl(file, I2C_SLAVE, 0x50) < 0) {
		usleep(WAITING_DELAY);
		return 1;
	}

	i2c_rdwr.msgs = actual_msgs;
	i2c_rdwr.nmsgs = 1;

	actual_msgs[0].addr = 0x50;
	actual_msgs[0].flags = 0;
	actual_msgs[0].len = 2;
	write_buffer[0] = 0x01;
	write_buffer[1] = 0x01;
	actual_msgs[0].buf = write_buffer;

	actual_msgs[1].addr = 0x50;
	actual_msgs[1].flags = I2C_M_RD;
	actual_msgs[1].len = sizeof(read_buffer);
	actual_msgs[1].buf = read_buffer;

	memset(read_buffer, 0, sizeof(read_buffer));

	res = 1;

	// this takes more than one try!
	for (i = 0; i < 20; i++) {

	// Write command to the device to read a card
	if (ioctl(file, I2C_RDWR, &i2c_rdwr) < 0) {
		// If it fails, do something here like log??
		usleep(WAITING_DELAY);
	} else {
		// Delay before trying to read from the device
		// This value was discovered with trial and error
		usleep(READING_DELAY);
		// now read the result.
		i2c_rdwr.msgs = &actual_msgs[1];
		if (ioctl(file, I2C_RDWR, &i2c_rdwr) < 0) {
			// failed!
			usleep(WAITING_DELAY);
		} else {
			res = 0;
		}
	}

	// end the twenty try loop.
	}

	close(file);

	if (res == 0) {
		// was a card read?
		if (read_buffer[2] == 0) {
			int sz;
			sz = read_buffer[0] - 3;
			memcpy(cardid, read_buffer + 3, sz);
			*cardidsz = sz;
		} else {
			res = 1;
		}
	}

	return res;
}

