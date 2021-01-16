// #ifndef checks whether the given token has been #defined earlier in the file or in an included file; if not, 
// it includes the code between it and the closing #else or, if no #else is present, #endif statement. #ifndef 
// is often used to make header files idempotent by defining a token once the file has been included and checking 
// that the token was not set at the top of that file.

#ifndef __dbg_h__
// the #define directive allows the definition of macros within your source code. These macro definitions allow constant values to be declared for use throughout your code.
#define __dbg_h__

#include <stdio.h>
#include <errno.h>
#include <string.h>

// The start of a #ifdef that lets you recompile your program so that all of the debug log messages are removed.
#ifdef NDEBUG
// If you compile with NDEBUG defined, then “no debug” messages will remain.
// you need to add that as a flag to ensure that your compile is clean from debug messages
#define debug(M, ...)

#else

// The alternative #define debug that translates any use of debug("format", arg1, arg2) into an fprintf call to stderr.
// Many C programmers don’t know this, but you can create macros that actually work like printf and take variable arguments.
//  Some C compilers (actually CPP) don’t support this, but the ones that matter do. The magic here is the use of ##__VA_ARGS__
//  that says “put whatever they had for extra arguments (...) here.” Also notice the use of __FILE__ and __LINE__ to get 
// the current file:line for the debug message.
#define debug(M, ...) fprintf(stderr, "DEBUG %s:%d: " M "\n",\
        __FILE__, __LINE__, ##__VA_ARGS__)
#endif

#define assert_eq(a,b) if (a != b) {std::cout << "ASSERT: " << __FILE__ << ":" << __LINE__ << ": " << a << " not eq " << b << "\n"; errno=0; }

#define assert_neq(a,b) if (a == b) {std::cout << "ASSERT: " << __FILE__ << ":" << __LINE__ << ": " << a << " eq " << b << "\n"; errno=0;}


// The clean_errno macro that’s used in the others to get a safe, readable version of errno.
#define clean_errno() (errno == 0 ? "None" : strerror(errno))

// these log messages are for the end user
// log_err, log_warn, and log_info can't be compiled out
#define log_err(M, ...) fprintf(stderr,\
        "[ERROR] (%s:%d: errno: %s) " M "\n", __FILE__, __LINE__,\
        clean_errno(), ##__VA_ARGS__)

#define log_warn(M, ...) fprintf(stderr,\
        "[WARN] (%s:%d: errno: %s) " M "\n",\
        __FILE__, __LINE__, clean_errno(), ##__VA_ARGS__)

#define log_info(M, ...) fprintf(stderr, "[INFO] (%s:%d) " M "\n",\
        __FILE__, __LINE__, ##__VA_ARGS__)

// checks if A is true, and if not,
// it goes to the functions error for cleanup
#define check(A, M, ...) if(!(A)) {\
    log_err(M, ##__VA_ARGS__); errno=0; goto error; } else { log_info("Test Passed."); }

// kidn of the opposite of check
// is placed in any part of a function that shouldn’t run, and if it does, 
// it prints an error message and then jumps to the error: label. 
// You put this in if-statements and switch-statements to catch conditions that shouldn’t happen, like the default:.
#define sentinel(M, ...)  { log_err(M, ##__VA_ARGS__);\
    errno=0; goto error; }

// makes sure that the poitner is valid
#define check_mem(A) check((A), "Out of memory.")

// An alternative macro, check_debug, which still checks and handles an error, 
// but if the error is common, then it doesn’t bother reporting it. 
// In this one, it will use debug instead of log_err to report the message. 
// So when you define NDEBUG, the check still happens, and the error jump goes off, but the message isn’t printed.
#define check_debug(A, M, ...) if(!(A)) { debug(M, ##__VA_ARGS__);\
    errno=0; goto error; }



#endif