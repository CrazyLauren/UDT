/*
 * CSerialPort.h
 *
 * Copyright © 2016  https://github.com/CrazyLauren
 *
 *  Created on: 22.03.2013
 *      Author:  https://github.com/CrazyLauren
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */
#ifndef CSERIALPORT_H_
#define CSERIALPORT_H_

#ifndef SERIAL_SOCKET_EXIST
#	define SERIAL_SOCKET_EXIST
#endif 

namespace NSHARE
{
namespace NSerial
{
class SHARE_EXPORT CSerialPort: public  ISerial,NSHARE::CDenyCopying
{
public:
	static const NSHARE::CText NAME;
	CSerialPort(NSHARE::CConfig const& aConf);
	CSerialPort(CText const & aPort = "", port_settings_t const& aSetting =
			port_settings_t());
	virtual ~CSerialPort();

	virtual void MSetUP(port_settings_t const& aType);

	virtual port_settings_t const& MGetSetting() const;

	virtual bool MOpen(CText const& aNamePort);

	virtual bool MReOpen();

	virtual bool MIsOpen() const;

	virtual void MClose();

	virtual size_t MAvailable() const;

	int MWaitData(float const aTime) const;

	virtual ssize_t MReceiveData(data_t *, float const aTime);

	ssize_t MReceiveData(data_t *,std::size_t aCount,float const aTime);

	virtual sent_state_t MSend(void const*  aData, std::size_t);

	virtual CText  MGetPort() const;

	const CSocket&  MGetSocket() const;

	void  MSetTimeout (timeouts_t &timeout);

	virtual void MSetBaudRate(eBaudRate const& aBaund);
	virtual eBaudRate MGetBaudRate() const;

	virtual void MFlush (eFlush const&);//FIXME to Socket

	virtual void MSendBreak (int );

	virtual bool MSignal(eLine const&, bool aVal);

	virtual bool MLineState(eLine const&) const;

	virtual bool  MWaitForLineChanged ();

	virtual std::ostream & MPrint(std::ostream & aStream) const;
	NSHARE::CConfig MSerialize() const;
protected:
	  class CImpl;
private:
	  CImpl *FImpl;
	  port_settings_t FSetting;
	  diagnostic_io_t FDiagnostic;
};
} /* namespace ISerialPort */
} //namespace USHARE

#endif /* CSERIALPORT_H_ */
