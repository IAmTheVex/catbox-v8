`catbox-v8` supports loading native compiled modules. In this directory is an example module to
get you started. Your code must be isolate-aware, context-aware, and thread-safe. Using `nan`
[Native Abstractions for Node] as demonstrated here will get you most of the way there since all
those functions are isolate and context-aware. Sometimes existing native modules can be modified
to support `catbox-v8` with very few changes at all.

You should *not* use libuv in `catbox-v8`, except in the default isolate. Also, asynchronous
callbacks are not supported except for the default isolate.
