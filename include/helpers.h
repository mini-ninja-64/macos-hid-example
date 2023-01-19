#pragma once

#include <CoreFoundation/CoreFoundation.h>
#include <IOKit/hid/IOHidManager.h>

char * create_c_string(CFStringRef cf_string_ref);
void print_cf_dictionary(CFDictionaryRef cf_dictionary_ref);


CFDictionaryRef create_hid_filter_dictionary(
    int vendorId, bool checkVendorId, 
    int productId, bool checkProductId,  
    int usagePage, bool checkUsagePage, 
    int usageId, bool checkUsageId);
void print_iohid_device(IOHIDDeviceRef io_hid_device_ref);
