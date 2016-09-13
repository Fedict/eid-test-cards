#pragma once

#include <stdlib.h>
#include <stdio.h>

#define MAX_CONTENT    8196
#define MAX_ELEMENT    1024

namespace EidInfra
{
class Xml
{
public:
Xml(void);
virtual ~Xml(void);
bool GetNextElement(FILE * file, char *element, size_t elementMaxLength, char *content, size_t maxLength) const;
bool FindElement(FILE * file, const char *element, char *content, size_t maxLength) const;
bool seekToContentElement(FILE * file, const char *element, const char *content) const;
};
}
