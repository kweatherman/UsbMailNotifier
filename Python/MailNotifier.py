
"""
USB "Mail Notifier" HID device support
"""
import os
import sys
import usb      # pip install libusb pyusb
import site
import time
from enum import IntEnum

# LED colors
class LEDColor(IntEnum):
    NONE = 0      # None/black/off
    BLUE = 1
    RED = 2
    GREEN = 3
    CYAN = 4
    MAGENTA = 5
    YELLOW = 6
    WHITE = 7

MAIL_VENDOR_ID = 0x1294
MAIL_PRODUCT_ID = 0x1320

if os.name == 'nt':
    """
    Windows OS
    Add 'libusb-1.0.dll' to the Windows path programmatically.
    Alternately, drop the DLL into an existing path folder manually
    """
    dll_path = '\\libusb\\_platform\\_windows\\x86'
    if sys.maxsize > 2 ** 32:
        dll_path = '\\libusb\\_platform\\_windows\\x64'
    for path in site.getsitepackages():
        os.environ['PATH'] += (';' + path + dll_path)



def set_mail_device_color(color):
    """
    Set "Mail Notifier" color
    :param color:
    :return: True on success
    """
    # Find first USB "Mail Notifier" device
    device = usb.core.find(idVendor=MAIL_VENDOR_ID, idProduct=MAIL_PRODUCT_ID)
    if device:
        # Write HID packet
        device.write(2, (int(color) & 7, 0, 0, 0, 0), 1000)
        return True
    return False


# "Mail Notifier" HID LED test
if __name__ == "__main__":
    # Check if device is present and set the color black
    if set_mail_device_color(0):
        print("Device located.")
    else:
        print("** A USB Mail LED HID device not found! ***")
        sys.exit(1)

    # Cycle through the available colors twice
    print("\nCycling colors..")
    for i in range(2):
        for color in range(LEDColor.WHITE + 1):
            set_mail_device_color(color)
            time.sleep(0.5)

    print("\nFlashing colors..")
    for color in range(LEDColor.BLUE, LEDColor.WHITE + 1):
        for flash in range(4):
            set_mail_device_color(color)
            time.sleep(0.140)
            set_mail_device_color(0)
            time.sleep(0.140)
    set_mail_device_color(0)

    print("Done.")