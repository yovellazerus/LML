
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define RAM_SIZE 0x10000  // 64KB RAM
#define PC_INIT 0x0

// -----------------------------
// Bus abstraction
// -----------------------------
typedef struct Bus {
    uint8_t *ram;
    uint32_t (*read32)(struct Bus *bus, uint32_t addr);
    void (*write32)(struct Bus *bus, uint32_t addr, uint32_t value);
} Bus;

// Simple memory read/write
uint32_t bus_read32(Bus *bus, uint32_t addr) {
    if (addr + 3 >= RAM_SIZE) return 0;
    return bus->ram[addr] |
           (bus->ram[addr+1]<<8) |
           (bus->ram[addr+2]<<16) |
           (bus->ram[addr+3]<<24);
}

void bus_write32(Bus *bus, uint32_t addr, uint32_t value) {
    if (addr + 3 >= RAM_SIZE) return;
    bus->ram[addr]   = value & 0xFF;
    bus->ram[addr+1] = (value >> 8) & 0xFF;
    bus->ram[addr+2] = (value >> 16) & 0xFF;
    bus->ram[addr+3] = (value >> 24) & 0xFF;
}

void RAM_dump(const Bus* bus, const char* path) {
    FILE* f = fopen(path, "w");
    if (!f) {
        perror("RAM_dump fopen");
        return;
    }

    fprintf(f, "=== RAM HEXDUMP ===\n\n");

    for (uint32_t addr = 0; addr < RAM_SIZE; addr += 16) {

        // Print address
        fprintf(f, "%04X: ", addr);

        // Print hex bytes
        for (int i = 0; i < 16; i++) {
            uint8_t b = bus->ram[addr + i];
            fprintf(f, "%02X ", b);
        }

        fprintf(f, " ");

        // Print ASCII
        for (int i = 0; i < 16; i++) {
            uint8_t b = bus->ram[addr + i];
            if (b >= 32 && b <= 126)   // printable ASCII
                fputc(b, f);
            else
                fputc('.', f);
        }

        fputc('\n', f);
    }

    fclose(f);
}

// -----------------------------
// CPU structure
// -----------------------------
typedef struct CPU {
    uint32_t x[32];      // x0-x31
    uint32_t pc;         // program counter
    Bus *bus;            // pointer to the bus
} CPU;

// Forward declarations for instruction handlers
void handle_rtype(CPU *cpu, uint32_t inst);
void handle_itype(CPU *cpu, uint32_t inst);
void handle_stype(CPU *cpu, uint32_t inst);
void handle_btype(CPU *cpu, uint32_t inst);
void handle_utype(CPU *cpu, uint32_t inst);
void handle_jtype(CPU *cpu, uint32_t inst);

void CPU_dump(const CPU* cpu, const char* path) {
    FILE* f = fopen(path, "w");
    if (!f) {
        perror("CPU_dump fopen");
        return;
    }

    fprintf(f, "=== CPU STATE DUMP ===\n");
    fprintf(f, "PC = 0x%08X\n\n", cpu->pc);

    fprintf(f, "Registers:\n");
    for (int i = 0; i < 32; i++) {
        fprintf(f, "x%-2d = 0x%08X (%u)\n", i, cpu->x[i], cpu->x[i]);
    }

    fclose(f);
}

// -----------------------------
// Helper macros
// -----------------------------
#define OPCODE(inst)  ((inst) & 0x7F)
#define FUNCT3(inst)  (((inst)>>12) & 0x7)
#define FUNCT7(inst)  (((inst)>>25) & 0x7F)
#define RD(inst)      (((inst)>>7) & 0x1F)
#define RS1(inst)     (((inst)>>15) & 0x1F)
#define RS2(inst)     (((inst)>>20) & 0x1F)
#define IMM_I(inst)   ((int32_t)(inst) >> 20)
#define IMM_S(inst)   (((int32_t)((inst>>25)<<5)) | ((inst>>7)&0x1F))
#define IMM_B(inst)   (((int32_t)((inst>>31)<<12)) | (((inst>>7)&1)<<11) | (((inst>>25)&0x3F)<<5) | (((inst>>8)&0xF)<<1))
#define IMM_U(inst)   ((inst) & 0xFFFFF000)
#define IMM_J(inst)   (((int32_t)((inst>>31)<<20)) | (((inst>>12)&0xFF)<<12) | (((inst>>20)&1)<<11) | (((inst>>21)&0x3FF)<<1))

// -----------------------------
// Instruction implementations
// -----------------------------
void instr_add(CPU *cpu, uint32_t rd, uint32_t rs1, uint32_t rs2) {
    if (rd == 0) return;
    cpu->x[rd] = cpu->x[rs1] + cpu->x[rs2];
}

void instr_sub(CPU *cpu, uint32_t rd, uint32_t rs1, uint32_t rs2) {
    if (rd == 0) return;
    cpu->x[rd] = cpu->x[rs1] - cpu->x[rs2];
}

void instr_addi(CPU *cpu, uint32_t rd, uint32_t rs1, int32_t imm) {
    if (rd == 0) return;
    cpu->x[rd] = cpu->x[rs1] + imm;
}

void instr_lw(CPU *cpu, uint32_t rd, uint32_t rs1, int32_t imm) {
    if (rd == 0) return;
    uint32_t addr = cpu->x[rs1] + imm;
    cpu->x[rd] = cpu->bus->read32(cpu->bus, addr);
}

void instr_sw(CPU *cpu, uint32_t rs1, uint32_t rs2, int32_t imm) {
    uint32_t addr = cpu->x[rs1] + imm;
    cpu->bus->write32(cpu->bus, addr, cpu->x[rs2]);
}

// -----------------------------
// Step function
// -----------------------------
bool CPU_step(CPU *cpu) {
    uint32_t inst = cpu->bus->read32(cpu->bus, cpu->pc);
    uint32_t opcode = OPCODE(inst);

    switch(opcode) {
        case 0x33: handle_rtype(cpu, inst); break; // R-type
        case 0x13: handle_itype(cpu, inst); break; // I-type ALU
        case 0x03: handle_itype(cpu, inst); break; // I-type LOAD
        case 0x23: handle_stype(cpu, inst); break; // S-type STORE
        case 0x63: handle_btype(cpu, inst); break; // B-type BRANCH
        case 0x17: handle_utype(cpu, inst); break; // U-type AUIPC
        case 0x37: handle_utype(cpu, inst); break; // U-type LUI
        case 0x6F: handle_jtype(cpu, inst); break; // J-type JAL
        default:
            printf("Unknown opcode 0x%02X at PC=0x%08X\n", opcode, cpu->pc);
            return false;
    }
    cpu->x[0] = 0;
    cpu->pc += 4;
    return true;
}

// -----------------------------
// Instruction type dispatchers
// -----------------------------
void handle_rtype(CPU *cpu, uint32_t inst) {
    uint32_t rd = RD(inst);
    uint32_t rs1 = RS1(inst);
    uint32_t rs2 = RS2(inst);
    uint32_t funct3 = FUNCT3(inst);
    uint32_t funct7 = FUNCT7(inst);

    if(funct3 == 0x0) {
        if(funct7 == 0x00) instr_add(cpu, rd, rs1, rs2);
        else if(funct7 == 0x20) instr_sub(cpu, rd, rs1, rs2);
    }
    // Add more R-type instructions here
}

void handle_itype(CPU *cpu, uint32_t inst) {
    uint32_t rd = RD(inst);
    uint32_t rs1 = RS1(inst);
    int32_t imm = IMM_I(inst);
    uint32_t opcode = OPCODE(inst);

    if(opcode == 0x13) { // ADDI
        instr_addi(cpu, rd, rs1, imm);
    } else if(opcode == 0x03) { // LW
        instr_lw(cpu, rd, rs1, imm);
    }
    // Add more I-type instructions here
}

void handle_stype(CPU *cpu, uint32_t inst) {
    uint32_t rs1 = RS1(inst);
    uint32_t rs2 = RS2(inst);
    int32_t imm = IMM_S(inst);
    instr_sw(cpu, rs1, rs2, imm);
}

void handle_btype(CPU *cpu, uint32_t inst) {
    // Implement BEQ/BNE/BLT/BGE later
}

void handle_utype(CPU *cpu, uint32_t inst) {
    // Implement LUI/AUIPC later
}

void handle_jtype(CPU *cpu, uint32_t inst) {
    // Implement JAL later
}

// -----------------------------
// Main
// -----------------------------
int main() {
    // Allocate ram
    Bus bus = {0};
    bus.ram = calloc(RAM_SIZE, 1);
    bus.read32 = bus_read32;
    bus.write32 = bus_write32;

    // Initialize CPU
    CPU cpu = {0};
    cpu.bus = &bus;
    cpu.pc = PC_INIT;

    // Example program: x1 = 5; x2 = 10; x3 = x1 + x2; store x3 to ram[0]
    uint32_t program[] = {
        0x00500093, // ADDI x1, x0, 5
        0x00A00113, // ADDI x2, x0, 10
        0x002081B3, // ADD x3, x1, x2
        0x00302023  // SW x3, 0(x0)
    };
    memcpy(bus.ram + PC_INIT, program, sizeof(program));

    // Run program
    while(CPU_step(&cpu)) {}

    CPU_dump(&cpu, "cpu_dump.txt");
    RAM_dump(&bus, "ram_dump.txt");

    free(bus.ram);
    return 0;
}
