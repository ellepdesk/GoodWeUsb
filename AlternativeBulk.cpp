#include <iostream>
#include <unistd.h> 
#include <libusb-1.0/libusb.h>
using namespace std;

//From: http://www.dreamincode.net/forums/topic/148707-introduction-to-using-libusb-10/
//Installed: sudo apt-get install libusb-1.0-0-dev

int main() {
	libusb_device **devs; //pointer to pointer of device, used to retrieve a list of devices
	libusb_device_handle *dev_handle; //a device handle
	libusb_context *ctx = NULL; //a libusb session
	int r; //for return values
	int i; // counter
	int j; // counter interupt
	ssize_t cnt; //holding number of devices in list
	r = libusb_init(&ctx); //initialize the library for the session we just declared
	if(r < 0) {
		cout<<"Init Error "<<r<<endl; //there was an error
		return 1;
	}
	libusb_set_debug(ctx, 3); //set verbosity level to 3, as suggested in the documentation

	cnt = libusb_get_device_list(ctx, &devs); //get the list of devices
	if(cnt < 0) {
		cout<<"Get Device Error"<<endl; //there was an error
		return 1;
	}
	cout<<cnt<<" Devices in list."<<endl;
	//GoodWe 0x0084, 0x0041,
	// original 5118, 7424
	dev_handle = libusb_open_device_with_vid_pid(ctx, 0x0084, 0x0041); //these are vendorID and productID I found for my usb device
	if(dev_handle == NULL)
		cout<<"Cannot open device"<<endl;
	else
		cout<<"Device Opened"<<endl;
	libusb_free_device_list(devs, 1); //free the list, unref the devices in it

	// Control 1 	cc99 09aa 5580 7f00 0000 01fe 0000..00
	// Control 2 	cc99 1aaa 5580 7f00 0111 3134 3230 3044 5355 3135 3630 3030 3638 1105 9e00 0000..00
	// Rep Control: cc99 09aa 5580 1101 0100 0192 0000..00
	unsigned char *dataControl1 = new unsigned char[72]; //data to write
	unsigned char *dataControl2 = new unsigned char[72]; //data to write
	unsigned char *dataControlR = new unsigned char[72]; //data to write
	for (i=0;i<=72;i++)
	{
		dataControl1[i]=0x00;
		dataControl2[i]=0x00;
		dataControlR[i]=0x00;
	}
		
	// Control 1 	cc99 09aa 5580 7f00 0000 01fe 0000..00
	dataControl1[0]=0xcc;
	dataControl1[1]=0x99;
	dataControl1[2]=0x09;
	dataControl1[3]=0xaa;
	dataControl1[4]=0x55;
	dataControl1[5]=0x80;
	dataControl1[6]=0x7f;
	dataControl1[7]=0x00;
	dataControl1[8]=0x00;
	dataControl1[9]=0x00;
	dataControl1[10]=0x01;
	dataControl1[11]=0xfe;
	
	// Control 2 	cc99 1aaa 5580 7f00 0111 3134 3230 3044 5355 3135 3630 3030 3638 1105 9e00 0000..00
	// ASCII		- -  - -  - -  - -  - -  1 4  2 0  0 D  S U  1 5  6 0  0 0  6 8  - -  -   
	dataControl2[0]=0xcc;
	dataControl2[1]=0x99;
	dataControl2[2]=0x1a;
	dataControl2[3]=0xaa;
	dataControl2[4]=0x55;
	dataControl2[5]=0x80;
	dataControl2[6]=0x7f;
	dataControl2[7]=0x00;
	dataControl2[8]=0x01;
	dataControl2[9]=0x11;
	dataControl2[10]=0x01;
	dataControl2[11]=0xfe;
	
	// Rep Control: cc99 09aa 5580 1101 0100 0192 0000..00
	dataControlR[0]=0xcc;
	dataControlR[1]=0x99;
	dataControlR[2]=0x09;
	dataControlR[3]=0xaa;
	dataControlR[4]=0x55;
	dataControlR[5]=0x80;
	dataControlR[6]=0x11;
	dataControlR[7]=0x01;
	dataControlR[8]=0x01;
	dataControlR[9]=0x00;
	dataControlR[10]=0x01;
	dataControlR[11]=0x92;

	
	unsigned char *readData = new unsigned char[72]; //data to write
	for (i=0;i<=72;i++)
	{
		readData[i]=0x00;
	}
	
	int actual; //used to find out how many bytes were written
	if(libusb_kernel_driver_active(dev_handle, 0) == 1) { //find out if kernel driver is attached
		cout<<"Kernel Driver Active"<<endl;
		if(libusb_detach_kernel_driver(dev_handle, 0) == 0) //detach it
			cout<<"Kernel Driver Detached!"<<endl;
	}
	
	/*r= libusb_set_configuration(dev_handle,1);// set active configuration on device BEFORE claiming interface
	if(r < 0) {
		cout<<"Cannot Configure Interface "<<r<<endl;
		return 1;
	}
	cout<<"Configured Interface"<<endl;
	*/
	
	r = libusb_claim_interface(dev_handle, 0); //claim interface 0 (the first) of device (mine had jsut 1)
	if(r < 0) {
		cout<<"Cannot Claim Interface"<<endl;
		return 1;
	}
	cout<<"Claimed Interface"<<endl;
	
	//**************** CONTROL 1 *******************/
	
	
	//libusb_control_transfer(libusb_device_handle * 	dev_handle,uint8_t 	bmRequestType,uint8_t 	bRequest,uint16_t 	wValue,uint16_t 	wIndex,unsigned char * 	data,uint16_t 	wLength,unsigned int 	timeout )
	r = libusb_control_transfer(dev_handle, 0x21, 9, 0x0200, 0x0, dataControl1, 72, 10000);
	if(r >= 0 ) //control succesful
		cout<<"Control Successful! &&"<<dataControl1<<"&&"<<endl;
	else
		cout<<"Control Error "<<r<<endl;
	
	usleep(200000);
	// READING
	//int libusb_interrupt_transfer	(	struct libusb_device_handle * 	dev_handle,unsigned char 	endpoint,unsigned char * 	data,int 	length,int * 	transferred,unsigned int 	timeout )	
	
	cout<<"Reading Data..."<<endl;
	for (j=0;j<=7;j++){
		r = libusb_interrupt_transfer(dev_handle, (1 | LIBUSB_ENDPOINT_IN), readData, 8, &actual, 0); //my device's out endpoint was 2, found with trial- the device had 2 endpoints: 2 and 129
		if(r == 0){// && actual == 8) { //we wrote the 4 bytes successfully
			cout<<"Reading interupt  Successful!"<<endl;
		}
		else {
			cout<<"Read interupt Error "<<r<<" Actual: "<<actual<<endl;
		}

		cout<<j<<" #"<<actual<<": ";
		for (i=0;i<actual;i++) {
			cout<<hex<<(int)readData[i]<<" ";
		}
		cout<<endl;
	}
	
	//**************** CONTROL 2 *******************/
		
	//libusb_control_transfer(libusb_device_handle * 	dev_handle,uint8_t 	bmRequestType,uint8_t 	bRequest,uint16_t 	wValue,uint16_t 	wIndex,unsigned char * 	data,uint16_t 	wLength,unsigned int 	timeout )
	r = libusb_control_transfer(dev_handle, 0x21, 9, 0x0200, 0x0, dataControl2, 72, 10000);
	if(r >= 0 ) //control succesful
		cout<<"Control Successful! &&"<<dataControl2<<"&&"<<endl;
	else
		cout<<"Control Error "<<r<<endl;
	usleep(200000);
	// READING
	//int libusb_interrupt_transfer	(	struct libusb_device_handle * 	dev_handle,unsigned char 	endpoint,unsigned char * 	data,int 	length,int * 	transferred,unsigned int 	timeout )	
	
	cout<<"Reading Data..."<<endl;
	for (j=0;j<=7;j++){
		r = libusb_interrupt_transfer(dev_handle, (1 | LIBUSB_ENDPOINT_IN), readData, 8, &actual, 0); //my device's out endpoint was 2, found with trial- the device had 2 endpoints: 2 and 129
		if(r == 0){// && actual == 8) { //we wrote the 4 bytes successfully
			cout<<"Reading interupt  Successful!"<<endl;
		}
		else {
			cout<<"Read interupt Error "<<r<<" Actual: "<<actual<<endl;
		}

		cout<<j<<" #"<<actual<<": ";
		for (i=0;i<actual;i++) {
			cout<<hex<<(int)readData[i]<<" ";
		}
		cout<<endl;
	}
	
	
	
	
	
	/*
	cout<<"Data->"<<data<<"<-"<<endl; //just to see the data we want to write : abcd
	cout<<"Writing Data..."<<endl;
	r = libusb_bulk_transfer(dev_handle, (2 | LIBUSB_ENDPOINT_OUT), data, 4, &actual, 0); //my device's out endpoint was 2, found with trial- the device had 2 endpoints: 2 and 129
	if(r == 0 && actual == 4) //we wrote the 4 bytes successfully
		cout<<"Writing Successful!"<<endl;
	else
		cout<<"Write Error"<<endl;
	*/
		
	r = libusb_release_interface(dev_handle, 0); //release the claimed interface
	if(r!=0) {
		cout<<"Cannot Release Interface"<<endl;
		return 1;
	}
	cout<<"Released Interface"<<endl;

	libusb_close(dev_handle); //close the device we opened
	libusb_exit(ctx); //needs to be called to end the

	delete[] dataControl1; //delete the allocated memory for data
	delete[] dataControl2; //delete the allocated memory for data
	delete[] dataControlR; //delete the allocated memory for data
	delete[] readData; //delete the allocated memory for data
	
	return 0;
}
