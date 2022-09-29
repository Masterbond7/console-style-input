#include <iostream>
#include <libusb-1.0/libusb.h>

// Core program variables
int result = 0;
libusb_context *usb_context;

int main() {
    // Initialize libusb
    result = libusb_init(&usb_context);
    if (result != 0) {
        std::cout << "Error initializing libusb: " << libusb_error_name(result) << std::endl;
        return 1; // ERROR: Failed to initialize libusb
    }

    result = libusb_set_option(usb_context, LIBUSB_OPTION_LOG_LEVEL, LIBUSB_LOG_LEVEL_WARNING);
    if (result != 0) {
        std::cout << "Error setting libusb log level: " << libusb_error_name(result) << std::endl;
        return 1; // ERROR: Failed to initialize libusb
    }

    // Deinitialize libusb
    libusb_exit(usb_context);

    // Exit successfully
    return 0;
}