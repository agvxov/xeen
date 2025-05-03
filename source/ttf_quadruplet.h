#ifndef TTF_QUADRUPLET_H
#define TTF_QUADRUPLET_H

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

extern ttf_quadruplet_t load_font(char * fonts_path, const char * target_name);

#endif
