#include "helpers.h"
#include "constants.h"

static const char* NULL_STRING = "(NULL)";

char * create_c_string(CFStringRef cf_string) {
    if(cf_string == NULL) return NULL;

    CFIndex string_length = CFStringGetLength(cf_string);
    CFIndex max_size = CFStringGetMaximumSizeForEncoding(string_length, kCFStringEncodingASCII) + 1;
    
    char *string_buffer = calloc(sizeof(char), max_size);
    if(CFStringGetCString(cf_string, string_buffer, max_size, kCFStringEncodingASCII)){
        return string_buffer;
    }
    free(string_buffer);
    return NULL;
}

#define HANDLE_NULL_OBJECT(cf_object) if (cf_object == NULL) { printf("%s\n", NULL_STRING); return; }

void print_cf_dictionary(CFDictionaryRef cf_dictionary) {
    HANDLE_NULL_OBJECT(cf_dictionary);

    CFIndex dict_size = CFDictionaryGetCount(cf_dictionary);
    const void **key_pointer_array = calloc(sizeof(void*), dict_size); // array of void pointers
    const void **value_pointer_array = calloc(sizeof(void*), dict_size); // array of void pointers
    // TODO: check neither is null

    CFDictionaryGetKeysAndValues(cf_dictionary, key_pointer_array, value_pointer_array);

    printf("Dictionary (%li) {\n", dict_size);
    for(CFIndex i = 0; i < dict_size; i++) {
        // TODO: Check if CoreFoundation object or not
        CFTypeRef key = (CFTypeRef) key_pointer_array[i];
        CFStringRef cf_key_description = CFCopyDescription(key);
        char *key_description = create_c_string(cf_key_description);
        CFRelease(cf_key_description);

        CFTypeRef value = (CFTypeRef) value_pointer_array[i];
        CFStringRef cf_value_description = CFCopyDescription(value);
        char *value_description = create_c_string(cf_value_description);
        CFRelease(cf_value_description);

        printf("\t%s:\n\t\t%s,\n\n", key_description, value_description);
        free(key_description);
        free(value_description);
    }
    printf("}\n");

    free(key_pointer_array);
    free(value_pointer_array);
}

void print_iohid_device(IOHIDDeviceRef iohid_device) {
    HANDLE_NULL_OBJECT(iohid_device);

    int number_of_keys = sizeof(CF_LOG_KEYS)/sizeof(CFStringRef);

    for(int i=0; i < number_of_keys; i++) {
        CFStringRef property_key = CF_LOG_KEYS[i];
        char *property_key_string = create_c_string(property_key);
        printf("\t%s: ", property_key_string);
        free(property_key_string);
        CFTypeRef property_value = IOHIDDeviceGetProperty(iohid_device, property_key);
        if(property_value == NULL) {
            printf("%s", NULL_STRING);
        } else {
            CFTypeID type_id = CFGetTypeID(property_value);
            if(type_id == CFNumberGetTypeID()) {
                int number_value;
                bool success = CFNumberGetValue((CFNumberRef) property_value, kCFNumberIntType, &number_value);
                if(success) printf("%i", number_value);
            } else if (type_id == CFStringGetTypeID()) {
                char *cString = create_c_string(property_value);
                printf("%s", cString);
                free(cString);
            } else {
                CFStringRef cf_value_description = CFCopyDescription(property_value);
                char *value_description = create_c_string(cf_value_description);
                printf("%s", value_description);
                free(value_description);
                CFRelease(cf_value_description);
            }
        };
        printf("\n");
    }
}

#define CONDITIONALLY_ADD_KEY_VALUE(condition, counter, key, value) { \
    if (condition) { \
        keys[counter] = key; \
        values[counter] = value; \
        counter++; \
    } \
}

CFDictionaryRef create_hid_filter_dictionary(
    int vendor_id, bool check_vendor_id, 
    int product_id, bool check_product_id,  
    int usage_page, bool check_usage_page, 
    int usage_id, bool check_usage_id) {
        const int property_count = check_vendor_id + check_product_id + check_usage_page + check_usage_id;
        CFStringRef *keys = calloc(sizeof(CFStringRef), property_count);
        CFNumberRef *values = calloc(sizeof(CFNumberRef), property_count);

        int property_position = 0;
        CONDITIONALLY_ADD_KEY_VALUE(
            check_vendor_id, property_position,
            CF_VENDOR_ID_KEY, CFNumberCreate(NULL, kCFNumberIntType, &vendor_id)    
        )
        CONDITIONALLY_ADD_KEY_VALUE(
            check_product_id, property_position,
            CF_PRODUCT_ID_KEY, CFNumberCreate(NULL, kCFNumberIntType, &product_id)
        )
        CONDITIONALLY_ADD_KEY_VALUE(
            check_usage_page, property_position,
            CF_PRIMARY_USAGE_PAGE_KEY, CFNumberCreate(NULL, kCFNumberIntType, &usage_page)
        )
        CONDITIONALLY_ADD_KEY_VALUE(
            check_usage_id, property_position,
            CF_PRIMARY_USAGE_ID_KEY, CFNumberCreate(NULL, kCFNumberIntType, &usage_id)
        )

        CFDictionaryRef dictionary = CFDictionaryCreate(
            kCFAllocatorDefault,
            (void *) keys,
            (void *) values,
            property_count,
            NULL,
            NULL
        );
        
        for(int i = 0; i < property_count; i++) {
            CFRelease(values[i]);
        }

        free(keys);
        free(values);

        return dictionary;
}
