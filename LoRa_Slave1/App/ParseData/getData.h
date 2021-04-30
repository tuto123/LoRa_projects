#ifndef __GETVALUE_H__
#define __GETVALUE_H__

#include "stdlib.h"
#include "stdio.h"

#include "Temperature.h"
#include "fan.h"

#define DEVICE_ID       1
#define TERMINAL_ID     1
#define MASK_NUM        63
#define SPLIT_CHAR      "_"
#define StrCat(str1, str2) str1##str2

void GenerateTask();
void GenerateData(char *generateData);

#endif //__GETVALUE_H__

