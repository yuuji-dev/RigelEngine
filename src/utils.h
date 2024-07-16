#pragma once

#include <stdint.h>


void *loadShader(const char *path, uint32_t *codeSize);
uint32_t clamp(uint32_t delta, uint32_t alpha, uint32_t omega);
