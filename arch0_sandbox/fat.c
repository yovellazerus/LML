
#include "fat.h"

struct Disk_t {
    uint8_t data[NUMBER_OF_SECTORS][SECTOR_SIZE];
};

void Disk_read_sector(Disk_t* disk, uint8_t buffer[SECTOR_SIZE], uint16_t sid){
    if(!disk || !buffer || sid >= NUMBER_OF_SECTORS) return;
}

void Disk_write_sector(Disk_t* disk, uint8_t buffer[SECTOR_SIZE], uint16_t sid){
    if(!disk || !buffer || sid >= NUMBER_OF_SECTORS) return;
}
