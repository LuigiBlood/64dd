#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <stdint.h>
#include <libdragon.h>

static resolution_t res = RESOLUTION_320x240;
static bitdepth_t bit = DEPTH_32_BPP;

//64DD RTC
static unsigned char year = 0;
static unsigned char month = 0;
static unsigned char day = 0;
static unsigned char hour = 0;
static unsigned char min = 0;
static unsigned char sec = 0;

int detect64dd_ipl(void)
{
    //Look at 0x9FF00 from the IPL.
    uint32_t test = io_read(0x0609FF00);

    //Check if the highest byte is 0xC3
    test &= 0xFF000000;
    if (test == 0xC3000000)
	return 1;	//if yes, then retail 64DD is present (JPN)
    else if (test == 0x04000000)
	return 2;	//it has found a potential US 64DD.
    else
	return 0;	//if not, then there are no 64DD connected.
}

int detectdisk(void)
{
    uint32_t test = io_read(0x05000508);	//read status

    test &= 0x01000000;		//mask disk status

    if (test == 0x01000000)
	return 1;	//disk found
    else
	return 0;	//disk not found
}

void wait64dd_ready(void)
{
    while ((io_read(0x05000508) & 0x00800000) == 0x00800000)
    {
	//DO NOTHING. JUST WAIT.
    }
}

void getRTC_64dd(void)
{
    uint32_t temp;
    wait64dd_ready(); //wait for 64DD to be ready

    io_write(0x05000508, 0x00120000);	//request year and date
    wait64dd_ready();
    temp = io_read(0x05000500); //get data

    year = (unsigned char)((temp & 0xFF000000) >> 24);
    month = (unsigned char)((temp & 0x00FF0000) >> 16);
    

    wait64dd_ready(); //wait for 64DD to be ready

    io_write(0x05000508, 0x00130000);	//request day and hour
    wait64dd_ready();
    temp = io_read(0x05000500); //get data

    day = (unsigned char)((temp & 0xFF000000) >> 24);
    hour = (unsigned char)((temp & 0x00FF0000) >> 16);


    wait64dd_ready(); //wait for 64DD to be ready

    io_write(0x05000508, 0x00140000);	//request min and sec
    wait64dd_ready();
    temp = io_read(0x05000500); //get data

    min = (unsigned char)((temp & 0xFF000000) >> 24);
    sec = (unsigned char)((temp & 0x00FF0000) >> 16);
}

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
