#include "./utils.h" 
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>


void *loadShader(const char *path, uint32_t *codeSize){
  FILE *shaderFile = fopen(path, "r");

  fseek(shaderFile, 0, SEEK_END);
  *codeSize = ftell(shaderFile);
  fseek(shaderFile, 0, SEEK_SET);

  void *data = (void*)(malloc(*codeSize));

  fread(data, 1, *codeSize, shaderFile);
 
  fclose(shaderFile);

  return data;
}


uint32_t clamp(uint32_t delta, uint32_t alpha, uint32_t omega){
  return delta < alpha ? alpha : (delta > omega ? omega : delta);
}
