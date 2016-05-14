#include <hidapi/hidapi.h>
#include <cstdio>
#include <cstring>

main()
{	
	int res = 0;
	unsigned char buf[256];
	#define MAX_STR 255
	wchar_t wstr[MAX_STR];
	hid_device *handle;
	int i;

	// Set up the command buffer.
	memset(buf,0x00,sizeof(buf));
	buf[0] = 0x01;
	buf[1] = 0x81;
	

	// Open the device using the VID, PID,
	// and optionally the Serial number.
	////handle = hid_open(0x4d8, 0x3f, L"12345");
	handle = hid_open(0x0084, 0x0041, NULL);
	if (!handle) {
		printf("unable to open device \n");
 		return 1;
	}

	// Read the Manufacturer String
	wstr[0] = 0x0000;
	res = hid_get_manufacturer_string(handle, wstr, MAX_STR);
	if (res < 0)
		printf("Unable to read manufacturer string\n");
	printf("Manufacturer String: %ls\n", wstr);

	// Read the Product String
	wstr[0] = 0x0000;
	res = hid_get_product_string(handle, wstr, MAX_STR);
	if (res < 0)
		printf("Unable to read product string\n");
	printf("Product String: %ls\n", wstr);

	// Read the Serial Number String
	wstr[0] = 0x0000;
	res = hid_get_serial_number_string(handle, wstr, MAX_STR);
	if (res < 0)
		printf("Unable to read serial number string\n");
	printf("Serial Number String: (%d) %ls", wstr[0], wstr);
	printf("\n");

	// Read Indexed String 1
	wstr[0] = 0x0000;
	res = hid_get_indexed_string(handle, 1, wstr, MAX_STR);
	if (res < 0)
		printf("Unable to read indexed string 1\n");
	printf("Indexed String 1: %ls\n", wstr);

	// Set the hid_read() function to be non-blocking.
	hid_set_nonblocking(handle, 1);
	
	// Try to read from the device. There shoud be no
	// data here, but execution should not block.
	res = hid_read(handle, buf, 17);
}
