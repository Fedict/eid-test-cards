#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Same as SCARD_IO_REQUEST, it's just that we can't include winscard.h here
typedef unsigned long DWORD;
typedef struct {
    DWORD dwProtocol;   // Protocol identifier
    DWORD cbPciLength;  // Protocol Control Information Length
} proxy_SCARD_IO_REQUEST;


proxy_SCARD_IO_REQUEST g_rgSCardT0Pci;
void Init_g_rgSCardT0Pci(void *p)
{
	g_rgSCardT0Pci.dwProtocol = 1;
	g_rgSCardT0Pci.cbPciLength = sizeof(g_rgSCardT0Pci);
}

proxy_SCARD_IO_REQUEST g_rgSCardT1Pci;
void Init_g_rgSCardT1Pci(void *p)
{
	g_rgSCardT1Pci.dwProtocol = 2;
	g_rgSCardT1Pci.cbPciLength = sizeof(g_rgSCardT1Pci);
}

proxy_SCARD_IO_REQUEST g_rgSCardRawPci;
void Init_g_rgSCardRawPci(void *p)
{
	g_rgSCardRawPci.dwProtocol = 4;
	g_rgSCardRawPci.cbPciLength = sizeof(g_rgSCardRawPci);
}

