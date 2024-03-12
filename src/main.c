#include<stdio.h>
#include<stdint.h>

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
void bytes_binary_print(u8 *memory, u32 bytes);

int main(int argc, char **argv)
{
    if (argc <= 1)
        return 0;

    Intel8086 intel_8086;
    u32 bytes = intel_8086_bytes_load_from_file(&intel_8086, argv[1], 0);
    bytes_binary_print(intel_8086.memory, bytes);

    return 0;
}

u32 intel_8086_bytes_load_from_file(Intel8086 *intel_8086, char *file_path, u32 offset)
{
    FILE *f = fopen(file_path, "rb");
    u32 bytes = 0;
    u32 bytes_read = 0;
    while ((bytes_read = fread(intel_8086->memory + bytes, 1, 2048, f)) != 0)
        bytes += bytes_read;

    return bytes;
}

void bytes_binary_print(u8 *memory, u32 bytes)
{
    printf("Bytes: %u\n", bytes);
    u32 i;
    for (i = 0; i < bytes; i++)
        printf("%u%u%u%u%u%u%u%u\n",
               (memory[i] >> 7) & 1,
               (memory[i] >> 6) & 1,
               (memory[i] >> 5) & 1,
               (memory[i] >> 4) & 1,
               (memory[i] >> 3) & 1,
               (memory[i] >> 2) & 1,
               (memory[i] >> 1) & 1,
               memory[i] & 1);
}
