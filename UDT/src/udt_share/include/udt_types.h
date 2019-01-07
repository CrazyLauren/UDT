/*
 * udt_types.h
 *
 *  Created on: 03.02.2016
 *      Author:  https://github.com/CrazyLauren
 *
 *	Copyright © 2016  https://github.com/CrazyLauren
 *
 *	Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 *	https://www.mozilla.org/en-US/MPL/2.0)
 */
#ifndef UDT_TYPES_H_
#define UDT_TYPES_H_

namespace NUDT
{
struct required_header_t
{
	required_header_t(); //cleanuping to avoid parser error
	NSHARE::version_t FVersion; //Required msg version

	union //using for  header msg specification
	//
	//The fixed size (8 byte) "union"  is be used to avoid
	//frequently "required_header_t" structure changing.
	{
		struct //header style of
		{
			uint32_t FNumber;
		};
		uint8_t FReserved[8];
	};

#ifdef SHARE_CONFIG_DEFINED
	required_header_t(NSHARE::CConfig const& aConf):
		FVersion(aConf.MChild("ver")) //
	{
		for(unsigned i=0;i<sizeof(FReserved);++i)
			FReserved[i]=0x0;

		if(aConf.MHasValue("buf"))
		{
			aConf.MChild("buf").MValueBuf(sizeof(FReserved),FReserved);
		}
	}
	NSHARE::CConfig MSerialize() const
	{
		NSHARE::CConfig _conf("rh");
		_conf.MAdd("buf",FReserved,sizeof(FReserved));
		_conf.MAdd(FVersion.MSerialize());
		return _conf;
	}
	bool MIsValid()const
	{
		return true;
	}
#endif
};
inline required_header_t::required_header_t()
{
	for(unsigned i=0;i<sizeof(FReserved);++i)
		FReserved[i]=0x0;
}
;
struct CReqHeaderFastLessCompare
{
	bool operator()(const required_header_t& a,
			const required_header_t& b) const
	{
		return (memcmp(a.FReserved, b.FReserved, sizeof(a.FReserved)) > 0);
	}
};
template<class T>
inline std::ostream& print_buffer(std::ostream& aStream, T aBegin, T aEnd)
{
	int _i = 0;
	for (; aBegin != aEnd; ++aBegin)
	{
		aStream << *aBegin << " ";
		if (!(++_i % 8))
			aStream << "\n";
	}
	return aStream;
}
}
namespace std
{
inline std::ostream& operator<<(std::ostream & aStream,
		NUDT::required_header_t const& aVal)
{

	aStream << "Version:" << aVal.FVersion << std::endl;
	aStream << "Number: ";
	int _i = 0;

	aStream.setf(ios::hex, ios::basefield);
	uint8_t* aBegin=(uint8_t*)aVal.FReserved;
	uint8_t* aEnd=aBegin+ sizeof(aVal.FReserved);
	for (; aBegin != aEnd; ++aBegin)
	{
		aStream << "0x" << static_cast<unsigned const&>(*aBegin) << " ";
		if (!(++_i % 8))
			aStream << "\n";
	}
	aStream.unsetf(ios::hex);
	return aStream;
}
}
#endif /* UDT_TYPES_H_ */
