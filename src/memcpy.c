//
// Created by Administrator on 2019/3/22.
//

#include <string.h>
#include "glibcVersion.h"

void *__memcpy_old(void *, const void *, size_t);

asm(".symver __memcpy_old, memcpy@" GLIBC_VER);
void *__wrap_memcpy(void *dest, const void *src, size_t n)
{
    return __memcpy_old(dest, src, n);
}