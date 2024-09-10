/* assert.h: assert and static_assert macros for C and C++
 *
 * Documentation:
 *   STATIC_ASSERT(condition, message?): (may be a declaration)
 *     for static assertions at file, struct or function scope.
 *   STATIC_ASSERT_EXPR(condition, message?): (is a void expression)
 *     use this when an expression is required (i.e: in a switch statement).
 *   ASSERT(condition):
 *     for regular assertions, can be tuned for debugging by defining
 *     ASSERT_DEBUG or DEBUG, otherwise invokes undefined behavior on failure.
 *
 * Limitations: (in C89, C99, C++98 and C++03)
 *   - static assertion messages are ignored.
 *   - if the compiler does not support __COUNTER__, static assertions will
 *     fail to compile if used on the same line.
 */
#ifndef ASSERT_H_
#define ASSERT_H_

#if defined(__cpp_static_assert) || defined(static_assert) || \
    defined(__STDC_VERSION__) && __STDC_VERSION__ >= 202311L
# define STATIC_ASSERT__IMPL(cond, msg, ...) static_assert(cond, msg)
# define STATIC_ASSERT_EXPR__IMPL            STATIC_ASSERT__IMPL
#elif defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L
# define STATIC_ASSERT__IMPL(cond, msg, ...) _Static_assert(cond, msg)
# define STATIC_ASSERT_EXPR__IMPL            STATIC_ASSERT__IMPL
#else
# ifdef __COUNTER__
#  define ASSERT__UNIQUE __COUNTER__
# else /* don't use twice on the same line if __COUNTER__ is not available */
#  define ASSERT__UNIQUE __LINE__
# endif
#
# define ASSERT__CONCAT_(X, Y) X ## Y
# define ASSERT__CONCAT(X, Y) ASSERT__CONCAT_(X, Y)
#  /* A named bit-field of width of zero is an error */
# define STATIC_ASSERT__IMPL(condition, ...)                   \
     ;struct ASSERT__CONCAT(static_assertion_, ASSERT__UNIQUE) \
     { unsigned static_assertion_failed: (int)!!(condition); }
# define STATIC_ASSERT_EXPR__IMPL(condition, ...) \
     ((void)sizeof(int[1 - 2*(int)!(condition)]))
#endif

#ifndef STATIC_ASSERT
# define STATIC_ASSERT(...) STATIC_ASSERT__IMPL(__VA_ARGS__, "", _)
#endif

#ifndef STATIC_ASSERT_EXPR
# define STATIC_ASSERT_EXPR(...) STATIC_ASSERT_EXPR__IMPL(__VA_ARGS__, "", _)
#endif

#ifndef ASSERT
# ifndef ASSERT_FAILED
#  if defined(ASSERT_DEBUG) || defined(DEBUG)
#   if defined(__GNUC__)
#    define ASSERT_FAILED __builtin_trap()
#   elif defined(_MSC_VER)
#    define ASSERT_FAILED __debugbreak()
#   else
#    define ASSERT_FAILED ((void)(*(volatile int *)0 = 0))
#   endif
#  else
#   if defined(unreachable)
#    define ASSERT_FAILED unreachable()
#   elif defined(__GNUC__)
#    define ASSERT_FAILED __builtin_unreachable()
#   elif defined(_MSC_VER)
#    define ASSERT_FAILED __assume(0)
#   else /* undefined behavior, should be optimized out */
#    define ASSERT_FAILED ((void)(*(int*)0 = 0))
#   endif
#  endif
# endif
#
# define ASSERT(condition) ((condition) ? (void)0 : ASSERT_FAILED)
#endif

#endif
