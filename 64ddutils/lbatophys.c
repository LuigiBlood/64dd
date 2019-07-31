#include <stdio.h>
#include "dd_globals.h"

int lbatophys(int lba, unsigned char* sys_data)
{
    int start_block = 0;
    int vzone, pzone = 0;
    int param_head, param_zone, param_cylinder = 0;
    int vzone_lba, cylinder_zone_start, pzone_defect_offset, defect_amount = 0;
    
    int disktype = sys_data[5] & 0xF;

    //Skip System Area to correspond LBAs to SDK manual
    //lba += 24;

    //Unused here, get Block 0/1 on Disk Track
    if (((lba & 3) == 0) || ((lba & 3) == 3))
        start_block = 0;
    else
        start_block = 1;
    
    //Get Virtual & Physical Disk Zones
    vzone = LBAToVZone(lba, disktype);
    pzone = VZoneToPZone(vzone, disktype);

    //Get Disk Head
    param_head = (7 < pzone);

    //Get Disk Zone
    param_zone = pzone;
    if (param_head != 0)
        param_zone = pzone - 7;
    
    //Get Virtual Zone LBA start, if Zone 0, it's LBA 0
    if (vzone == 0)
        vzone_lba = 0;
    else
        vzone_lba = VZONE_LBA_TBL[disktype][vzone - 1];
    
    //Calculate Physical Cylinder
    param_cylinder = (lba - vzone_lba) >> 1;

    //Get the start cylinder from current zone
    cylinder_zone_start = SCYL_ZONE_TBL[0][pzone];
    if (param_head != 0)
    {
        //If Head 1, count from the other way around
        param_cylinder = -param_cylinder;
        cylinder_zone_start = OUTERCYL_TBL[param_zone - 1];
    }
    param_cylinder += SCYL_ZONE_TBL[0][pzone];

    //Get the relative offset to defect tracks for the current zone (if Zone 0, then it's 0)
    if (pzone == 0)
        pzone_defect_offset = 0;
    else
        pzone_defect_offset = sys_data[8 + pzone - 1];
    
    //Get amount of defect tracks for the current zone
    defect_amount = sys_data[8 + pzone] - pzone_defect_offset;

    //Skip defect tracks
    while ((defect_amount != 0) && ((sys_data[0x20 + pzone_defect_offset] + cylinder_zone_start) <= param_cylinder))
    {
        param_cylinder++;
        pzone_defect_offset++;
        defect_amount--;
    }

    //Return Cylinder and Head data for 64DD Seek command
    return param_cylinder | (param_head * 0x1000);
}

int main(int argc, char const *argv[])
{
    FILE *pFile;
    unsigned char* sys_data;
    int lba = 0;
    if (argc < 3)
    {
        printf("usage: lbatophys <file> <lba>\n");
        //returns TRACK + HEAD (0xHTTT)
    }
    else
    {
        if (sscanf(argv[2], "%d", &lba) <= 0)
            return 1;
        
        pFile = fopen(argv[1], "r");
        if (pFile == NULL)
            return 1;
        
        sys_data = (unsigned char*)malloc(sizeof(unsigned char)*0xE8);
        if (sys_data == NULL)
            return 1;
        
        fread(sys_data, 1, 0xE8, pFile);
        fclose(pFile);
        
        printf("%04x\n", lbatophys(lba, sys_data));

        free(sys_data);
    }
    return 0;
}
