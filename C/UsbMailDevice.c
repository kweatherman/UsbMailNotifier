
// Minimalist USB "Mail Notifier" LED device support
#define WIN32_LEAN_AND_MEAN
#define WINVER       0x0A00 // _WIN32_WINNT_WIN10
#define _WIN32_WINNT 0x0A00
#include <windows.h>
#include <stdio.h>

#include <SetupAPI.h>
#pragma comment(lib, "SetupAPI.lib")

#ifdef __cplusplus
extern "C"
{
#endif
#define INITGUID
#include <hidsdi.h>
#include <hidclass.h>
#ifdef __cplusplus
}
#endif
#pragma comment(lib, "hid.lib")

// Open the first supported HID LED mail light device if any
HANDLE OpenMailDevice()
{
	HANDLE result = NULL;

	// Get HID device information set
	HDEVINFO devNfoSet = SetupDiGetClassDevsA(&GUID_DEVINTERFACE_HID, NULL, NULL, (DIGCF_PRESENT | DIGCF_DEVICEINTERFACE));
	if (devNfoSet != INVALID_HANDLE_VALUE)
	{
		// Iterate over HID devices..
		for(DWORD i = 0; ; i++)
		{
			// Next device by index
			SP_DEVICE_INTERFACE_DATA DeviceInterfaceData;
			DeviceInterfaceData.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);
			if (!SetupDiEnumDeviceInterfaces(devNfoSet, NULL, &GUID_DEVINTERFACE_HID, i, &DeviceInterfaceData))
				// No more devices..
				break;
			else
			{
				// Get registered device path
				BYTE pathBuffer[256];
				PSP_DEVICE_INTERFACE_DETAIL_DATA_A deviceDetails = (PSP_DEVICE_INTERFACE_DETAIL_DATA_A) pathBuffer;
				deviceDetails->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA_A);

				if (SetupDiGetDeviceInterfaceDetailA(devNfoSet, &DeviceInterfaceData, deviceDetails, sizeof(pathBuffer), NULL, NULL))
				{
					//printf("DevicePath: \"%s\"\n", deviceDetails->DevicePath);

					// Open an overlapped R/W handle to the device
					HANDLE deviceHandle = CreateFileA(deviceDetails->DevicePath, (GENERIC_WRITE | GENERIC_READ), 0, NULL, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, 0);
					if (deviceHandle != INVALID_HANDLE_VALUE)
					{
						// Get the Vendor ID and Product ID for it
						HIDD_ATTRIBUTES attrib;
						attrib.Size = sizeof(HIDD_ATTRIBUTES);
						if (HidD_GetAttributes(deviceHandle, &attrib))
						{
							//printf("Product/Vendor: %04X %04X\n", Attrib.ProductID, Attrib.VendorID);

							// Match wanted mail notifier device?
							// Could also match by manufacture or product string as L"MAIL "
							static const WORD MailVendorID = 0x1294, MailProductID = 0x1320;
							if ((attrib.VendorID == MailVendorID) && (attrib.ProductID == MailProductID))
							{
								// Yes, break out with this handle
								result = deviceHandle;
								break;
							}
						}

						CloseHandle(deviceHandle);
						deviceHandle = NULL;
					}
				}
			}
		}

		SetupDiDestroyDeviceInfoList(devNfoSet);
		devNfoSet = NULL;
	}

	return result;
}

// Close the device handle
void CloseMailDevice(HANDLE deviceHandle)
{
	if (deviceHandle)
	{
		CancelIo(deviceHandle);
		CloseHandle(deviceHandle);
	}
}

// Set mail device color
void SetMailDeviceColor(HANDLE deviceHandle, UINT color)
{
	if (deviceHandle)
	{
		// HID control packet
		BYTE packet[6] = { 2, (BYTE) (color & 7), 0, 0, 0, 0};

		// Overlapped IO to avoid blocking this thread for several milliseconds writing to the slow device
		OVERLAPPED ol;
		ZeroMemory(&ol, sizeof(ol));
		WriteFile(deviceHandle, packet, sizeof(packet), NULL, &ol);
	}
}