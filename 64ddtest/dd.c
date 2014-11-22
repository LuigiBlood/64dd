#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <stdint.h>
#include <libdragon.h>

#include "dd.h"

const uint32_t MSEQdata[] =
	{0x00010000, 0x00020200, 0x80030100, 0x82040000,
	0xA8050000, 0xA0060600, 0x31760000, 0x00020300,
	0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00060000};

const uint8_t SecSizes[] =
	{232, 216, 208, 192, 176, 160, 144, 128, 112};	//Zones 0-8

const uint8_t C1_Length = 7;

const uint8_t Tracks[] =
	{150, 150, 141, 141, 141, 141, 141, 106, 0,	//Head 0 (Zones 0-8)
	0, 150, 150, 141, 141, 141, 141, 141, 106};	//Head 1 (Zones 0-8)

const uint8_t Zones[] =
	{0, 1, 2, 3, 4, 5, 6, 7, 8,
	0, 1, 2, 3, 4, 5, 6, 7, 8};

static unsigned int stat, bm_stat, err_stat = 0;

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
    while ((io_read(ASIC_STATUS) & STAT) != STAT);
}

void wait64dd_statusOFF(uint32_t STAT)
{
    while ((io_read(ASIC_STATUS) & STAT) == STAT);
}

uint32_t readDiskID(void)
{
    uint32_t * diskIDsector = malloc(60 * sizeof(uint32_t));
    diskIDsector[0] = 0x12345678; //TEST
    readDiskSectorLBA(14, 0, diskIDsector);
    uint32_t diskIDs = *(uint32_t*)diskIDsector;
    free(diskIDsector);
    return diskIDs;
}

void BMReset(uint8_t sector) 
{
    io_write(ASIC_BM_CTL, (BM_MODE | BM_RESET | ((uint32_t)sector << 16)) );    // Set BM (Bit Micro?) Reset. Leave BM_MODE set.
    io_write(ASIC_BM_CTL, (BM_MODE | ((uint32_t)sector << 16)));  		   // Clear BM (Bit Micro?) reset. Leave BM_MODE set.
}

void StartBM(uint8_t sector) 
{
    io_write(ASIC_BM_CTL, (START_BM | BM_MODE | ((uint32_t)sector << 16)));    // Set SECTOR offset (within the Track) depending on whether the LBA is Odd / Even.
}

void sendMSEQ(uint32_t secsize)
{
    //Send MSEQ
    for (int i = 0; i < 16; i++)
    {
	if (i == 4)
	    io_write((MSEQ_RAM_ADDR + i * 4), (MSEQdata[i] | (secsize << 8)));
	else
	    io_write((MSEQ_RAM_ADDR + i * 4), MSEQdata[i]);
    }
}

void readDiskSectorLBA(uint8_t LBA, uint8_t sector, void * buffer)
{
    readDiskSector((LBA >> 1), (sector + (ALL_SECS_PER_BLK * (LBA & 1))), buffer);
}

void readDiskSector(uint8_t track, uint8_t sector, void * buffer)
{
    //Read single sector
    if (io_read(ASIC_STATUS) & LEO_STAT_MECHA_INT)
	io_write(ASIC_BM_CTL, BM_MECHA_INT_RESET);

    io_write(ASIC_DATA, (uint32_t)(track << 16));	//Read Track (LBA >> 1)
    io_write(ASIC_CMD, ASIC_RD_SEEK);	//SEEK CMD
    io_write(ASIC_BM_CTL, 0x00000000);	//Clear BM CTL
    io_write(ASIC_SEQ_CTL, 0x00000000);	//Clear SEQ CTL

    wait64dd_statusON(LEO_STAT_MECHA_INT);
    //io_write(ASIC_BM_CTL, BM_MECHA_INT_RESET);

    if (getZonefromTrack(track) == 0xFF)
	return;

    uint32_t SecSize = (uint32_t)SecSizes[getZonefromTrack(track)] - 1;

    sendMSEQ(SecSize);	//Send MSEQ
    io_write(ASIC_HOST_SECBYTE, (SecSize << 16));
    io_write(ASIC_SEC_BYTE, (0x59000000 | ((SecSize + C1_Length) << 16)));

    io_write(ASIC_SEQ_CTL, MICRO_PC_ENABLE);	//MSEQ enable

    BMReset(sector);
    StartBM(sector);

    printf("BM Check\n");
    console_render();

    wait64dd_statusON(LEO_STAT_BM_INT);

    while ((io_read(ASIC_BM_STATUS) & LEO_BMST_RUNNING) == LEO_BMST_RUNNING);
	//Wait until BM is done
    bm_stat = io_read(ASIC_BM_STATUS);

    printf("DATA REQ CHECK\n");
    console_render();
    //wait64dd_statusON(LEO_STAT_DATA_REQ);

    stat = io_read(ASIC_STATUS);
    if ((stat & LEO_STAT_DATA_REQ) == LEO_STAT_DATA_REQ)
    {
	printf("DMA !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
	console_render();
	//*(uint32_t*)buffer = io_read(ASIC_SECTOR_BUFF);
	dma_read(buffer, ASIC_SECTOR_BUFF, 240);
    }

    err_stat = io_read(ASIC_ERR_SECTOR);
}

void PRINTSTAT(void)
{
    printf("\nASIC_STATUS:    0x%08x\nASIC_BM_STATUS: 0x%08x\nASIC_ERR_SECTOR: 0x%08x\n", stat, bm_stat, err_stat);
}

uint8_t getZonefromTrack(uint8_t track)
{
    int Total = 0;
    for (int i = 0; i < 16; i++)
    {
	Total += Tracks[i];
	if (track < Total)
	    return Zones[i];
    }
    return 0xFF;
}
