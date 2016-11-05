/*
 * CFile.cpp
 *
 * Copyright © 2016 Sergey Cherepanov (sergey0311@gmail.com)
 *
 *  Created on: 06.04.2013
 *      Author: Sergey Cherepanov (https://github.com/CrazyLauren)
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */  
#include <deftype>
#include <stdio.h>
#include <string.h>
#include <UType/CFile.h>
namespace NSHARE
{

CFile::CFile(const char* aName, const unsigned & aType) :
		FPFile(aName, aType)
{
}
CFile::CFile(const CPFile& aPFile) :
		FPFile(aPFile)
{

}
CFile::~CFile()
{
	FPFile.MClose();
}
bool CFile::MOpen()
{
	return FPFile.MOpen();
}
bool CFile::MIsOpen(void) const
{
	return FPFile.MIsOpen();
}
void CFile::MClose(void)
{
	return FPFile.MClose();
}

String const& CFile::MGetFileName() const
{
	return FPFile.FName;
}
CFile::size_t CFile::MWrite(const void* aBuf, size_t aCount)
{
	size_t _revl = fwrite(aBuf, 1, aCount, FPFile);
	fflush(FPFile);
	return _revl;
}
size_t CFile::MWrite(const char* aStr)
{
	return MWrite(aStr,strlen(aStr));
}
CFile::size_t CFile::MGetLength()
{
	pos_t const _pos = MGetPosition();
	MSeek(0, END);
	pos_t const _revl = MGetPosition();
	MSeek(_pos, SET);
	return _revl;
}
CFile::pos_t CFile::MGetPosition()
{
	return ftell(FPFile);
}
bool CFile::MSeek(pos_t const, TypeSeek aFlag)
{
	CFlags<TypeSeek> _flags(aFlag);

	int _type = -1;
	if (_flags == SET)
		_type = SEEK_SET;
	else if (_flags == CUR)
		_type = SEEK_CUR;
	else if (_flags == END)
		_type = SEEK_END;
	return !fseek(FPFile, 9, _type);
}
}//namespace NSHARE
