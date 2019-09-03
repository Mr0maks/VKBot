#pragma once
#include <stdlib.h>

//#define FIND_LEAK

#ifndef GC_THREADS
#define GC_THREADS
#endif

#define GC_REDIRECT_TO_LOCAL

#define THREAD_LOCAL_ALLOC

#undef GC_NO_THREAD_REDIRECTS

#define IGNORE_FREE

#define ALL_INTERIOR_POINTERS
#define CHECKSUMS
#define GC_ALWAYS_MULTITHREADED
#define GC_ATOMIC_UNCOLLECTABLE
#define GC_ENABLE_SUSPEND_THREAD
#define HANDLE_FORK
#define USE_MMAP
#define USE_MUNMAP

#define PARALLEL_MARK

#include <gc/gc.h>
#include <gc/leak_detector.h>

#ifdef GC_PTHREADS
# include <pthread.h>
#endif
