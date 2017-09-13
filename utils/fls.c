#ifndef __FLS_HEADER__
#define __FLS_HEADER__

static inline int fls(int value) {
    int r = 32;

    if (!value) return 0;
    if (!(value & 0xffff0000u)) {
        value <<= 16;
        r -= 16;
    }
    if (!(value & 0xff000000u)) {
        value <<= 8;
        r -= 8;
    }
    if (!(value & 0xf0000000u)) {
        value <<= 4;
        r -= 4;
    }
    if (!(value & 0xc0000000u)) {
        value <<= 2;
        r -= 2;
    }
    if (!(value & 0x80000000u)) {
        value <<= 1;
        r -= 1;
    }
    return r;
}

static inline int flsll(long long value) {
    int r = 64;

    if (!value) return 0;
    if (!(value & 0xffffffff00000000UL)) {
        value <<= 32;
        r -= 32;
    }
    if (!(value & 0xffff000000000000UL)) {
        value <<= 16;
        r -= 16;
    }
    if (!(value & 0xff00000000000000UL)) {
        value <<= 8;
        r -= 8;
    }
    if (!(value & 0xf000000000000000UL)) {
        value <<= 4;
        r -= 4;
    }
    if (!(value & 0xc000000000000000UL)) {
        value <<= 2;
        r -= 2;
    }
    if (!(value & 0x8000000000000000UL)) {
        value <<= 1;
        r -= 1;
    }
    return r;
}

#if defined(__LP64__) || defined(_WIN64) || (defined(__x86_64__) && !defined(__ILP32__)) || \
    defined(_M_X64) || defined(__ia64) || defined(_M_IA64) || defined(__aarch64__) ||       \
    defined(__powerpc64__)
static inline int flsl(long value) { return flsll(value); }
#else /* !__LP64__ */
static inline int flsl(long value) { return fls(value); }
#endif

#endif /* ifndef __FLS_HEADER__ */
