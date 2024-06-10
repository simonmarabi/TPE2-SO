#pragma once
#include <stdint.h>
#include <stddef.h>

uint32_t GetUint();
uint32_t GetUniform(uint32_t max);
uint8_t memcheck(void *start, uint8_t value, uint32_t size);
void memset_2(void *start, uint8_t value, uint32_t size);

