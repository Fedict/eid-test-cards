///// WARNING: this code is correct only if only pointers are exported! /////

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void *g_rgSCardT0Pci = NULL;
void Init_g_rgSCardT0Pci(void *p)
{
	g_rgSCardT0Pci = p;
}

void *g_rgSCardT1Pci = NULL;
void Init_g_rgSCardT1Pci(void *p)
{
	g_rgSCardT1Pci = p;
}

void *g_rgSCardRawPci = NULL;
void Init_g_rgSCardRawPci(void *p)
{
	g_rgSCardRawPci = p;
}

