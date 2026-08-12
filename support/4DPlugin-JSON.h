#ifndef PLUGIN_JSON_H
#define PLUGIN_JSON_H

#include "4DPluginAPI.h"
#include <string>
#include <vector>
#include <map>
#include <iostream>     // std::cout
#include <iterator>     // std::back_inserter
#include <vector>       // std::vector
#include <algorithm>    // std::copy

#ifdef _WIN32
//some external libraries assume first load; include this file after them 
//need to load winsock2 before windows
//BSD wrappers
#define close closesocket
#define TickCount GetTickCount
//#define getpid GetCurrentProcessId
#include <winsock2.h>

#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

#include <windows.h>
#include <iphlpapi.h>
#include <icmpapi.h>

#pragma comment(lib, "iphlpapi.lib")
#include <time.h>
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")

// Undefine immediately: `close`/`TickCount` above are BSD-compatibility
// renames for whatever networking/ICMP code elsewhere expects those names.
// Leaving them defined for the rest of this header's lifetime pollutes
// every other translation unit that includes 4DPlugin-JSON.h -- e.g. it
// would silently rewrite std::fstream::close() into std::fstream::closesocket(),
// which either fails to compile far from the real cause or compiles into
// the wrong call if a matching member ever exists. Nothing in this header
// or its .cpp needs these names past this point. If other files in the
// project rely on this renaming, they should #define it locally right
// before the code that needs it instead of depending on this header.
#undef close
#undef TickCount
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <netdb.h>
#include <arpa/inet.h>
#define SOCKET int
#define SOCKET_ERROR (-1)
#define INVALID_SOCKET (SOCKET)(~0)
#endif

#ifdef __APPLE__
#include <CoreFoundation/CoreFoundation.h>
#endif

typedef std::basic_string<PA_Unichar> CUTF16String;
typedef std::basic_string<uint8_t> CUTF8String;

void json_wconv(const wchar_t *value, CUTF16String *u16);

void ob_set_p(PA_ObjectRef obj, const wchar_t *_key, PA_Picture value);

void ob_set_s(PA_ObjectRef obj, const wchar_t *_key, const char *_value);
void ob_set_s(PA_ObjectRef obj, const char *_key, const char *_value);

void ob_set_a(PA_ObjectRef obj, const wchar_t *_key, const wchar_t *_value);
// Convenience overload: every call site across the plugins that use this
// header passes a CUTF16String (by pointer) rather than a raw wchar_t*
// buffer -- e.g. `CUTF16String u16; ...; ob_set_a(obj, L"key", &u16);`.
// Without this overload that fails to compile (C2664: cannot convert
// CUTF16String* to const wchar_t*) the moment the caller's platform branch
// actually gets built.
void ob_set_a(PA_ObjectRef obj, const wchar_t *_key, const CUTF16String *_value);
void ob_set_c(PA_ObjectRef obj, const wchar_t *_key, PA_CollectionRef value);
void ob_set_o(PA_ObjectRef obj, const wchar_t *_key, PA_ObjectRef value);
// Convenience overload: narrow (char*) key, e.g. a column name returned by
// a C library such as libpq's PQfname(). Without this, callers with a
// char* key (as opposed to a wchar_t* literal) fail to compile (C2664:
// cannot convert char* to const wchar_t*) -- same shape as the existing
// ob_set_s(char*, char*) overload below, just for object-valued properties.
void ob_set_o(PA_ObjectRef obj, const char *_key, PA_ObjectRef value);
void ob_set_i(PA_ObjectRef obj, const wchar_t *_key, PA_long32 value);
void ob_set_n(PA_ObjectRef obj, const wchar_t *_key, double value);
// Narrow-key sibling: matches ob_set_s/ob_set_o, which already have both a
// wide- and narrow-key overload. Without this, a caller passing a char* key
// (e.g. a column name from a C library) fails to compile (C2664: cannot
// convert const char* to const wchar_t*) -- confirmed live in this project's
// own CI: 4DPlugin-Simple-SQLite-Client.cpp(206,37) and (214,29).
void ob_set_n(PA_ObjectRef obj, const char *_key, double value);
void ob_set_b(PA_ObjectRef obj, const wchar_t *_key, bool value);
// ob_set_0: sets a property to a JSON/4D null value. Did not exist at all in
// this header -- confirmed live in this project's own CI:
// 4DPlugin-Simple-SQLite-Client.cpp(246,29): error C3861: 'ob_set_0':
// identifier not found. Both overloads added for the same wide/narrow-key
// symmetry as every other setter above.
void ob_set_0(PA_ObjectRef obj, const wchar_t *_key);
void ob_set_0(PA_ObjectRef obj, const char *_key);
bool ob_is_defined(PA_ObjectRef obj, const wchar_t *_key);
bool ob_get_a(PA_ObjectRef obj, const wchar_t *_key, CUTF8String *value);
bool ob_get_b(PA_ObjectRef obj, const wchar_t *_key);
double ob_get_n(PA_ObjectRef obj, const wchar_t *_key);
PA_CollectionRef ob_get_c(PA_ObjectRef obj, const wchar_t *_key);

void ob_stringify(PA_ObjectRef obj, CUTF8String *value);

#endif /* PLUGIN_JSON_H */
