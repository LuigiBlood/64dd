#ifndef DD_H
#define DD_H

//ASIC commands
#define ASIC_NO_OPERATION          0x00000000
#define ASIC_RD_SEEK               0x00010001
#define ASIC_WR_SEEK               0x00020001
#define ASIC_RECAL                 0x00030001
#define ASIC_SLEEP                 0x00040000
#define ASIC_START                 0x00050001
#define ASIC_SET_STBY              0x00060000
#define ASIC_SET_SLEEP             0x00070000
#define ASIC_CLR_CHGFLG            0x00080000
#define ASIC_CLR_RSTFLG            0x00090000
#define ASIC_RD_VERSION            0x000a0000
#define ASIC_SET_DTYPE             0x000b0001
#define ASIC_REQ_STATUS            0x000c0000
#define ASIC_STANDBY               0x000d0000
#define ASIC_INDEX_LOCK_RETRY      0x000e0001
#define ASIC_SET_TIMER_YEAR        0x000f0000
#define ASIC_SET_TIMER_DATE        0x00100000
#define ASIC_SET_TIMER_MINUTE      0x00110000
#define ASIC_READ_TIMER_YEAR       0x00120000
#define ASIC_READ_TIMER_DATE       0x00130000
#define ASIC_READ_TIMER_MINUTE     0x00140000

#define ASIC_NEED_DISK_IN          0x00000001

//Memory Map
//----64DD Hardware
#define ASIC_IO_BASE       0x05000000
#define ASIC_C2_BUFF       (ASIC_IO_BASE+0x0000)
#define ASIC_SECTOR_BUFF   (ASIC_IO_BASE+0x0400)
#define MSEQ_RAM_ADDR      (ASIC_IO_BASE+0x0580)

#define ASIC_DATA          (ASIC_IO_BASE+0x0500) /* W / R */
#define ASIC_MISC_REG      (ASIC_IO_BASE+0x0504) /* R     */
#define ASIC_STATUS        (ASIC_IO_BASE+0x0508) /* R     */
#define ASIC_CMD           (ASIC_IO_BASE+0x0508) /* W     */
#define ASIC_CUR_TK        (ASIC_IO_BASE+0x050c) /* R     */
#define ASIC_BM_STATUS     (ASIC_IO_BASE+0x0510) /* R     */
#define ASIC_BM_CTL        (ASIC_IO_BASE+0x0510) /* W     */
#define ASIC_ERR_SECTOR    (ASIC_IO_BASE+0x0514) /* R     */
#define ASIC_SEQ_STATUS    (ASIC_IO_BASE+0x0518) /* R     */
#define ASIC_SEQ_CTL       (ASIC_IO_BASE+0x0518) /* W     */
#define ASIC_CUR_SECTOR    (ASIC_IO_BASE+0x051c) /* R     */
#define ASIC_HARD_RESET    (ASIC_IO_BASE+0x0520) /* W     */
#define ASIC_C1_S0         (ASIC_IO_BASE+0x0524) /* R     */
#define ASIC_HOST_SECBYTE  (ASIC_IO_BASE+0x0528) /* W / R */
#define ASIC_C1_S2         (ASIC_IO_BASE+0x052c) /* R     */
#define ASIC_SEC_BYTE      (ASIC_IO_BASE+0x0530) /* W / R */
#define ASIC_C1_S4         (ASIC_IO_BASE+0x0534) /* R     */
#define ASIC_C1_S6         (ASIC_IO_BASE+0x0538) /* R     */
#define ASIC_CUR_ADDR      (ASIC_IO_BASE+0x053c) /* R     */
#define ASIC_ID_REG        (ASIC_IO_BASE+0x0540) /* R     */
#define ASIC_TEST_REG      (ASIC_IO_BASE+0x0544) /* R     */
#define ASIC_TEST_PIN_SEL  (ASIC_IO_BASE+0x0548) /* W     */

//----64DD IPL
#define IPL_BASE           0x06000000
#define IPL_CHECK          (IPL_BASE+0x09FF00)

//Status bits
#define LEO_STAT_DATA_REQ      0x40000000
#define LEO_STAT_C2_XFER       0x10000000
#define LEO_STAT_BM_ERROR      0x08000000
#define LEO_STAT_BM_INT        0x04000000
#define LEO_STAT_MECHA_INT     0x02000000
#define LEO_STAT_DISK          0x01000000
#define LEO_STAT_BUSY          0x00800000
#define LEO_STAT_RESET         0x00400000
#define LEO_STAT_SPM_OFF       0x00100000
#define LEO_STAT_HEAD_RETRACT  0x00080000
#define LEO_STAT_WPROTECT_ERR  0x00040000
#define LEO_STAT_MECHA_ERROR   0x00020000
#define LEO_STAT_DISK_CHANGE   0x00010000
#define LEO_STAT_MASK          0xffff0000

#define LEO_STAT_NC_CHK        0x0000ffff

#define LEO_STAT_WRITE_END     0x00000000

//RESET
#define ASIC_RESET_CODE        0xaaaa0000

//BM STATUS
#define LEO_BMST_RUNNING       0x80000000 /* bit 15 */
#define LEO_BMST_ERROR         0x04000000 /* bit 10 */
#define LEO_BMST_MICRO_STATUS  0x02000000 /* bit  9 */
#define LEO_BMST_BLOCKS        0x01000000 /* bit  8 */
#define LEO_BMST_C1_CORRECT    0x00800000 /* bit  7 */
#define LEO_BMST_C1_DOUBLE     0x00400000 /* bit  6 */
#define LEO_BMST_C1_SINGLE     0x00200000 /* bit  5 */
#define LEO_BMST_C1_ERROR      0x00010000 /* bit  0 */

//BM CTL
#define START_BM               0x80000000
#define BM_MODE                0x40000000
#define BM_INT_MASK            0x20000000
#define BM_RESET               0x10000000
#define BM_DISABLE_OR_CHK      0x08000000
#define BM_DISABLE_C1          0x04000000
#define BM_XFERBLKS            0x02000000
#define BM_MECHA_INT_RESET     0x01000000

//Secs
#define USR_SECS_PER_BLK          85 /* Number of user sectors in a logical block */
#define C2_SECS_PER_BLK            4 /* Number of C2 sectors in a logical block */
#define GAP_SECS_PER_BLK           1 /* Number of GAP sectors in a logical block */
#define HALF_SECS_U16         0x5900 /* Valid number of sectors in a logical block(excluding GAP)<<16*/ // Note, 89 decimal (0x59 !)

#define ALL_SECS_PER_BLK         (USR_SECS_PER_BLK + C2_SECS_PER_BLK + GAP_SECS_PER_BLK)	// Note, 90 decimal (0x5A !)


//functions
int detect64dd_ipl(void);
int detectdisk(void);
void wait64dd_statusON(uint32_t STAT);
void wait64dd_statusOFF(uint32_t STAT);
uint32_t readDiskID(void);
void BMReset(uint8_t sector);
void StartBM(uint8_t sector);
void sendMSEQ(uint32_t secsize);
void readDiskSectorLBA(uint8_t LBA, uint8_t sector, void * buffer);
void readDiskSector(uint8_t track, uint8_t sector, void * buffer);
uint8_t getZonefromTrack(uint8_t track);

#endif
