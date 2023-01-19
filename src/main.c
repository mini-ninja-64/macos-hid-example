#include <IOKit/hid/IOHidManager.h>
#include "helpers.h"


void device_add(void *context, IOReturn result, void *sender, IOHIDDeviceRef device) {
    printf("device add!\n");
    print_iohid_device(device);
}

void device_removed(void *context, IOReturn result, void *sender, IOHIDDeviceRef device) {
    printf("device removed!\n");
    print_iohid_device(device);
}

int main(int argc, char *argv[]) {
    CFDictionaryRef logitech_mouse_match = create_hid_filter_dictionary(
        0x046d, true, // Logitech: 0x413c or 0x046d
        0x301a, true, // G502 Mouse
        0x00, false,
        0x00, false
    );

    // NOTE: it could match but the conditions are rare, so I have named it no_match
    CFDictionaryRef no_match = create_hid_filter_dictionary(
        0x00, true,
        0x00, true,
        0x00, true,
        0x00, true
    );

    CFDictionaryRef all_match = create_hid_filter_dictionary(
        0x00, false,
        0x00, false,
        0x00, false,
        0x00, false
    );
    
    printf("Logitech mouse dictionary:\n");
    print_cf_dictionary(logitech_mouse_match);
    printf("No match dictionary:\n");
    print_cf_dictionary(no_match);
    printf("All match dictionary:\n");
    print_cf_dictionary(all_match);

    IOHIDManagerRef manager = IOHIDManagerCreate(kCFAllocatorDefault, kIOHIDOptionsTypeNone);

    // NOTE: the device matcher setters set the matching condition and 
    //       do not get merged or stack etc... but instead replace.
    //       so the following call will be replaced by the call to
    //       IOHIDManagerSetDeviceMatchingMultiple.
    IOHIDManagerSetDeviceMatching(manager, no_match);

    const void* matchers_c_array[2] = {(void*)logitech_mouse_match, (void*)no_match};
    // NOTE: a call to CFArrayCreate copies the contents of the array so once the CFArray 
    //       is created the C array can be safely free'd if dynamiccaly created
    CFArrayRef matchers_array = CFArrayCreate(kCFAllocatorDefault, matchers_c_array, 2, NULL);
    IOHIDManagerSetDeviceMatchingMultiple(manager, matchers_array);
    // NOTE: Although not explicit in the documentation, 
    //       IOHIDManagerSetDeviceMatchingMultiple copies each dict reference
    //       making the cf matching array useless, because of this it is safe to release
    //       but could it be considered UB? 🤔
    //       - IOHIDManagerSetDeviceMatchingMultiple implementation: https://github.com/opensource-apple/IOKitUser/blob/b80a5cbc0ebfb5c4954ef6d757918db0e4dc4b7f/hid.subproj/IOHIDManager.c#L764
    if(matchers_array != NULL) CFRelease(matchers_array);

    // NOTE: context can be useful but unnessecary for this example so has been set as NULL
    IOHIDManagerRegisterDeviceMatchingCallback(manager, device_add, NULL);
    IOHIDManagerRegisterDeviceRemovalCallback(manager, device_removed, NULL);

    CFRunLoopRef cfRunLoop = CFRunLoopGetCurrent();
    IOHIDManagerScheduleWithRunLoop(manager, cfRunLoop, kCFRunLoopDefaultMode);
    IOReturn result = IOHIDManagerOpen(manager, kIOHIDOptionsTypeNone);

    if(result == kIOReturnSuccess){
        printf ("IOHIDManager has been opened\n");

        // NOTE: the following commented out code is illegal, it will result in segfault as matcher dictionaries are 
        //       required to exist while the matching dictionaries are in use by the IOHIDManager.
        // if(d != NULL) CFRelease(d);
        // if(d2 != NULL) CFRelease(d2);

        while (CFRunLoopRunInMode(kCFRunLoopDefaultMode, 5, false) == kCFRunLoopRunHandledSource);

        result = IOHIDManagerClose(manager, kIOHIDOptionsTypeNone);
        if(result == kIOReturnSuccess){
            printf ("IOHIDManager has been closed\n");
        } else {
            printf ("IOHIDManager could not be closed\n");
        }
    } else {
        printf ("IOHIDManager could not be opened\n");
    }

    if(manager != NULL) CFRelease(manager);
    if(logitech_mouse_match != NULL) CFRelease(logitech_mouse_match);
    if(no_match != NULL) CFRelease(no_match);
}
