#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <stdint.h>
#include <libdragon.h>

#include "dd.h"
#include "dd_rtc.h"

static resolution_t res = RESOLUTION_320x240;
static bitdepth_t bit = DEPTH_32_BPP;

char diskID[4];

int detect_exppak(void)
{
    io_write(0x00200000, 0xAAAA5555);
    uint32_t test = io_read(0x00200000);
    if (test == 0xAAAA5555)
	return 1;
    else
	return 0;
}

void emptydiskID(void)
{
    for (int i = 0; i < 4; i++)
	diskID[i] = 0;
}

int main(void)
{
    /* enable interrupts (on the CPU) */
    init_interrupts();

    emptydiskID();

    /* Initialize peripherals */
    display_init( res, bit, 2, GAMMA_NONE, ANTIALIAS_RESAMPLE );
    console_init();
    controller_init();

    console_set_render_mode(RENDER_MANUAL);

    console_clear();
    printf("64dd_test3 by LuigiBlood & OzOnE\n\n");
    console_render();

    //64DD IPL test
    int dd_present = detect64dd_ipl();

    if (dd_present > 0)
    {
	printf("64DD found\nApplying IPL PI settings...\n");
	console_render();
	//Set PI domain 2 settings like the 64DD IPL
	io_write(0x04600024, 3); //Set domain 2 latency
	io_write(0x04600028, 6); //Set domain 2 strobe pulse
	io_write(0x0460002C, 6); //Set domain 2 page size
	io_write(0x04600030, 2); //Set domain 2 release

	printf("Resetting 64DD...\n");
	console_render();
	//RESET 64DD
	io_write(ASIC_HARD_RESET, ASIC_RESET_CODE);

	while ((io_read(ASIC_STATUS) & LEO_STAT_RESET) == LEO_STAT_RESET)
	    io_write(ASIC_CMD, ASIC_CLR_RSTFLG); //clear reset flag

	io_write(ASIC_BM_CTL, BM_MECHA_INT_RESET);

	//get the date
	getRTC_64dd();
    }

    /* Main loop test */
    while(1) 
    {
        console_clear();

        /* To do initialize routines */
        controller_scan();

	printf("64dd_test3 by LuigiBlood & OzOnE\n\n");

	switch (dd_present)
	{
	    case 1:
		printf("Retail 64DD found (JPN)\n\n");
		break;
	    case 2:
		printf("Retail 64DD found (USA)\n\n");
		break;
	    case 3:
		printf("Development 64DD found\n\n");
		break;
	    default:
		printf("Retail 64DD NOT found\n\n");
		break;
	}

	//Expansion Pack detection
	if (detect_exppak())
	    printf("Expansion Pak found!\n\n");

	//NOTE: getRTC_64dd() IS PERFORMED ONLY ONCE.
	//THE DATE WILL NOT UPDATE IN REALTIME.
	//DATE: MM/DD/YY - HH:MM:SS
	if (dd_present)
	{
	    getRTC_64dd();
	    printf("DATE: %02x/%02x/%02x - %02x:%02x:%02x\n\n", month, day, year, hour, min, sec);
	}

	//disk test
	if (dd_present != 0)
	{
		if (detectdisk() == 1)
		{
			printf("DISK FOUND\n");
			printf("DISK ID: %c%c%c%c", diskID[0], diskID[1], diskID[2], diskID[3]);
			printf(" (0x%02x%02x%02x%02x)\n", diskID[0], diskID[1], diskID[2], diskID[3]);
		}
		else
		{
		    printf("DISK NOT FOUND\n");
		    emptydiskID();
		}
	}


	//random controller test
        struct controller_data keys = get_keys_down();

        int controllers = get_controllers_present();

        printf( "\nController 1 %spresent\n", (controllers & CONTROLLER_1_INSERTED) ? "" : "not " );

        if( keys.c[0].A && (dd_present != 0) && (detectdisk() == 1))
        {
            printf("Reading Disk ID...");
	    uint32_t idstuff = readDiskID();
	    diskID[0] = (char)((idstuff & 0xFF000000) >> 24);
	    diskID[1] = (char)((idstuff & 0x00FF0000) >> 16);
	    diskID[2] = (char)((idstuff & 0x0000FF00) >> 8);
	    diskID[3] = (char)((idstuff & 0x000000FF));
        }

        console_render();
    }
}
