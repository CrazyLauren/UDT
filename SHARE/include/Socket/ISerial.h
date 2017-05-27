/*
 * ISerial.h
 *
 * Copyright © 2016  https://github.com/CrazyLauren
 *
 *  Created on: 12.04.2013
 *      Author:  https://github.com/CrazyLauren
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */  

#ifndef ISERIAL_H_
#define ISERIAL_H_

namespace NSHARE
{
namespace NSerial
{
typedef unsigned eBaudRate;
/*!
 * Byte sizes for the serial port.
 */
enum eBytesize
{
	DATA_NOTSET = 0, DATA_5 = 5, DATA_6 = 6, DATA_7 = 7, DATA_8 = 8
};

/*!
 * Byte sizes for the serial port.
 */
enum eParity
{
	PAR_NONE = 0, PAR_ODD = 1, PAR_EVEN = 2,
#ifdef _WIN32
	PAR_MARK = 3,
#endif
	PAR_SPACE = 4
};
/*!
 * Stopbit types.
 */
enum eStopBits
{
	STOP_NOTSET = 0, STOP_1 = 1, STOP_2 = 2,
#ifdef _WIN32
	STOP_1_5
#endif
};
/*
 * Flow control types for the serial port
 */
enum eFlowType
{
	FLOW_OFF, FLOW_HARD, FLOW_SOFT
};
struct timeouts_t
{
// The maximum time allowed to elapse before the arrival of the next byte on the communications line, in milliseconds
	NSHARE::smart_field_t<double> FIntervelTimeout;//sec
//wait after calling write
	NSHARE::smart_field_t<double> FWriteByteTimeOut;//sec
//total time-out period for read operations
	NSHARE::smart_field_t<double> FReadTimeOut;//sec
//The multiplier used to calculate the total time-out period for read operations
	NSHARE::smart_field_t<double> FReadByteTimeOut;
};

enum ePortType
{
	RS232, RS422, RS485,
};
struct port_settings_t;
class CNotation
{
public:
	virtual ~CNotation()
	{

	}
	static std::auto_ptr<CNotation> sCreateNotation(CText const& aType);
	virtual port_settings_t& MSetting(port_settings_t& aTo) const =0;
};
class C8N1: public CNotation
{
public:
	virtual port_settings_t& MSetting(port_settings_t& aTo) const;
};
struct SHARE_EXPORT port_settings_t
{
	static const CText BAUND_RATE;
	static const CText NOTATION;

	unsigned FBaudRate;
	eBytesize FByteSize;
	eParity FParity;
	eStopBits FStopBits;
	eFlowType FFlowControl;
	timeouts_t FTimeout;
	port_settings_t(NSHARE::CConfig const& aConf)
	{
		FByteSize=DATA_NOTSET;
		FParity=PAR_NONE;
		FStopBits=STOP_NOTSET;
		FBaudRate = 0;
		FFlowControl = FLOW_OFF;

		aConf.MGetIfSet(BAUND_RATE,FBaudRate);
		CText _name;
		if(aConf.MGetIfSet(NOTATION,_name))
		{
			std::auto_ptr<CNotation> _not(CNotation::sCreateNotation(_name));
			if(_not.get())
				_not->MSetting(*this);
		}
	}
	port_settings_t(const CNotation& aNot = C8N1(), const eBaudRate& aSpeed =
			9600)
	{
		FByteSize=DATA_NOTSET;
		FParity=PAR_NONE;
		FStopBits=STOP_NOTSET;
		aNot.MSetting(*this);
		FBaudRate = aSpeed;
		FFlowControl = FLOW_OFF;
	}
	port_settings_t(eBaudRate const& aBR, eBytesize const& aBS,
			eParity const& aP, eStopBits const& aS, eFlowType const& aFT,
			timeouts_t const& aT) :
			FBaudRate(aBR), FByteSize(aBS), FParity(aP), FStopBits(aS), FFlowControl(
					aFT), FTimeout(aT)
	{

	}
	inline bool MIsValid() const
	{
		return FBaudRate != 0 && FByteSize != DATA_NOTSET
				&& FStopBits != STOP_NOTSET;
	}
	CConfig MSerialize() const
	{
		CConfig _conf;
		if (MIsValid())
		{
			_conf.MSet(BAUND_RATE,FBaudRate);
			//todo
		}
		return _conf;
	}
};
inline port_settings_t& C8N1::MSetting(port_settings_t& aTo) const
{
	aTo.FParity = PAR_NONE;
	aTo.FStopBits = STOP_1;
	aTo.FByteSize = DATA_8;
	return aTo;
}

class C7E1: public CNotation
{
public:
	virtual port_settings_t& MSetting(port_settings_t& aTo) const
	{
		aTo.FParity = PAR_EVEN;
		aTo.FStopBits = STOP_1;
		aTo.FByteSize = DATA_7;
		return aTo;
	}
};
class C7O1: public CNotation
{
public:
	virtual port_settings_t& MSetting(port_settings_t& aTo) const
	{
		aTo.FParity = PAR_ODD;
		aTo.FStopBits = STOP_1;
		aTo.FByteSize = DATA_7;
		return aTo;
	}
};
#ifdef _WIN32
class C7M1: public CNotation
{
public:
	virtual port_settings_t& MSetting(port_settings_t& aTo) const
	{
		aTo.FParity = PAR_MARK;
		aTo.FStopBits = STOP_1;
		aTo.FByteSize = DATA_7;
		return aTo;
	}

};
#endif
class C7S1: public CNotation
{
public:
	virtual port_settings_t& MSetting(port_settings_t& aTo) const
	{
		aTo.FParity = PAR_SPACE;
		aTo.FStopBits = STOP_1;
		aTo.FByteSize = DATA_7;
		return aTo;
	}
};
class SHARE_EXPORT  ISerial: public  ISocket
{
public:
	static const CText PATH;
	enum eFlush
	{
		FLUSH_IN = 0x1 << 0, FLUSH_OUT = 0x1 << 1,
	};
	enum eLine
	{
		DTR, RTS, CTS, DSR, RI, CD, BREAK
	};
	virtual ~ISerial()
	{
		;
	}

	virtual bool MOpen(CText const& aNamePort)=0;
	virtual CText  MGetPort() const=0;
	//virtual size_t MAvailable()=0; //FIXME to Socket

	virtual void MSetUP(port_settings_t const& aType)=0;
	virtual port_settings_t const& MGetSetting() const=0;
	virtual NSHARE::CConfig MSettings(void) const
	{
		return MGetSetting().MSerialize();
	}

	virtual void MSetBaudRate(eBaudRate const& aBaund)=0;
	virtual eBaudRate MGetBaudRate() const=0;

	virtual void MFlush(eFlush const&)=0; //FIXME to Socket

	virtual bool MSignal(eLine const&, bool aVal)=0;
	virtual bool MLineState(eLine const&) const=0;

	virtual void MSendBreak(int duration)=0;

	virtual bool MWaitForLineChanged()=0;
};
class IRS485: public virtual ISerial
{
public:
	virtual ~IRS485()
	{
		;
	}
};
inline std::auto_ptr<CNotation> CNotation::sCreateNotation(CText const& aType)
{
	CNotation *_ptr=NULL;
	if (aType == "8N1")
		_ptr= new C8N1();
	else if (aType == "7E1")
		_ptr= new C7E1();
	else if (aType == "7O1")
		_ptr=new C7O1();
#ifdef _WIN32
	else if (aType == "7M1")
		_ptr=new C7M1();
#endif
	else if (aType == "7S1")
		_ptr= new  C7S1();
	else
		LOG(FATAL)<<"Unknown serial port type:"<<aType;
	return std::auto_ptr<CNotation>(_ptr);
};

} //namespace NSerial
} //namespace USHARE
namespace std
{
inline std::ostream& operator <<(std::ostream& aStream,
		const NSHARE::NSerial::eFlowType& aFlow)
{
	using namespace NSHARE::NSerial;
	switch (aFlow)
	{
	case FLOW_OFF:
		aStream << "None";
		break;

	case FLOW_HARD:
		aStream << "Hard";
		break;
	case FLOW_SOFT:
		aStream << "Soft";
		break;
	}

	return aStream;
}

inline std::ostream& operator <<(std::ostream& aStream,
		const NSHARE::NSerial::eParity& aParity)
{
	using namespace NSHARE::NSerial;

	switch (aParity)
	{
	case PAR_NONE:
		aStream << "None";
		break;
	case PAR_ODD:
		aStream << "Odd";
		break;

	case PAR_EVEN:
		aStream << "Even";
		break;
#ifdef _WIN32
	case PAR_MARK:
		aStream << "Mark";
		break;
#endif
	case PAR_SPACE:
		aStream << "Space";
		break;
	}

	return aStream;
}

inline std::ostream& operator <<(std::ostream& aStream,
		const NSHARE::NSerial::port_settings_t& aSetting)
{
	aStream << "B" << static_cast<unsigned>(aSetting.FBaudRate) << "; S="
			<< static_cast<unsigned>(aSetting.FByteSize) << "; P="
			<< aSetting.FParity << "; Stop="
			<< static_cast<unsigned>(aSetting.FStopBits) << "; F="
			<< aSetting.FFlowControl;
	return aStream;
}
inline std::ostream& operator <<(std::ostream& aStream,
		const NSHARE::NSerial::ISerial::eLine& aLine)
{
	using namespace NSHARE::NSerial;

	switch (aLine)
	{
	case ISerial::DTR:
		aStream << "DTR";
		break;
	case ISerial::RTS:
		aStream << "RTS";
		break;
	case ISerial::CTS:
		aStream << "CTS";
		break;
	case ISerial::DSR:
			aStream << "DSR";
			break;
	case ISerial::RI:
			aStream << "RI";
			break;
	case ISerial::CD:
			aStream << "CD";
			break;
	case ISerial::BREAK:
		aStream << "BREAK";
		break;
	}

	return aStream;
}

}
#endif /* ISERIAL_H_ */
