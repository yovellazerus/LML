
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define SECTOR_SIZE 256
#define NUMBER_OF_SECTORS (8*256)

typedef uint8_t byte;

typedef struct Sector {
    byte data[SECTOR_SIZE];
} Sector;

typedef struct Disk {
    Sector data[NUMBER_OF_SECTORS];
} Disk;

void read_sector(Disk* disk, int sector_number, Sector* buffer){
    if(!disk || sector_number < 0 || sector_number >= NUMBER_OF_SECTORS || !buffer) return;
    memcpy(&buffer->data, &disk->data[sector_number].data, SECTOR_SIZE);
} 

void write_sector(Disk* disk, int sector_number, Sector* buffer){
    if(!disk || sector_number < 0 || sector_number >= NUMBER_OF_SECTORS || !buffer) return;
    memcpy(&disk->data[sector_number].data, buffer->data, SECTOR_SIZE);
}

void print_sector(Sector* buffer, int sector_number){
    if(!buffer) return;
    printf("sector: %d", sector_number);
    for(int i = 0; i < SECTOR_SIZE; i++){
        if(i % 16 == 0){
            printf("\n%.2X: ", i);
        }
        printf("%.2X ", buffer->data[i]);
    }
    printf("\n");
} 

int main()
{
    byte buffer[SECTOR_SIZE] = {0};
    Disk disk = {0};

    int sector_number = 32;  // reserve for the root dir 
    Sector s0 = {0};
    s0.data[0] = 'b';    // name
    s0.data[1] = 'i';
    s0.data[2] = 'n';

    s0.data[16] = 0x01;  // type: dir

    s0.data[17] = 0x40;  //size in bytes
    s0.data[18] = 0x60;
    s0.data[19] = 0x01;

    s0.data[20] = 0x03;  // fat addr low than hight
    s0.data[21] = 0x00;

    s0.data[22] = 0x01;  // time stamp
    s0.data[23] = 0x02;
    s0.data[24] = 0x03;
    s0.data[25] = 0x04;

    int bin_number = 3;
    Sector s1 = {0};
    s1.data[0] = 'a';    // name
    s1.data[1] = 's';
    s1.data[2] = 's';
    s1.data[3] = 'e';
    s1.data[4] = 'm';
    s1.data[5] = 'b';
    s1.data[6] = 'l';
    s1.data[7] = 'e';
    s1.data[8] = 'r';
    s1.data[9] = '.';
    s1.data[10] = 'e';
    s1.data[11] = 'x';
    s1.data[12] = 'e';

    s1.data[16] = 2;     // type: bin file

    s1.data[17] = 0x00;  //size in bytes
    s1.data[18] = 0x20;
    s1.data[19] = 0x00;

    s1.data[20] = 0x05;  // fat addr low than hight
    s1.data[21] = 0x00;

    s1.data[22] = 0x10;  // time stamp
    s1.data[23] = 0x20;
    s1.data[24] = 0x30;
    s1.data[25] = 0x40;

    write_sector(&disk, sector_number, &s0);
    write_sector(&disk, bin_number, &s1);

    print_sector(&disk.data[sector_number], sector_number);
    print_sector(&disk.data[bin_number], bin_number);

    return 0;
}
