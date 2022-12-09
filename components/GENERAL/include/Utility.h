#pragma once

#include "Common.h"

#include<string>



void ReplaceAll( std::string &pString, const std::string &pSearch, const std::string &pReplace);
std::string GetCurrentIsoTimeString();
bool HexStringToBytes(const std::string &pString, uint8_t **pNewBuffer, uint16_t *pLength);