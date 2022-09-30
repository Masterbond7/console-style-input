#include <iostream>
#include <signal.h>
#include <libusb-1.0/libusb.h>
#include <unistd.h>

#define HID_GET_REPORT 0x01
#define HID_REPORT_TYPE_INPUT 0x01

// Global variables
libusb_context *usb_context;
libusb_device_handle *device_handle;

void catch_interrupt(int sig) {
    // Close PS3 controller
    libusb_close(device_handle);

    // Deinitialize libusb
    libusb_exit(usb_context);

    exit(0);
}

int main() {
    // Set Ctrl-C interrupt
    signal(SIGINT, catch_interrupt);

    // Initialize libusb
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
    for (int i = 0; i < 1000; i++) {
        libusb_control_transfer(device_handle, LIBUSB_ENDPOINT_IN|LIBUSB_REQUEST_TYPE_CLASS|LIBUSB_RECIPIENT_INTERFACE,
        HID_GET_REPORT, (HID_REPORT_TYPE_INPUT<<8)|0x01, 0, hid_report, sizeof(hid_report), 100);
        for (int i = 0; i < sizeof(hid_report); i++) {
            std::cout << int(hid_report[i]) << ",";
        }
        std::cout << std::endl << std::endl;
        usleep(100000);
    }

    // Close PS3 controller
    libusb_close(device_handle);

    // Deinitialize libusb
    libusb_exit(usb_context);

    // Exit successfully
    return 0;
}