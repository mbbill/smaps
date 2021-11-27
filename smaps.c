#include <ctype.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "include/smaps.h"

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

char linebuf[BUFSIZ];

void smaps_free(type_seg **smaps_head)
{
    type_seg *head = *smaps_head;
    while( head != NULL) {
        type_seg *tmp = head;
        head = head->next;
        free(tmp);
    }
    *smaps_head = NULL;
}

#define RET_SYNTAX_ERROR(x) do{fprintf(stderr,"Syntax error!\n%s\n",x); smaps_free(&head_seg); break;}while(0)

type_seg* smaps(int arg_pid, const char* arg_filename, int arg_field) {
    int i,j;
    FILE *file;
    type_seg *head_seg = NULL;
    char filename[64];

    if (arg_pid) {
        sprintf(filename,"/proc/%d/smaps", arg_pid);
    } else {
        sprintf(filename,"%s",arg_filename);
    }

    file = fopen(filename, "r");
    if (!file) {
        perror(filename);
        return NULL;
    }

    while (1) {
        if (!fgets(linebuf, sizeof(linebuf), file)) {
            if (ferror(file)) {
                perror(filename);
                smaps_free(&head_seg);
                return NULL;
            } else {
                break;
            }
        }

        //type_seg *preg = malloc(sizeof(type_seg));
        type_seg preg = {0};
        if (sscanf(linebuf,"%s%s%s%s%s%s",
                    preg.range,
                    preg.perm,
                    preg.offset,
                    preg.device,
                    preg.inode,
                    preg.path
                    ) < 5) {
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
                    preg.fields[j] = _value;
                    break;
                }
            }
            if (j == FIELD_LEN) {
                RET_SYNTAX_ERROR(linebuf);
            }
        }
        // ignore VmFlags line.
        char* ret = fgets(linebuf, sizeof(linebuf), file);
        if (!ret) { printf("failed to read line\n"); };

        type_seg *new_seg = malloc(sizeof(type_seg));
        *new_seg = preg;
        new_seg->next = head_seg;
        head_seg = new_seg;

    }
    fclose(file);

    // reverse list
    type_seg* current = head_seg;
    type_seg *prev = NULL, *next = NULL;
    while (current != NULL) {
        next = current->next;
        current->next = prev;
        prev = current;
        current = next;
    }
    head_seg = prev;

    return head_seg;
}
