
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SECTOR_SIZE 256
#define NUMBER_OF_SECTORS (8*256)
#define FAT_SIZE 16
#define DIR_PER_SEC 8
#define ROOT_DIR_NUMBER FAT_SIZE

typedef unsigned char byte;

typedef struct Dir {
    byte name[16];
    byte attr;
    byte size[3];
    byte fat_entry[2];
    byte time_stamp[4];
    byte reserve[6];
} Dir;

void read_secotor(byte disk[NUMBER_OF_SECTORS][SECTOR_SIZE], byte buffer[SECTOR_SIZE], int sector_number){
    if(!disk || !buffer || sector_number < 0 || sector_number >= NUMBER_OF_SECTORS) return;
    memcpy(buffer, disk[sector_number], SECTOR_SIZE);
}

void write_secotor(byte disk[NUMBER_OF_SECTORS][SECTOR_SIZE], byte buffer[SECTOR_SIZE], int sector_number){
    if(!disk || !buffer || sector_number < 0 || sector_number >= NUMBER_OF_SECTORS) return;
    memcpy(disk[sector_number], buffer, SECTOR_SIZE);
}

void write_dir(byte disk[NUMBER_OF_SECTORS][SECTOR_SIZE], int sector_number, Dir* dir){
    if(!disk || !dir || sector_number < 0 || sector_number >= NUMBER_OF_SECTORS) return;
    
    byte buffer[SECTOR_SIZE] = {0};
    read_secotor(disk, buffer, sector_number);
    int i;
    for(i = 0; i < DIR_PER_SEC; i++){
        if(buffer[i*32] == '\0'){
            break;
        }
    }
    if(i == DIR_PER_SEC){
        printf("sector full.\n");
        return;
    }
    
    memcpy(buffer + i*32 + 0, dir->name, 16);
    memcpy(buffer + i*32 + 16, &dir->attr, 1);
    memcpy(buffer + i*32 + 17, dir->size, 3);
    memcpy(buffer + i*32 + 20, dir->fat_entry, 2);
    memcpy(buffer + i*32 + 22, dir->time_stamp, 4);
    memcpy(buffer + i*32 + 26, dir->reserve, 6);
    
    write_secotor(disk, buffer, sector_number);
}

void write_to_fat(byte disk[NUMBER_OF_SECTORS][SECTOR_SIZE], int entry, int next_claster){
   
}

void dump_sector(byte disk[NUMBER_OF_SECTORS][SECTOR_SIZE], int sector_number){
    if(!disk || sector_number < 0 || sector_number >= NUMBER_OF_SECTORS) return;
    printf("sector number: %d", sector_number);
    for(int i = 0; i < SECTOR_SIZE; i++){
        if(i % 16 == 0){
            printf("\n%.2X: ", i);
        }
        printf("%.2X ", disk[sector_number][i]);
    }
    printf("\n");
}

void dump_fat(byte disk[NUMBER_OF_SECTORS][SECTOR_SIZE]){
    if(!disk) return;
    printf("FAT:\n" "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
    for(int i = 0; i < FAT_SIZE; i++){
        dump_sector(disk, i);
    }
}

int main()
{
    byte disk[NUMBER_OF_SECTORS][SECTOR_SIZE] = {0};
    
    byte buffer[SECTOR_SIZE] = {0};
    
    int bin_num  = 0x0020;
    int user_num = 0x0070;
    
    Dir bin = { .name = {'b', 'i', 'n'}, 
                .attr = 0x01, 
                .size = {0x20, 0x30, 0x01}, 
                .fat_entry = {0x20, 0x00}, 
                .time_stamp = {0x10, 0x20, 0x30, 0x40}
        
    }; 
    
    Dir usr = { .name = {'u', 's', 'r'}, 
                .attr = 0x01, 
                .size = {0xff, 0xfe, 0x03}, 
                .fat_entry = {0x70, 0x00}, 
                .time_stamp = {0x11, 0x21, 0x31, 0x41}
        
    }; 
    
    write_dir(disk, ROOT_DIR_NUMBER, &bin);
    write_dir(disk, ROOT_DIR_NUMBER, &usr);
    
    printf("root directory: \n" "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
    dump_sector(disk, ROOT_DIR_NUMBER);
    
    dump_fat(disk);

    return 0;
}



