#include <ctype.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFLEN (256)

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

char *(field_name_short[]) = {
    "size",
    "kps",
    "mmu",
    "rss",
    "pss",
    "shrc",
    "shrd",
    "pric",
    "prid",
    "ref",
    "anon",
    "lzf",
    "anonp",
    "shmap",
    "shhu",
    "prhu",
    "swap",
    "swpss",
    "lockd",
};

char *(field_name[]) = {
    "Size:",
    "KernelPageSize:",
    "MMUPageSize:",
    "Rss:",
    "Pss:",
    "Shared_Clean:",
    "Shared_Dirty:",
    "Private_Clean:",
    "Private_Dirty:",
    "Referenced:",
    "Anonymous:",
    "LazyFree:",
    "AnonHugePages:",
    "ShmemPmdMapped:",
    "Shared_Hugetlb:",
    "Private_Hugetlb:",
    "Swap:",
    "SwapPss:",
    "Locked:",
};

typedef struct {
    char range[18];
    char perm[5];
    char offset[9];
    char device[6];
    char inode[9];
    char path[BUFLEN];
    int  fields[FIELD_LEN];
} type_seg;

int seg_index;
#define MAX_SEGS 4096
type_seg *(segs[MAX_SEGS]);

char linebuf[BUFSIZ];

#define RET_SYNTAX_ERROR(x) do{fprintf(stderr,"Syntax error!\n%s\n",x);return 1;}while(0)

void print_usage(void) {
    int i;
    printf("Usage: smaps [-p pid | -f filename] [-s field]\n");
    printf("Analyze memory statistics in use of /proc/<pid>/smaps.\n");
    printf("Optional arguments:\n");
    printf("  -p <pid>      pid of the target process.\n");
    printf("  -f <filename> use <filename> instead of /proc/<pid>/smaps\n");
    printf("  -s <field>    only list memory usage by <field>, including:\n");
    for (i = 0; i < FIELD_LEN; i++) {
        printf("    %8s    %s (kB)\n",field_name_short[i],field_name[i]);
    }
    printf("Example:\n");
    printf("    To get summary memory info:\n");
    printf("         smaps -p <pid>\n");
    printf("    Find out the most 'pss' consumption of the 'rw-p' segments:\n");
    printf("         smaps -p <pid> -s pss | grep -e ' rw-p '| sort -rn | head\n");
}

int arg_pid = 0;
char *arg_filename = 0;
int arg_field = -1;
char filename[BUFSIZ];

int main(const int argc, char **argv) {
    int c,i,j;
    FILE *file;
    type_seg seg_sum;

    while ((c = getopt(argc, argv, "p:f:s:")) != -1) {
        switch (c) {
            case 'p':
                arg_pid = atoi(optarg);
                if (!arg_pid) {
                    fprintf(stderr,"%s is not a valid pid\n",optarg);
                    return 1;
                }
                break;
            case 'f':
                arg_filename = optarg;
                break;
            case 's':
                if (arg_field != -1) {
                    fprintf(stderr,"Only one -s <field> is sported.\n");
                    return 1;
                }
                for (i = 0; i < FIELD_LEN; i++) {
                    if (strcmp(optarg,field_name_short[i]) == 0) {
                        arg_field = i;
                        break;
                    }
                }
                if (i == FIELD_LEN) {
                    fprintf(stderr,"%s is not a valid field name.\n",optarg);
                    return 1;
                }
                break;
            default:
                print_usage();
                return 1;
        }
    }

    if (!(!arg_pid != !arg_filename)) {
        print_usage();
        return 1;
    }

    if (arg_pid) {
        sprintf(filename,"/proc/%d/smaps",arg_pid);
    } else {
        sprintf(filename,"%s",arg_filename);
    }

    file = fopen(filename, "r");
    if (!file) {
        perror(filename);
        return 1;
    }

    memset(segs, 0, sizeof(segs));
    seg_index = 0;

    while (1) {
        if (!fgets(linebuf, sizeof(linebuf), file)) {
            if (ferror(file)) {
                perror(filename);
                return 1;
            } else {
                break;
            }
        }

        type_seg *preg = malloc(sizeof(type_seg));
        if (!preg) {
            perror("malloc failed!\n");
            return 1;
        }

        memset(preg, 0, sizeof(type_seg));
        if (sscanf(linebuf,"%s%s%s%s%s%s",
                    preg->range,
                    preg->perm,
                    preg->offset,
                    preg->device,
                    preg->inode,
                    preg->path) < 5) {
            RET_SYNTAX_ERROR(linebuf);
        }

        for (i = 0; i < FIELD_LEN; i++) {
            char _name[BUFSIZ];
            int _value;
            memset(_name,0,sizeof(_name));
            if (!fgets(linebuf, sizeof(linebuf), file))
                RET_SYNTAX_ERROR(linebuf);
            if (sscanf(linebuf, "%s%d", _name, &_value) != 2)
                RET_SYNTAX_ERROR(linebuf);
            for (j = 0; j < FIELD_LEN; j++) {
                if (strcmp(_name,field_name[j]) == 0) {
                    preg->fields[j] = _value;
                    break;
                }
            }
            if (j == FIELD_LEN) {
                RET_SYNTAX_ERROR(linebuf);
            }
        }
        // ignore VmFlags line.
        fgets(linebuf, sizeof(linebuf), file);

        segs[seg_index++] = preg;
        if (seg_index >= MAX_SEGS) {
            fprintf(stderr,"Too many segments.\n");
            return 1;
        }
    }
    fclose(file);

    if (arg_field == -1) {
        memset(&seg_sum, 0, sizeof(seg_sum));
        for (i = 0; i < seg_index; i++) {
            for (j = 0; j < FIELD_LEN; j++) {
                if (j == kps || j == mmu)
                    seg_sum.fields[j] = (segs[i]->fields)[j];
                else
                    seg_sum.fields[j] += (segs[i]->fields)[j];
            }
        }
        printf("\n%20s  %d mappings\n\n","Total:",seg_index-1);
        for (i = 0; i < FIELD_LEN; i++) {
            printf("%20s  %d kB\n",field_name[i],seg_sum.fields[i]);
        }
    } else {
        for (i = 0; i < seg_index; i++) {
            printf("%6d kB %s %s %s %s %s %s\n",
                    segs[i]->fields[arg_field],
                    segs[i]->range,
                    segs[i]->perm,
                    segs[i]->offset,
                    segs[i]->device,
                    segs[i]->inode,
                    segs[i]->path);
        }
    }

    for (i = 0; i < seg_index; i++) {
        free(segs[i]);
    }

    return 0;
}
