#ifndef STR2CODEPOINT_H
#define STR2CODEPOINT_H

static
int32_t str2codepoint(const char * s) {
    const unsigned char utftab[64] = {
        0, 0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, /* 1100xxxx */
        2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, /* 1101xxxx */
        3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, /* 1110xxxx */
        4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 6, 6, 0, 0, /* 1111xxxx */
    };
    const int UTFmax = 6; /* maximum bytes per rune */
    const int32_t RUNEERROR = (int32_t)-1;
    unsigned char c, i, m, n, x;
    int32_t r;

    c = *s++;

    /* basic byte */
    if (!(c & 0200)) { return c; }

    /* continuation byte */
    if (!(c & 0100)) { return RUNEERROR; }

    n = utftab[c & 077];

    /* illegal byte */
    if (n == 0) { return RUNEERROR; }

    /* not a continuation byte */
    if ((*s & 0300) != 0200) { return RUNEERROR; }

    x = 0377 >> n;
    r = c & x;

    r = (r << 6) | (*s++ & 077);

    /* overlong sequence */
    if (r <= x) { return RUNEERROR; }

    m = (UTFmax < n) ? UTFmax : n;

    for(i = 2; i < m; i++) {
        /* not a continuation byte */
        if ((*s & 0300) != 0200) { return RUNEERROR; }

        r = (r << 6) | (*s++ & 077);
    }

    /* must have reached len limit */
    if (i < n) { return 0; }

    /* is valid rune? */
    if (r < 0                       /* negative value */
    || (r >= 0xD800 && r <= 0xDFFF) /* surrogate pair range */
    || (r >= 0xFDD0 && r <= 0xFDEF) /* non-character range */
    || ((r & 0xFFFE) == 0xFFFE)     /* non-character at end of plane */
    || (r > 0x10FFFF)               /* too large, thanks to UTF-16 */
    ) {
        return RUNEERROR;
    }

    return r;
}

#endif
