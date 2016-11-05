/*
 * CPFile.h
 *
 * Copyright © 2016 Sergey Cherepanov (sergey0311@gmail.com)
 *
 *  Created on: 05.04.2013
 *      Author: Sergey Cherepanov (https://github.com/CrazyLauren)
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */ 
#ifndef CPFILE_H_
#define CPFILE_H_

namespace NSHARE
{
class  CPFile
{
public:
	enum Type
	{
		READ = 1 << 0, //"r"
		WRITE = 1 << 1, //"w"
		APPEND = 1 << 2, //"a"
		UPDATE = 1 << 3, //"+"
		BINARY = 1 << 4, //"b"
	};

	CPFile(const char* aName, const unsigned & aType) ;
	bool MOpen() throw();
	bool MIsOpen(void)  const;
	void MClose(void) throw();
	operator FILE *(void) throw();
	operator FILE const *(void) const ;

	String const FName;
private:
	FILE *FPf;
	char FType[5];
	void MGetType(char*, const unsigned&);
};
inline CPFile::CPFile(const char* aName, const unsigned & aType):
		FName(aName), FPf(NULL)
{
	MGetType(FType, aType);
}
inline bool CPFile::MOpen() throw()
{
	FPf = fopen(FName.c_str(), FType);
	return MIsOpen();
}
inline void CPFile::MGetType(char* aTo, const unsigned& aType)
{
	CFlags<unsigned> _flags(aType);
	if (_flags == READ)
		*aTo++ = 'r';
	else if (_flags == WRITE)
		*aTo++ = 'w';
	else if (_flags == APPEND)
		*aTo++ = 'a';
	else
		throw std::invalid_argument("");

	if (_flags == UPDATE)
		*aTo++ = '+';

	if (_flags == BINARY)
		*aTo++ = '+';
	*aTo = '\0';
}
inline bool CPFile::MIsOpen() const
{
	return FPf != NULL;
}
inline void CPFile::MClose() throw()
{
	if (MIsOpen())
	{
		fclose(FPf);
		FPf = NULL;
	}
}
inline CPFile::operator FILE *() throw()
{
	return FPf;
}
inline CPFile::operator const FILE *() const
{
	return FPf;
}
template<> class CRAII<CPFile> : public CDenyCopying
{
public:
	explicit CRAII(CPFile & aPFile) :
			FPFile(aPFile)
	{
		FPFile.MOpen();
	}
	~CRAII(void)
	{
		FPFile.MClose();
	}
	operator CPFile &()
	{
		return FPFile;
	}
private:
	CPFile &FPFile;
};
}//namespace USHARE

#endif /* CPFILE_H_ */
