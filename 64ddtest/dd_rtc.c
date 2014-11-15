#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <stdint.h>
#include <libdragon.h>

#include "dd.h"
#include "dd_rtc.h"

unsigned char year;
unsigned char month;
unsigned char day;
unsigned char hour;
unsigned char min;
unsigned char sec;

void getRTC_64dd(void)
{
    uint32_t temp;
    wait64dd_ready(); //wait for 64DD to be ready

    io_write(ASIC_CMD, ASIC_READ_TIMER_YEAR);	//request year and month
    for (int i = 0; i < 100; i++)
	wait64dd_ready();
    temp = io_read(ASIC_DATA); //get data

    year = (unsigned char)((temp & 0xFF000000) >> 24);
    month = (unsigned char)((temp & 0x00FF0000) >> 16);
    

    wait64dd_ready(); //wait for 64DD to be ready

    io_write(ASIC_CMD, ASIC_READ_TIMER_DATE);	//request day and hour
    for (int i = 0; i < 100; i++)
	wait64dd_ready();
    temp = io_read(ASIC_DATA); //get data

    day = (unsigned char)((temp & 0xFF000000) >> 24);
    hour = (unsigned char)((temp & 0x00FF0000) >> 16);


    wait64dd_ready(); //wait for 64DD to be ready

    io_write(ASIC_CMD, ASIC_READ_TIMER_MINUTE);	//request min and sec
    wait64dd_ready();
    temp = io_read(ASIC_DATA); //get data

    min = (unsigned char)((temp & 0xFF000000) >> 24);
    sec = (unsigned char)((temp & 0x00FF0000) >> 16);
}
