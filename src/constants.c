#include <CoreFoundation/CoreFoundation.h>
#include <IOKit/hid/IOHIDKeys.h>

#include "constants.h"

// TODO: CFSTR does not guarantee compile time constants!!
//       On MacOS it uses a compiler built in which does :)
//       clang-tidy will complain as it is not aware of the builtin.
const CFStringRef CF_VENDOR_ID_KEY = CFSTR(kIOHIDVendorIDKey);
const CFStringRef CF_PRODUCT_ID_KEY = CFSTR(kIOHIDProductIDKey);
const CFStringRef CF_PRIMARY_USAGE_PAGE_KEY = CFSTR(kIOHIDPrimaryUsagePageKey);
const CFStringRef CF_PRIMARY_USAGE_ID_KEY = CFSTR(kIOHIDPrimaryUsageKey);

const CFStringRef CF_LOG_KEYS[] = {
    CFSTR(kIOHIDManufacturerKey),
    CFSTR(kIOHIDProductKey),
    CFSTR(kIOHIDTransportKey),
    CF_VENDOR_ID_KEY,
    CF_PRODUCT_ID_KEY,
    CF_PRIMARY_USAGE_PAGE_KEY,
    CF_PRIMARY_USAGE_ID_KEY,
};

const size_t CF_LOG_KEYS_LENGTH = sizeof(CF_LOG_KEYS) / sizeof(CFStringRef);
