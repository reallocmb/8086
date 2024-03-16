#include<stdio.h>
#include<stdint.h>
#include<stdlib.h>
#include<string.h>

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;

#define INTEL_8086_MEMORY_SIZE 1048576 /* 1024 * 1024 */
#define INTEL_8086_REGISTER_COUNT 8

typedef struct Intel8086 {
    u8 memory[INTEL_8086_MEMORY_SIZE];
    u16 registers[INTEL_8086_REGISTER_COUNT];
} Intel8086;

typedef enum {
    REGISTER_A,
    REGISTER_C,
    REGISTER_D,
    REGISTER_B,
    REGISTER_SP,
    REGISTER_BP,
    REGISTER_SI,
    REGISTER_DI,
} Register;

typedef enum {
    OP_CODE_MOV,
    OP_CODE_ADD,
} OpCode;

typedef struct Operand {
    Register reg;
} Operand;

typedef enum {
    BITS_IDENTIFY,
    BITS_W,
    BITS_D,
    BITS_MOD,
    BITS_REG,
    BITS_RM,
    BITS_END,
} Bits;

typedef struct BitsField {
    Bits type;
    u8 count;
    u8 value;
} BitsField;

typedef struct InstructionFormat {
    BitsField bits[7];
} InstructionFormat;

InstructionFormat instruction_formats[] = {
    {
        {
            { BITS_IDENTIFY, 6, 0b100010 }, /* register/memory to/from register */
            { BITS_D, 1 },
            { BITS_W, 1 },
            { BITS_MOD, 5 },
            { BITS_REG, 5 },
            { BITS_RM, 5 },
            { BITS_END },
        }
    },
    {
        {
            { BITS_IDENTIFY, 4, 0b1011 }, /* immediate to register */
            { BITS_D, 1 },
            { BITS_W, 1 },
            { BITS_MOD, 5 },
            { BITS_REG, 5 },
            { BITS_RM, 5 },
            { BITS_END },
        }
    }
};

typedef struct Instruction {
    OpCode op_code;
    Operand operands[2];
} Instruction;

const char register_table_str[INTEL_8086_REGISTER_COUNT][3] = {
    "ax",
    "cx",
    "dx",
    "bx",
    "sp",
    "bp",
    "si",
    "di"
};

void intel_8086_startup(Intel8086 *intel_8086);
void intel_8086_memory_print(u8 *memory, u32 lookup_start, u32 lookup_bytes);
void intel_8086_registers_print(u16 *registers);
u32 intel_8086_bytes_load_from_file(Intel8086 *intel_8086, char *file_path, u32 offset);
void intel_8086_instruction_decode(Intel8086 *intel_8086, u32 bytes, u32 process_start);
void bytes_binary_print(u8 *memory, u32 bytes);

int main(int argc, char **argv)
{
    if (argc <= 1)
        return 0;

    Intel8086 intel_8086;
    intel_8086_startup(&intel_8086);

#if 0
    intel_8086_memory_print(intel_8086.memory, 0, 50);
    intel_8086_registers_print(intel_8086.registers);
#endif

    u32 bytes = intel_8086_bytes_load_from_file(&intel_8086, argv[1], 0);
    intel_8086_instruction_decode(&intel_8086, bytes, 0);

#if 0
    intel_8086_memory_print(intel_8086.memory, 0, 50);
    intel_8086_registers_print(intel_8086.registers);
#endif

    return 0;
}

void intel_8086_startup(Intel8086 *intel_8086)
{
    memset(intel_8086->memory, 0, INTEL_8086_MEMORY_SIZE * sizeof(*intel_8086->memory));
    memset(intel_8086->registers, 0, INTEL_8086_REGISTER_COUNT * sizeof(*intel_8086->registers));
}

void intel_8086_registers_print(u16 *registers)
{
    printf("Registers:\n");
    u8 i;
    for (i = 0; i < INTEL_8086_REGISTER_COUNT; i++)
        printf("%s 0x%s%x\n",
               register_table_str[i],
               (registers[i] <= 0xf) ?"0" :"",
               registers[i]);
}

void intel_8086_memory_print(u8 *memory, u32 lookup_start, u32 lookup_bytes)
{
    printf("Memory:\n");
    u32 i;
    for (i = lookup_start; i < lookup_bytes; i += 8)
    {
        printf("0x%x:\t%s%x       %s%x       %s%x       %s%x       %s%x       %s%x       %s%x       %s%x\n",
               i,
               (memory[i] <= 0xf) ?"0" :"", memory[i]  ,
               (memory[i + 1] <= 0xf) ?"0" :"", memory[i + 1],
               (memory[i + 2] <= 0xf) ?"0" :"", memory[i + 2],
               (memory[i + 3] <= 0xf) ?"0" :"", memory[i + 3],
               (memory[i + 4] <= 0xf) ?"0" :"", memory[i + 4]  ,
               (memory[i + 5] <= 0xf) ?"0" :"", memory[i + 5],
               (memory[i + 6] <= 0xf) ?"0" :"", memory[i + 6],
               (memory[i + 7] <= 0xf) ?"0" :"", memory[i + 7]);
        putchar('\t');
        bytes_binary_print(&memory[i], 8);
        putchar('\n');
    }
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

void intel_8086_instruction_decode(Intel8086 *intel_8086, u32 bytes, u32 process_start)
{
    u8 i;
    u8 *memory = intel_8086->memory + process_start;
    printf("bytes:%d\n", bytes);
    while (bytes--)
    {
        u8 byte_offset = 0;
        u8 byte = *memory;
        for (i = 0; i < 2; i++)
        {
            if (instruction_formats[i].bits[0].value == bits)
            {
            }
        }
        printf("round\n");
        bytes_binary_print(memory, 1);
        memory++;
    }
    printf("end");
}

void bytes_binary_print(u8 *memory, u32 bytes)
{
    u32 i;
    for (i = 0; i < bytes; i++)
        printf("%u%u%u%u%u%u%u%u ",
               (memory[i] >> 7) & 1,
               (memory[i] >> 6) & 1,
               (memory[i] >> 5) & 1,
               (memory[i] >> 4) & 1,
               (memory[i] >> 3) & 1,
               (memory[i] >> 2) & 1,
               (memory[i] >> 1) & 1,
               memory[i] & 1);
}
