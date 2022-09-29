#include <iostream>
#include <libusb-1.0/libusb.h>

int result = 0;

int main() {
    // Initialize libusb
    result = libusb_init(NULL);
    if (result != 0) {
        std::cout << "Error initializing libusb: " << libusb_error_name(result) << std::endl;
        return 1; // ERROR: Failed to initialize libusb
    }

    result = libusb_set_option(NULL, LIBUSB_OPTION_LOG_LEVEL, LIBUSB_LOG_LEVEL_WARNING);
    if (result != 0) {
        std::cout << "Error setting libusb log level: " << libusb_error_name(result) << std::endl;
        return 1;
    }

    // Deinitialize libusb
    libusb_exit(NULL);

    // Exit successfully
    return 0;
}