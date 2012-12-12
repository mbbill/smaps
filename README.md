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
        Examine the most 'pss' consumption segments:
             smaps -p <pid> -s pss | sort -rn | head
