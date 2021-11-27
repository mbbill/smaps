#ifndef _SMAPS_H
#define _SMAPS_H

typedef enum {
    size = 0,
    kps,
    mmu,
    rss,
    pss,
    shrc,
    shrd,
    pric,
    prid,
    ref,
    anon,
    lzf,
    anonp,
    shmap,
    shhu,
    prhu,
    swap,
    swpss,
    lockd,
    FIELD_LEN,
} field_type;

typedef struct segtype {
    char range[18];
    char perm[5];
    char offset[9];
    char device[6];
    char inode[9];
    char path[256];
    int  fields[FIELD_LEN];
    struct segtype *next;
} type_seg;

type_seg* smaps(int pid, const char* _filename, int arg_field);
void smaps_free(type_seg **smaps_head);

#endif //_SMAPS_H
