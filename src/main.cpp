#include <iostream>
#include <libusb-1.0/libusb.h>
#include <unistd.h>

#define HID_GET_REPORT 0x01
#define HID_REPORT_TYPE_INPUT 0x01

// Function to display the important buts from the PS3 HID report
// in a human readable way
void print_hid_report_ps3(uint8_t *hid_report) {
    // First set of bits (select, start, stick buttons, d-pad)
    std::cout << "Select button: " << ((hid_report[2]&1)?" True":"False") << ", ";
    std::cout << "Start button: " << (((hid_report[2]>>3)&1)?" True":"False") << ", ";
    std::cout << "L-stick press: " << (((hid_report[2]>>1)&1)?" True":"False") << ", ";
    std::cout << "R-stick press: " << (((hid_report[2]>>2)&1)?" True":"False") << ", ";
    std::cout << "D-Pad up: " << (((hid_report[2]>>4)&1)?" True":"False") << ", ";
    std::cout << "D-Pad right: " << (((hid_report[2]>>5)&1)?" True":"False") << ", ";
    std::cout << "D-Pad down: " << (((hid_report[2]>>6)&1)?" True":"False") << ", ";
    std::cout << "D-Pad left: " << (((hid_report[2]>>7)&1)?" True":"False") << std::endl;

    // Second set of bits (triggers, bumpers, and right buttons)
    std::cout << "Left trigger: " << ((hid_report[3]&1)?" True":"False") << ", ";
    std::cout << "Left bumper: " << (((hid_report[3]>>2&1)?" True":"False")) << ", ";
    std::cout << "Right trigger: " << (((hid_report[3]>>1&1)?" True":"False")) << ", ";
    std::cout << "Right bumper: " << (((hid_report[3]>>3&1)?" True":"False")) << ", ";
    std::cout << "Triangle: " << (((hid_report[3]>>4&1)?" True":"False")) << ", ";
    std::cout << "Circle: " << (((hid_report[3]>>5&1)?" True":"False")) << ", ";
    std::cout << "Cross: " << (((hid_report[3]>>6&1)?" True":"False")) << ", ";
    std::cout << "Square: " << (((hid_report[3]>>7&1)?" True":"False")) << std::endl;

    // PS button & Sticks (L&R, x&y)
    std::cout << "PS button: " << ((hid_report[4]&1)?" True":"False") << ", ";
    std::cout << "Left stick: " << int(hid_report[6]) << " (x), " << int(hid_report[7]) << "(y); ";
    std::cout << "Right stick: " << int(hid_report[8]) << " (x), " << int(hid_report[9]) << "(y)" << std::endl;


    // New line
    std::cout << std::endl;
}

// Main function
int main() {
    // Initialize libusb
    libusb_context *usb_context;
    int result = libusb_init(&usb_context);
    if (result != 0) {
        std::cout << "Error initializing libusb: " << libusb_error_name(result) << std::endl;
        return 1; // ERROR: Failed to initialize libusb
    }

    result = libusb_set_option(usb_context, LIBUSB_OPTION_LOG_LEVEL, LIBUSB_LOG_LEVEL_WARNING);
    if (result != 0) {
        std::cout << "Error setting libusb log level: " << libusb_error_name(result) << std::endl;
        return 1; // ERROR: Failed to initialize libusb
    }

    // Enumerate USB devices
    libusb_device **usb_devices;
    ssize_t num_devices = libusb_get_device_list(usb_context, &usb_devices);
    if (num_devices <= 0) {
        std::cout << "Error enumerating usb devices: " << libusb_error_name(num_devices) << std::endl;
        return 2; // ERROR: Failed to enumerate usb devices
    }

    // Connect PS3 controller
    libusb_device_handle *device_handle;
    device_handle = libusb_open_device_with_vid_pid(usb_context, 0x054c, 0x0268);
    libusb_device *usb_device = libusb_get_device(device_handle); // TODO: Handle potential errors
    if (&device_handle == NULL) {
        std::cout << "Error connecting to PS3 controller; Device could not be found" << std::endl;
        return 3; // ERROR: Could not find/connect to PS3 controller
    } else { std::cout << "Successfully connected to PS3 controller: " << &device_handle << std::endl; }

    // Free unused USB devices
    libusb_free_device_list(usb_devices, 1);

    // Claim PS3 controller
    libusb_detach_kernel_driver(device_handle, 0);
    result = libusb_claim_interface(device_handle, 0); // TODO: make result one variable
    if (result != 0) {
        std::cout << "Error claiming PS3 controller: " << libusb_error_name(result) << std::endl; // ERROR: Error claiming PS3 controller
        return 4;
    }

    // Read PS3 controller HID report
    uint8_t hid_report[64];
    std::cout << "Getting HID report" << std::endl;
    for (int i = 0; i < 64*15; i++) {
        libusb_control_transfer(device_handle, LIBUSB_ENDPOINT_IN|LIBUSB_REQUEST_TYPE_CLASS|LIBUSB_RECIPIENT_INTERFACE,
        HID_GET_REPORT, (HID_REPORT_TYPE_INPUT<<8)|0x01, 0, hid_report, sizeof(hid_report), 100);
        print_hid_report_ps3(hid_report);
        std::cout << std::endl;
        usleep(15625); // 1/64th of a second
    }

    // Close PS3 controller
    libusb_close(device_handle);

    // Deinitialize libusb
    libusb_exit(usb_context);

    // Exit successfully
    return 0;
}