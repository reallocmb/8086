#include<stdio.h>
#include<stdint.h>
#include<stdlib.h>

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;

typedef struct Intel8086 {
    u8 memory[1024 * 1024];
} Intel8086;

u32 intel_8086_bytes_load_from_file(Intel8086 *intel_8086, char *file_path, u32 offset);
void intel_8086_disasm(Intel8086 *intel8086, u32 bytes);
void bytes_binary_print(u8 *memory, u32 bytes);

int main(int argc, char **argv)
{
    if (argc <= 1)
        return 0;

    Intel8086 intel_8086;
    u32 bytes = intel_8086_bytes_load_from_file(&intel_8086, argv[1], 0);
    bytes_binary_print(intel_8086.memory, bytes);
    intel_8086_disasm(&intel_8086, bytes);

    return 0;
}

u32 intel_8086_bytes_load_from_file(Intel8086 *intel_8086, char *file_path, u32 offset)
{
    FILE *f = fopen(file_path, "rb");
    if (f == NULL)
    {
        fprintf(stderr,
                "fopen() failed can't open file: %s\n", file_path);
        exit(1);
    }

    u32 bytes = 0;
    u32 bytes_read = 0;
    while ((bytes_read = fread(intel_8086->memory + bytes, 1, 2048, f)) != 0)
        bytes += bytes_read;

    return bytes;
}

typedef enum {
    BITS_LITERAL,
    BITS_D,
    BITS_W,
    BITS_MOD,
    BITS_REG,
    BITS_RM,
} BitsType;

typedef struct Bits {
    BitsType type;
    u8 count;
    u8 value;
} Bits;

typedef struct InstructionFormat {
    char *mnemonic;
    Bits bits[16];
} InstructionFormat;

InstructionFormat instruction_formats[] = {
    { "mov", /* register/memory to/from register */
        {
            { BITS_LITERAL, 6, 0b100010 },
            { BITS_D, 1 },
            { BITS_W, 1 },
            { BITS_MOD, 2 },
            { BITS_REG, 3 },
            { BITS_RM, 3 },
        } },
    { "mov", /* immediate to register */
        {
            { BITS_LITERAL, 4, 0b1011 },
            { BITS_W, 1 },
            { BITS_REG, 3 },
        } },
};

typedef struct Instruction {
    InstructionFormat *instruction_format;
    char *mnemonic;
    u8 bit_d;
    u8 bit_w;
    u8 bit_mod;
    u8 bit_reg;
    u8 bit_rm;
} Instruction;

void instruction_decode(Instruction *instruction, u8 *memory, u32 *i)
{
    u32 instruction_formats_count = sizleof(instruction_formats) / sizeof(*instruction_formats);
    u32 bits_offset = 0;
    while (--instruction_formats_count)
    {
        u32 count = instruction_formats[instruction_formats_count].bits[0].count;
        u32 shift = 8 - count;
        u32 bits = memory;
        bits >>= count;
        bits &= ~(0xff << count);
        if (bits == instruction_formats[instruction_formats_count].bits[0].value)
        {
        }
    }
}

void instruction_print(Instruction *instruction)
{
}

void intel_8086_disasm(Intel8086 *intel_8086, u32 bytes)
{
    Instruction instruction;
    u32 i = 0;
    while (bytes--)
    {
        instruction_decode(&instruction, intel_8086->memory, &i);
        instruction_print(&instruction);
    }
}

void bytes_binary_print(u8 *memory, u32 bytes)
{
    u32 i;
    for (i = 0; i < bytes; i++)
        printf("%5u %u%u%u%u%u%u%u%u\n",
               i,
               (memory[i] >> 7) & 1,
               (memory[i] >> 6) & 1,
               (memory[i] >> 5) & 1,
               (memory[i] >> 4) & 1,
               (memory[i] >> 3) & 1,
               (memory[i] >> 2) & 1,
               (memory[i] >> 1) & 1,
               memory[i] & 1);
}
