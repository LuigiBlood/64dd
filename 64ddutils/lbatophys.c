#include <stdio.h>
#include "dd_globals.h"

int lbatophys(int lba, unsigned char* sys_data)
{
    int start_block = 0;
    int vzone, pzone = 0;
    int param_head, param_zone, param_cylinder = 0;
    int vzone_lba, cylinder_temp, pzone_defect_num, defect_num = 0;
    
    int disktype = sys_data[5] & 0xF;
    lba += 24;

    //Unused here
    if (((lba & 3) == 0) || ((lba & 3) == 3))
        start_block = 0;
    else
        start_block = 1;
    
    vzone = LBAToVZone(lba, disktype);
    pzone = VZoneToPZone(vzone, disktype);

    param_head = (7 < pzone);
    param_zone = pzone;
    if (param_head != 0)
        param_zone = pzone - 7;
    
    if (vzone == 0)
        vzone_lba = 0;
    else
        vzone_lba = VZONE_LBA_TBL[disktype][vzone - 1];
    
    param_cylinder = (lba - vzone_lba >> 1);
    cylinder_temp = SCYL_ZONE_TBL[0][pzone];
    if (param_head != 0)
    {
        param_cylinder = -param_cylinder;
        cylinder_temp = OUTERCYL_TBL[param_zone - 15];
    }
    param_cylinder += SCYL_ZONE_TBL[0][pzone];

    if (pzone == 0)
        pzone_defect_num = 0;
    else
        pzone_defect_num = sys_data[7 + pzone];
    
    defect_num = sys_data[8 + pzone] - pzone_defect_num;
    while ((defect_num != 0) && ((sys_data[0x20 + pzone_defect_num] + cylinder_temp) <= param_cylinder))
    {
        param_cylinder++;
        pzone_defect_num++;
        defect_num--;
    }

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
