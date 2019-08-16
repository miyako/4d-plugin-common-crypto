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

#ifdef __APPLE__
#include <CoreFoundation/CoreFoundation.h>
#else
#include <Windows.h>
#endif

typedef std::basic_string<PA_Unichar> CUTF16String;
typedef std::basic_string<uint8_t> CUTF8String;

void json_wconv(const wchar_t *value, CUTF16String *u16);

void ob_set_p(PA_ObjectRef obj, const wchar_t *_key, PA_Picture value);

void ob_set_s(PA_ObjectRef obj, const wchar_t *_key, const char *_value);
void ob_set_s(PA_ObjectRef obj, const char *_key, const char *_value);

void ob_set_a(PA_ObjectRef obj, const wchar_t *_key, CUTF16String *value);
void ob_set_a(PA_ObjectRef obj, const wchar_t *_key, const wchar_t *_value);

void ob_set_c(PA_ObjectRef obj, const wchar_t *_key, PA_CollectionRef value);
void ob_set_o(PA_ObjectRef obj, const wchar_t *_key, PA_ObjectRef value);
void ob_set_i(PA_ObjectRef obj, const wchar_t *_key, PA_long32 value);
void ob_set_n(PA_ObjectRef obj, const wchar_t *_key, double value);
void ob_set_b(PA_ObjectRef obj, const wchar_t *_key, bool value);

bool ob_is_defined(PA_ObjectRef obj, const wchar_t *_key);

bool ob_get_s(PA_ObjectRef obj, const wchar_t *_key, CUTF8String *value);

bool ob_get_a(PA_ObjectRef obj, const wchar_t *_key, CUTF16String *value);

bool ob_get_b(PA_ObjectRef obj, const wchar_t *_key);
double ob_get_n(PA_ObjectRef obj, const wchar_t *_key);

PA_CollectionRef ob_get_c(PA_ObjectRef obj, const wchar_t *_key);
PA_ObjectRef ob_get_o(PA_ObjectRef obj, const wchar_t *_key);

void ob_stringify(PA_ObjectRef obj, CUTF8String *value);

#endif /* PLUGIN_JSON_H */
