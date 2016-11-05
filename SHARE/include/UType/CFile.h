/*
 * CFile.h
 *
 * Copyright © 2016 Sergey Cherepanov (sergey0311@gmail.com)
 *
 *  Created on: 06.04.2013
 *      Author: Sergey Cherepanov (https://github.com/CrazyLauren)
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */ 
#ifndef CFILE_H_
#define CFILE_H_

class CDenyCopying;
namespace NSHARE
{
class SHARE_EXPORT CFile
{
public:
	typedef std::size_t size_t;
	typedef int pos_t;
	enum TypeSeek
	{
		SET = 1 << 0, //Beginning of file
		CUR = 1 << 1, //Current position of the file pointer
		END = 1 << 2, //End of file *
	};
	typedef CPFile::Type Type;

	CFile(const char* aName, const unsigned & aType);//Type
	explicit CFile(const CPFile& aFile);
	~CFile();
	bool MOpen();
	bool MIsOpen(void) const;
	void MClose(void);
	String const& MGetFileName() const;
//	void MGetFilePath(String *aTo) const; //TODO
	size_t MGetLength();

	pos_t MGetPosition();
//	size_t MRead(void* aBuf, size_t aCount) const; //TODO
//	bool MRemove(); //TODO
//	bool MRename(String const&); //TODO
	bool MSeek(pos_t const, TypeSeek aFlag);
//	bool SetFilePath(size_t const&);//TODO
	size_t MWrite(const void* aBuf, size_t aCount);
	size_t MWrite(const char* aStr);
private:
	CPFile FPFile;
};
template<> class CRAII<CFile> : public CDenyCopying
{
public:
	explicit CRAII(CFile & aPFile) :
			FFile(aPFile)
	{
		FFile.MOpen();
	}
	~CRAII(void)
	{
		FFile.MClose();
	}
	operator CFile &()
	{
		return FFile;
	}
private:
	CFile &FFile;
};
}//namespace USHARE

#endif /* CFILE_H_ */
