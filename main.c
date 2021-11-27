#include <ctype.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "include/smaps.h"

extern char *(field_name_short[]);
extern char *(field_name[]);

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

int main(const int argc, char **argv) {
    int arg_pid = 0;
    char *arg_filename = NULL;
    int arg_field = -1;

    int c,i,j;

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

    type_seg *seglist = smaps(arg_pid, arg_filename, arg_field);

    if (arg_field == -1) {
        memset(&seg_sum, 0, sizeof(seg_sum));
        type_seg *seg = seglist;
        int count=0;
        while(seg != NULL) {
            for (j = 0; j < FIELD_LEN; j++) {
                if (j == kps || j == mmu)
                    seg_sum.fields[j] = (seg->fields)[j];
                else
                    seg_sum.fields[j] += (seg->fields)[j];
            }
            count++;
            seg = seg->next;
        }
        printf("\n%20s  %d mappings\n\n","Total:",count);
        for (i = 0; i < FIELD_LEN; i++) {
            printf("%20s  %d kB\n",field_name[i],seg_sum.fields[i]);
        }
    } else {

        type_seg *seg = seglist;
        while(seg != NULL) {
            printf("%6d kB %s %s %s %s %s %s\n",
                    seg->fields[arg_field],
                    seg->range,
                    seg->perm,
                    seg->offset,
                    seg->device,
                    seg->inode,
                    seg->path
            );
            seg = seg->next;
        }
    }

    smaps_free(&seglist);

    return 0;
}
