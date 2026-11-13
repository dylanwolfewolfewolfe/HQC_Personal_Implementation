#include "os_random.h"
#ifdef _WIN32
#include <windows.h>
#include <bcrypt.h>
int os_random(uint8_t *out, size_t len) {
    if (len > 0xffffffffu) return -1;
    return BCryptGenRandom(NULL, out, (ULONG)len, BCRYPT_USE_SYSTEM_PREFERRED_RNG) == 0 ? 0 : -1;
}
#else
#include <stdio.h>
int os_random(uint8_t *out, size_t len) {
    FILE *f = fopen("/dev/urandom", "rb");
    if (!f) return -1;
    size_t got = fread(out, 1, len, f);
    fclose(f);
    return got == len ? 0 : -1;
}
#endif
