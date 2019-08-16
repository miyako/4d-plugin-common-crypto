#ifndef C_TYPES_H
#define C_TYPES_H

#if __APPLE__
#import <CoreFoundation/CoreFoundation.h>
#import <Cocoa/Cocoa.h>
#import <AppKit/AppKit.h>
#endif

#if _WIN32
#include <windows.h>
#endif

#include <string>
#include <vector>

#ifndef uint8_t
typedef unsigned char uint8_t;
#endif
#ifndef uint16_t
typedef unsigned short uint16_t;
#endif
#ifndef uint32_t
typedef unsigned int uint32_t;
#endif

typedef uint8_t * BytePtr;
typedef BytePtr *PackagePtr;

typedef std::basic_string<PA_Unichar> CUTF16String;
typedef std::basic_string<uint8_t> CUTF8String;
typedef std::vector<CUTF16String> CUTF16StringArray;
typedef std::vector<CUTF8String> CUTF8StringArray;

#endif /* C_TYPES_H */
