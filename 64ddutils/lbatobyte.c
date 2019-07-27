#include <stdio.h>
#include "dd_globals.h"

int lbatobyte(int disktype, int nlbas, int startlba)
{
    int totalbytes = 0;
    char init_flag = 1;
    int vzone = 1;
    int pzone = 0;
    int lba = startlba;
    int lba_count = nlbas;
    int blkbytes = 0;
    if (nlbas != 0)
    {
        for (; lba_count != 0; lba_count--)
        {
            if ((init_flag) || (VZONE_LBA_TBL[disktype][vzone] == lba))
            {
                vzone = LBAToVZone(lba, disktype);
                pzone = VZoneToPZone(vzone, disktype);
                if (7 < pzone)
                {
                    pzone -= 7;
                }
                blkbytes = BLOCK_SIZES[pzone];
            }
            totalbytes += blkbytes;
            lba++;
            init_flag = 0;
        }
    }
    return totalbytes;
}

int main(int argc, char const *argv[])
{
    int disktype = 0;
    int nlbas = 0;
    int startlba = 0;
    if (argc < 4)
    {
        printf("usage: lbatobyte <type> <nlbas> <startlba>\n");
    }
    else
    {
        if (sscanf(argv[1], "%d", &disktype) <= 0)
            return 1;
        if (sscanf(argv[2], "%d", &nlbas) <= 0)
            return 1;
        if (sscanf(argv[3], "%d", &startlba) <= 0)
            return 1;
        
        printf("%d\n", lbatobyte(disktype, nlbas, startlba));
    }
    return 0;
}
