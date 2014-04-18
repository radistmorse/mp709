/*******************************************************
 *  mp709 - control your mp709 over usb
 *
 *  Vsevolod Potapov
 *  AdRiver
 *  Radist Morse
 *
 *  10/08/2011
 *
 *        Copyright 2011-2014, All Rights Reserved.
 *         
 *  This software may be licensed under the terms of the
 *  GNU Public License v3.
 *
 ******************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hidapi.h"



int main(int argc, char* argv[])
{
	unsigned char buf[65];
	//#define MAX_STR 255
	//wchar_t wstr[MAX_STR];
	hid_device *handle;
	int i=0;
	struct hid_device_info *devs, *cur_dev;

	devs = hid_enumerate(0x16c0, 0x05df);
	cur_dev = devs;
	while (cur_dev) {
		i++;
		//parsing input here (yes, for every device anew)
		int arg_on_off = 0;
		int on_off = 0;
		for (int arg = 1; arg < argc; arg++){
			//we look which command was the most recent
			if(strcmp(argv[arg], "on") == 0) {
				arg_on_off = 1;
				continue;
			}
			if(strcmp(argv[arg], "off") == 0) {
				arg_on_off = -1;
				continue;
			}

			//"all" means that the most recent command applies to all
			if(strcmp(argv[arg], "all") == 0) {
				on_off = arg_on_off;
				continue;
			}

			//it's not a command, hence it's the device
			int device;
			if (sscanf (argv[arg], "%i", &device)!=1) {
				//match by path
				if (strcmp(argv[arg], cur_dev->path) == 0)
						on_off = arg_on_off;
			} else {
				//match by int
				if (device == i)
					on_off = arg_on_off;
			}
		}

		//by this point we have on_off set to 0,-1 or 1
		handle = hid_open_path(cur_dev->path);
		char state[20] = "unknown";
		if (handle) {
			//some magic
			buf[0] = 0x00; // First byte is report number
			buf[1] = 0x1d;
			buf[2] = 0x00;
			hid_send_feature_report(handle, buf, 8);
			hid_get_feature_report(handle, buf, sizeof(buf));

			//issue the command if needed
			if (on_off != 0) {
				memset(buf, sizeof(buf), 0);
				buf[1] = 0xe7;

				if(on_off == 1) //on
					buf[2] = 0x0;
				if(on_off == -1) //off
					buf[2] = 0x19;

				hid_send_feature_report(handle, buf, 8);
			}

			//check the result
			memset(buf, sizeof(buf), 0);
			buf[1] = 0x7e;
			hid_send_feature_report(handle, buf, 8);
			hid_get_feature_report(handle, buf, sizeof(buf));
			if (buf[2] == 0x19)
				sprintf(state, "off");
			else if (buf[2] == 0)
				sprintf(state, "on");
		} else {
			if (on_off != 0) {
				fprintf(stderr,"Unable to change the state of the device number %d path: %s\n", i++, cur_dev->path);
			}
		}
		hid_close(handle);
		printf("Device number %d path: %s state: %s\n", i++, cur_dev->path, state);


		cur_dev = cur_dev->next;
	}
	hid_free_enumeration(devs);

	return 0;
}

