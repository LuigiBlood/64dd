#include <stdio.h>
#include "dd_globals.h"

int bytetolba(int disktype, int nbytes, int startlba)
{
    char init_flag = 1;
    int vzone = 1;
    int pzone = 0;
    int lba = startlba;
    int lba_count = 0;
    int byte_count = nbytes;
    int blkbytes = 0;
    if (nbytes != 0)
    {
        do
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
            if (byte_count < blkbytes)
            {
                byte_count = 0;
            }
            else
            {
                byte_count -= blkbytes;
            }
            lba++;
            lba_count++;
            init_flag = 0;
            if ((byte_count != 0) && (lba > 0x10db))
            {
                return -1;
            }
        } while (byte_count != 0);
    }
    return lba_count;
}

int main(int argc, char const *argv[])
{
    int disktype = 0;
    int nbytes = 0;
    int startlba = 0;
    if (argc < 4)
    {
        printf("usage: bytetolba <type> <nbytes> <startlba>\n");
    }
    else
    {
        if (sscanf(argv[1], "%d", &disktype) <= 0)
            return 1;
        if (sscanf(argv[2], "%d", &nbytes) <= 0)
            return 1;
        if (sscanf(argv[3], "%d", &startlba) <= 0)
            return 1;
        
        printf("%d\n", bytetolba(disktype, nbytes, startlba));
    }
    return 0;
}
