
#ifndef FAT_H_
#define FAT_H_

#include <stdint.h>

#define SECTOR_SIZE 256
#define NUMBER_OF_SECTORS (8*256)
#define SECTORS_PER_CLUSTER 8

typedef struct Disk_t Disk_t;

void Disk_read_sector(Disk_t* disk, uint8_t buffer[SECTOR_SIZE], uint16_t sid);
void Disk_write_sector(Disk_t* disk, uint8_t buffer[SECTOR_SIZE], uint16_t sid);

#endif // FAT_H_
