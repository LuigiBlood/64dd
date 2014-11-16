#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <stdint.h>
#include <libdragon.h>

#include "dd.h"

int detect64dd_ipl(void)
{
    //Look at 0x9FF00 from the IPL.
    uint32_t test = io_read(IPL_CHECK);

    //Isolate the highest byte
    test &= 0xFF000000;

    if (test == 0xC3000000)
	return 1;	//if yes, then retail 64DD is present (JPN)
    else if (test == 0x04000000)
	return 2;	//it has found a potential US 64DD.
    else
    {
	test = io_read(ASIC_ID_REG);
	if (test == 0x00040000)
	    return 3;
	else
	    return 0;	//if not, then there are no 64DD connected.
    }
}

int detectdisk(void)
{
    uint32_t test = io_read(ASIC_STATUS);	//read status

    test &= LEO_STAT_DISK;		//mask disk status

    if (test == LEO_STAT_DISK)
	return 1;	//disk found
    else
	return 0;	//disk not found
}

void wait64dd_statusON(uint32_t STAT)
{
    while ((io_read(ASIC_STATUS) & STAT) != STAT)
    {
	//DO NOTHING. JUST WAIT.
    }
}

void wait64dd_statusOFF(uint32_t STAT)
{
    while ((io_read(ASIC_STATUS) & STAT) == STAT)
    {
	//DO NOTHING. JUST WAIT.
    }
}
