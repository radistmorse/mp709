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
	if (argc > 1 && strcmp(argv[1], "--help") == 0) {
		printf("Usage: %s [on|off [<path>|<decimal int>|<hex int>|all]...]...\n", argv[0]);
		printf("\t'on' or 'off' is the command that is issued to all subsequent relays\n");
		printf("\t<path> - determine the relay by path, usually '/dev/hidrawX'\n");
		printf("\t<decimal int> - determine the relay by the internal number.\n");
		printf("\t                Be aware, that this number is not persistent,\n");
		printf("\t                and may change on device reconnection, software upgrade, or reboot.\n");
		printf("\t<hex int> - determine the relay by a unique ID.\n");
		printf("\t            This ID is persistent, and will always identify the same relay.\n");
		printf("\t'all' - apply the command to all relays.\n");
		printf("\n\tCalling without parameters will list all the devices with the corresponding paths, internal numbers and IDs.\n\n");
		printf("Example: %s on all off /dev/hidraw2 3 0x17F\n", argv[0]);
		printf("\tSwitches on all the relays except for three, determined by their path, internal number and unique ID respectively.\n");
		return 0;
	}
	unsigned char buf[65];
	unsigned long device_id;

	struct hid_device_info *devs, *cur_dev;
	hid_device *handle;

	int i=0;

	devs = hid_enumerate(0x16c0, 0x05df);

	if (devs == NULL) {
		printf("No devices found.\n");
		return 0;
	}

	cur_dev = devs;
	while (cur_dev) {
		i++;

		handle = hid_open_path(cur_dev->path);
		if (handle) {
		
			memset(buf, sizeof(buf), 0);
			device_id = 0;
			buf[0] = 0x1d;
			if (hid_send_feature_report(handle, buf, 8) > -1){
				if (hid_get_feature_report(handle, buf, 8) > -1){
					if (buf[0]==0x1d) {
						device_id = (buf[4] << 24) + (buf[5] << 16) + (buf[6] << 8) + buf[7];
						//printf("0x%lx\n", device_id);
					}
				}
			}
			
			if (device_id == 0) {
				fprintf(stderr, "Unable to get unique ID for the device number %d path: %s\n", i, cur_dev->path);
			}
			
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
					if (argv[arg][0] == '0' && argv[arg][1] == 'x') {
						//hex int, it's a unique id
						if (device_id == device)
							on_off = arg_on_off;
					} else {
						//decimal int, match by number
						if (device == i)
							on_off = arg_on_off;
					}
				}
			}
	
			//by this point we have on_off set to 0,-1 or 1
			char state[20] = "unknown";
			//some magic
			buf[0] = 0x00;
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
				
			hid_close(handle);
			if (device_id) {
				printf("Device number %d path: %s unique ID: 0x%lx state: %s\n", i, cur_dev->path, device_id, state);
			} else {
				printf("Device number %d path: %s state: %s\n", i, cur_dev->path, state);
			}
		} else {
			fprintf(stderr, "Unable to access the device number %d path: %s\n", i, cur_dev->path);
		}


		cur_dev = cur_dev->next;
	}
	hid_free_enumeration(devs);

	return 0;
}

