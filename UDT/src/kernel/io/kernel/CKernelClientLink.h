/*
 * CKernelClientLink.h
 *
 * Copyright © 2016 Sergey Cherepanov (sergey0311@gmail.com)
 *
 *  Created on: 29.07.2016
 *      Author: Sergey Cherepanov (https://github.com/CrazyLauren)
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */
#ifndef CKERNELCLIENTLINK_H_
#define CKERNELCLIENTLINK_H_


namespace NUDT
{
class IMainChannel;
class CKernelIOByTCP;

class CKernelClientLink: public ILink,NSHARE::CDenyCopying
{
public:
	static NSHARE::CText const DEFAULT;
	static const NSHARE::CText NAME;
	static const NSHARE::CText MAIN_CHANNEL_TYPE;

	CKernelClientLink(descriptor_t aFD, uint64_t FTime,
			ILinkBridge*,programm_id_t const & aKernel);
	CKernelClientLink(CKernelClientLink const & aRht);
	virtual ~CKernelClientLink();

	bool MSend(const data_t& aVal);
	bool MSend(const programm_id_t& aVal, const routing_t& aRoute,error_info_t const&);
	bool MSend(const kernel_infos_array_t& aVal, const routing_t& aRoute,error_info_t const&);
	bool MSend(const user_data_t& aVal);
	bool MSend(const fail_send_t& aVal, const routing_t& aRoute,error_info_t const&);
	bool MSend(const demand_dgs_for_t& aVal, const routing_t& aRoute,error_info_t const&);


	void MReceivedData(data_t::const_iterator aBegin,
			data_t::const_iterator aEnd);

	void MReceivedData(user_data_t const&);
	void MReceivedData(programm_id_t const&, const routing_t& aRoute,error_info_t const& aError);
	void MReceivedData(demand_dgs_t const&, const routing_t& aRoute,error_info_t const&);
	void MReceivedData(demand_dgs_for_t const&, const routing_t& aRoute,error_info_t const&);
	void MReceivedData(kernel_infos_array_t const&, const routing_t& aRoute,error_info_t const&);
	void MReceivedData(fail_send_t const&, const routing_t& aFrom,error_info_t const&);

	void MCloseRequest();
	void MClose();

	descriptor_t MGetID() const;
	bool MIsOpened() const;
	bool MAccept();
	NSHARE::CConfig MSerialize() const;
	NSHARE::CConfig const& MBufSettingFor(NSHARE::CConfig const& aFrom) const;
	class CConnectionHandler;
private:
	typedef CInParser<CKernelClientLink> parser_t;

	enum eState //don't use pattern
	//as it's  difficult for the class
	{
		E_NOT_OPEN, //
		E_OPEN, //
		E_ERROR,//
	};
	template<class T>
	inline unsigned MFill(data_t* _buf, const T& _id, const routing_t& aRoute,error_info_t const&);



	template<class DG_T>
	void MProcess(DG_T const* aP, parser_t*);

	int MOpenMainChannel(NSHARE::CText const& aType);
	int MCloseMain();
	int MSendMainChannelError(NSHARE::CText const& _channel_type, unsigned aError);
	bool MSendService(const data_t& aVal);
	bool MSendProtocolType();
	bool MSendIDInfo();
	bool MOpen() ;
	bool MOpening(NSHARE::CBuffer::const_iterator aBegin,
			NSHARE::CBuffer::const_iterator aEnd);


	CKernelClientLink& operator=(CKernelClientLink const & aRht)
	{
		return *this;
	}
	void MChangeState(eState aNew);

	SHARED_PTR<user_data_t> FpUserDataFor;
	SHARED_PTR<fail_send_t> FpFailSent;
	SHARED_PTR<kernel_infos_array_t> FpKernelInfo;
	SHARED_PTR<demand_dgs_for_t> FpDemandsDgFor;
	SHARED_PTR<demand_dgs_t> FpDemands;



	parser_t FServiceParser;

	smart_bridge_t FBridge;
	descriptor_t  Fd;

	eState FState;
	IMainChannel* FMainChannel;
	programm_id_t const FKernel;
	CLinkDiagnostic FDiagnostic;

	friend class CInParser<CKernelClientLink> ;
	//mingw bug. It does not like the typedef
};
inline descriptor_t CKernelClientLink::MGetID() const
{
	return Fd;
}
} /* namespace NUDT */
#endif /* CKERNELCLIENTLINK_H_ */