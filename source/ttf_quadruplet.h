#ifndef TTF_QUADRUPLET_H
#define TTF_QUADRUPLET_H

typedef struct {
    char * normal;
    char * bold;
    char * italic;
    char * bold_italic;
} ttf_quadruplet_t;

static inline
bool is_quadruplet_full(ttf_quadruplet_t q) {
    return q.normal
        && q.bold
        && q.italic
        && q.bold_italic
    ;
}

static inline
void free_ttf_quadruplet(ttf_quadruplet_t q) {
    extern void free(void * ptr);
    free(q.normal);
    free(q.bold);
    free(q.italic);
    free(q.bold_italic);
}

extern ttf_quadruplet_t load_font_paths(const char * root_font_path, const char * target_name);

#endif
