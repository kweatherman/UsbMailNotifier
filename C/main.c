
// USB "Mail Notifier" LED device example
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#include "UsbMailDevice.h"

int main(int argc, char *argv[])
{
	// Locate the Mail notifier device is there is one and get a handle for it
	printf("Locating the Mail Notifier USB HID device:\n");
	HANDLE deviceHandle = OpenMailDevice();
	if (deviceHandle)
	{
		printf("Device located.\n");

		// Cycle through the available colors twice
		printf("\nCycling colors..\n");
		for (UINT i = 0; i < 2; i++)
		{
			for (UINT color = MAIL_COLOR_NONE; color <= MAIL_COLOR_WHITE; color++)
			{
				SetMailDeviceColor(deviceHandle, color);
				Sleep(500);
			}
		}

		// Flash cycle through the available colors
		printf("\nFlashing colors..\n");
		for (UINT color = MAIL_COLOR_BLUE; color <= MAIL_COLOR_WHITE; color++)
		{
			for (UINT i = 0; i < 4; i++)
			{
				SetMailDeviceColor(deviceHandle, color);
				Sleep(140);
				SetMailDeviceColor(deviceHandle, MAIL_COLOR_NONE);
				Sleep(140);
			}
		}

		// Done with the device
		CloseMailDevice(deviceHandle);
		deviceHandle = NULL;
		printf("Done.\n");
	}
	else
		printf("** A USB Mail LED HID device not found! ***");

	return 0;
}
