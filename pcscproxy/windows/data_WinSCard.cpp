/* ****************************************************************************

* eID Middleware Project.
* Copyright (C) 2016 FedICT.
*
* This is free software; you can redistribute it and/or modify it
* under the terms of the GNU Lesser General Public License version
* 3.0 as published by the Free Software Foundation.
*
* This software is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
* Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public
* License along with this software; if not, see
* http://www.gnu.org/licenses/.

**************************************************************************** */
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

