#include <iostream>
#include <libusb-1.0/libusb.h>

int result = 0;
char exit_code = 0;

int main() {
    // Initialize libusb
    result = libusb_init(NULL);
    if (result != 0) {
        std::cout << "Error initializing LIBUSB: " << libusb_error_name(result) << std::endl;
        exit_code = 1; // ERROR: Failed to initialize libusb
    }

    // Deinitialize libusb
    libusb_exit(NULL);

    // Exit successfully
    return exit_code;
}