### Usage

    Usage: smaps [-p pid | -f filename] [-s field]
    Analyze memory statistics in use of /proc/<pid>/smaps.
    Optional arguments:
      -p <pid>      pid of the target process.
      -f <filename> use <filename> instead of /proc/<pid>/smaps
      -s <field>    only list memory usage by <field>, including:
            size    Size: (kB)
             rss    Rss: (kB)
             pss    Pss: (kB)
            shrc    Shared_Clean: (kB)
            shrd    Shared_Dirty: (kB)
            pric    Private_Clean: (kB)
            prid    Private_Dirty: (kB)
             ref    Referenced: (kB)
            anon    Anonymous: (kB)
           anonp    AnonHugePages: (kB)
            swap    Swap: (kB)
             kps    KernelPageSize: (kB)
             mmu    MMUPageSize: (kB)
           lockd    Locked: (kB)
    Example:
        To get summary memory info:
             smaps -p <pid>
        Find out the most 'pss' consumption segments:
             smaps -p <pid> -s pss | grep -e ' rw-p ' | sort -rn | head

### Example Output

               Total:  324 mappings
 
                Size:  174848 kB
                 Rss:  27124 kB
                 Pss:  10531 kB
        Shared_Clean:  15500 kB
        Shared_Dirty:  3724 kB
       Private_Clean:  1540 kB
       Private_Dirty:  6360 kB
          Referenced:  24104 kB
           Anonymous:  9860 kB
       AnonHugePages:  0 kB
                Swap:  0 kB
      KernelPageSize:  1300 kB
         MMUPageSize:  1300 kB
              Locked:  0 kB
 

      2456 kB b8366000-b8666000 rw-p 00000000 00:00 0 [heap]
      1152 kB b8227000-b8366000 rw-p 00000000 00:00 0 [heap]
       596 kB 50800000-50900000 rw-p 00000000 00:00 0
       292 kB 3e000000-3e049000 rw-p 00000000 00:00 0
       192 kB 2b300000-2b339000 rw-p 00000000 00:00 0
       164 kB 5e500000-5e529000 rw-p 00000000 00:00 0
       140 kB b7983000-b79d8000 rw-p 00000000 00:00 0
       132 kB 58900000-58a00000 rw-p 00000000 00:00 0
