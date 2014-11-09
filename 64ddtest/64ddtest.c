#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <stdint.h>
#include <libdragon.h>

#include "dd.h"
#include "dd_rtc.h"

static resolution_t res = RESOLUTION_320x240;
static bitdepth_t bit = DEPTH_32_BPP;

unsigned char year;
unsigned char month;
unsigned char day;
unsigned char hour;
unsigned char min;
unsigned char sec;

int main(void)
{
    /* enable interrupts (on the CPU) */
    init_interrupts();

    /* Initialize peripherals */
    display_init( res, bit, 2, GAMMA_NONE, ANTIALIAS_RESAMPLE );
    console_init();
    controller_init();

    console_set_render_mode(RENDER_MANUAL);

    //64DD IPL test
    int dd_present = detect64dd_ipl();

    if (dd_present > 0)
	getRTC_64dd();

    /* Main loop test */
    while(1) 
    {
        console_clear();

        /* To do initialize routines */
        controller_scan();

	printf("64dd_test by LuigiBlood\n\n");

	switch (dd_present)
	{
	    case 1:
		printf("Retail 64DD found (JPN)\n\n");
		break;
	    case 2:
		printf("Retail 64DD found (USA)\n\n");
		break;
	    default:
		printf("Retail 64DD NOT found\n\n");
		break;
	}

	//NOTE: getRTC_64dd() IS PERFORMED ONLY ONCE.
	//THE DATE WILL NOT UPDATE IN REALTIME.
	//DATE: MM/DD/YY - HH:MM:SS
	if (dd_present)
	    printf("DATE: %02x/%02x/%02x - %02x:%02x:%02x\n\n", month, day, year, hour, min, sec);

	//disk test
	if (dd_present != 0)
	{
		if (detectdisk() == 1)
			printf("DISK FOUND\n\n");
		else
			printf("DISK NOT FOUND\n\n");
	}

	//random controller test
        struct controller_data keys = get_keys_down();

        int controllers = get_controllers_present();

        printf( "Controller 1 %spresent\n", (controllers & CONTROLLER_1_INSERTED) ? "" : "not " );

        if( keys.c[0].A )
        {
            printf("YOU PRESSED A");
        }

        console_render();
    }
}
