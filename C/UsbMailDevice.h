
// USB "Mail Notifier" LED device support
#pragma once

enum
{
	MAIL_COLOR_NONE,	// None/black/off
	MAIL_COLOR_BLUE,
	MAIL_COLOR_RED,
	MAIL_COLOR_GREEN,
	MAIL_COLOR_CYAN,
	MAIL_COLOR_MAGENTA,
	MAIL_COLOR_YELLOW,
	MAIL_COLOR_WHITE
};

#ifdef __cplusplus
extern "C"
{
#endif
HANDLE OpenMailDevice();
void CloseMailDevice(HANDLE deviceHandle);
void SetMailDeviceColor(HANDLE deviceHandle, UINT color);
#ifdef __cplusplus
}
#endif
