#ifndef TTF_QUADRUPLET_H
#define TTF_QUADRUPLET_H

enum {
    TTF_NORMAL     = 0b00,
    TTF_BOLD       = 0b01,
    TTF_ITALIC     = 0b10,
    TTF_BOLDITALIC = 0b11,
};

typedef struct {
    char * normal;
    char * bold;
    char * italic;
    char * bolditalic;
} ttf_quadruplet_t;

static inline
bool is_quadruplet_full(ttf_quadruplet_t q) {
    return q.normal
        && q.bold
        && q.italic
        && q.bolditalic
    ;
}

extern ttf_quadruplet_t load_font(const char * target_name);

#endif
