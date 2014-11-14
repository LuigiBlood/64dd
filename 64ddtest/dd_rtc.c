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
    uint32_t temp, test;
    wait64dd_ready(); //wait for 64DD to be ready

    test = io_read(ASIC_DATA);

    io_write(ASIC_CMD, ASIC_READ_TIMER_YEAR);	//request year and month
    while (test == io_read(ASIC_DATA))
    {
	//Do nothing, wait for the date to come
    }
    temp = io_read(ASIC_DATA); //get data

    year = (unsigned char)((temp & 0xFF000000) >> 24);
    month = (unsigned char)((temp & 0x00FF0000) >> 16);
    

    wait64dd_ready(); //wait for 64DD to be ready

    test = io_read(ASIC_DATA);

    io_write(ASIC_CMD, ASIC_READ_TIMER_DATE);	//request day and hour
    while (test == io_read(ASIC_DATA))
    {
	//Do nothing, wait for the date to come
    }
    temp = io_read(ASIC_DATA); //get data

    day = (unsigned char)((temp & 0xFF000000) >> 24);
    hour = (unsigned char)((temp & 0x00FF0000) >> 16);


    wait64dd_ready(); //wait for 64DD to be ready

    test = io_read(ASIC_DATA);

    io_write(ASIC_CMD, ASIC_READ_TIMER_MINUTE);	//request min and sec
    while (test == io_read(ASIC_DATA))
    {
	//Do nothing, wait for the date to come
    }
    temp = io_read(ASIC_DATA); //get data

    min = (unsigned char)((temp & 0xFF000000) >> 24);
    sec = (unsigned char)((temp & 0x00FF0000) >> 16);
}
