// These macros weren't added until v8 version 4.4
#include "node_wrapper.h"
#ifndef V8_MAJOR_VERSION
	#if NODE_MODULE_VERSION <= 11
		#define V8_MAJOR_VERSION 3
		#define V8_MINOR_VERSION 14
		#define V8_PATCH_LEVEL 0
	#elif V8_MAJOR_VERSION <= 14
		#define V8_MAJOR_VERSION 3
		#define V8_MINOR_VERSION 28
		#define V8_PATCH_LEVEL 0
	#else
		#error v8 version macros missing
	#endif
#endif

#define V8_AT_LEAST(major, minor, patch) (\
	V8_MAJOR_VERSION > (major) || \
	(V8_MAJOR_VERSION == (major) && V8_MINOR_VERSION >= (minor)) || \
	(V8_MAJOR_VERSION == (major) && V8_MINOR_VERSION == (minor) && V8_PATCH_LEVEL >= (patch)) \
)

#ifdef NODE_MODULE_VERSION
#define NODE_MODULE_OR_V8_AT_LEAST(nodejs, v8_major, v8_minor, v8_patch) (NODE_MODULE_VERSION >= nodejs)
#else
#define NODE_MODULE_OR_V8_AT_LEAST(nodejs, v8_major, v8_minor, v8_patch) (V8_AT_LEAST(v8_major, v8_minor, v8_patch))
#endif