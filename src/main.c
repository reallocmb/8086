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
void intel_8086_print(Intel8086 *intel_8086, u32 memory_point_start, u32 memory_bytes_lookup);
u32 intel_8086_bytes_load_from_file(Intel8086 *intel_8086, char *file_path, u32 offset);
void bytes_binary_print(u8 *memory, u32 bytes);

int main(int argc, char **argv)
{
    if (argc <= 1)
        return 0;

    Intel8086 intel_8086;
    intel_8086_startup(&intel_8086);
    intel_8086_print(&intel_8086, 0, 50);
    u32 bytes = intel_8086_bytes_load_from_file(&intel_8086, argv[1], 0);
    intel_8086_print(&intel_8086, 0, 50);
    bytes_binary_print(intel_8086.memory, bytes);

    return 0;
}

void intel_8086_startup(Intel8086 *intel_8086)
{
    memset(intel_8086->memory, 0, INTEL_8086_MEMORY_SIZE * sizeof(*intel_8086->memory));
    memset(intel_8086->registers, 0, INTEL_8086_REGISTER_COUNT * sizeof(*intel_8086->registers));
}

void intel_8086_print(Intel8086 *intel_8086, u32 memory_point_start, u32 memory_bytes_lookup)
{
    printf("Registers:\n");
    u32 i;
    for (i = 0; i < INTEL_8086_REGISTER_COUNT; i++)
        printf("%s 0x%s%x\n",
               register_table_str[i],
               (intel_8086->registers[i] <= 0xf) ?"0" :"",
               intel_8086->registers[i]);

    printf("Memory:\n");
    for (i = memory_point_start; i < memory_bytes_lookup; i += 8)
        printf("0x%x:\t %s%x %s%x %s%x %s%x %s%x %s%x %s%x %s%x\n",
               i,
               (intel_8086->memory[i] <= 0xf) ?"0" :"", intel_8086->memory[i]  ,
               (intel_8086->memory[i + 1] <= 0xf) ?"0" :"", intel_8086->memory[i + 1],
               (intel_8086->memory[i + 2] <= 0xf) ?"0" :"", intel_8086->memory[i + 2],
               (intel_8086->memory[i + 3] <= 0xf) ?"0" :"", intel_8086->memory[i + 3],
               (intel_8086->memory[i + 4] <= 0xf) ?"0" :"", intel_8086->memory[i + 4]  ,
               (intel_8086->memory[i + 5] <= 0xf) ?"0" :"", intel_8086->memory[i + 5],
               (intel_8086->memory[i + 6] <= 0xf) ?"0" :"", intel_8086->memory[i + 6],
               (intel_8086->memory[i + 7] <= 0xf) ?"0" :"", intel_8086->memory[i + 7]);

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
