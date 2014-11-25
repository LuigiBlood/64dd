#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <stdint.h>
#include <libdragon.h>

#include "dd.h"

static resolution_t res = RESOLUTION_320x240;
static bitdepth_t bit = DEPTH_32_BPP;

static unsigned short LBA = 0;
static unsigned char Sector = 0;
static int Offset = 0;
static unsigned char * SectorData;

int detect_exppak(void)
{
    io_write(0x00200000, 0xAAAA5555);
    uint32_t test = io_read(0x00200000);
    if (test == 0xAAAA5555)
	return 1;
    else
	return 0;
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

    console_clear();
    printf("64DDview by LuigiBlood & OzOnE\n\n");
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

	SectorData = calloc(0x100, sizeof(unsigned char));
    }

    /* Main loop test */
    while(1) 
    {
        console_clear();

        /* To do initialize routines */
        controller_scan();

	printf("64DDview by LuigiBlood & OzOnE\n");

	switch (dd_present)
	{
	    case 1:
		printf("- Retail 64DD found (JPN)\n");
		break;
	    case 2:
		printf("- Retail 64DD found (USA)\n");
		break;
	    case 3:
		printf("- Development 64DD found\n");
		break;
	    default:
		printf("- Retail 64DD NOT found\n");
		break;
	}

	//disk test
	if (dd_present != 0)
	{
		if (detectdisk() == 1)
		    printf("+ DISK FOUND\n");
		else
		    printf("- DISK NOT FOUND\n");
	}

	printf("LBA: %04u - Sector: %04u\n", LBA, Sector);
	printf("Left/Right: LBA - Up/Down: Offset\nC-Left/C-Right: Sector\n");
	printf("Press A to Read Sector\n");

	for (int y = Offset; y < (0x80 + Offset); y += 0x8)
	{
	    printf("\n%04x: ", y);
	    for (int x = 0; x < 0x8; x++)
		printf("%02x ", SectorData[y+x]);
	}

	//random controller test
        struct controller_data keys = get_keys_down();

        //int controllers = get_controllers_present();

        //printf( "\nController 1 %spresent\n", (controllers & CONTROLLER_1_INSERTED) ? "" : "not " );

        if( keys.c[0].A && (dd_present != 0) && (detectdisk() == 1))
        {
	    //READ SECTOR
	    printf("\n\nReading Sector...");
	    console_render();
	    readDiskSectorLBA(LBA, Sector, SectorData);
        }

	//SET LBA
	if( keys.c[0].left )
	    LBA--;

	if( keys.c[0].right )
	    LBA++;

	//SET OFFSET
	if( keys.c[0].up )
	    Offset = 0;

	if( keys.c[0].down )
	    Offset = 0x80;

	//SET SECTOR
	if( keys.c[0].C_left )
	{
	    if (Sector > 0)
		Sector--;
	    else
		Sector = ALL_SECS_PER_BLK;
	}

	if( keys.c[0].C_right )
	{
	    if (Sector < ALL_SECS_PER_BLK)
		Sector++;
	    else
		Sector = 0;
	}

        console_render();
    }
}
