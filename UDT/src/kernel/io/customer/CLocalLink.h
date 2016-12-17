/*
 * CLocalLink.h
 *
 * Copyright © 2016 Sergey Cherepanov (sergey0311@gmail.com)
 *
 *  Created on: 04.02.2016
 *      Author: Sergey Cherepanov (https://github.com/CrazyLauren)
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */
#ifndef CLINK_H_
#define CLINK_H_

namespace NUDT
{
class IMainChannel;
class CKernelIOByTCP;

class CLocalLink: public ILink,NSHARE::CDenyCopying
{
public:
	static NSHARE::CText const MAIN_CHANNEL_TYPE;
	static NSHARE::CText const DEFAULT;
	static NSHARE::CText const DEFAULT_MAIN;
	static const NSHARE::CText NAME;
	CLocalLink(descriptor_t aFD, uint64_t FTime, ILinkBridge*,
			program_id_t const & aCust);
	virtual ~CLocalLink();

	bool MSend(const program_id_t& aVal, const routing_t& aRoute,error_info_t const&);
	bool MSend(const kernel_infos_array_t& aVal, const routing_t& aRoute,error_info_t const&);
	bool MSend(const user_data_t& aVal);
	bool MSend(const data_t& aVal);
	bool MSend(const fail_send_t& aVal, const routing_t& aRoute,error_info_t const&);
	bool MSend(const demand_dgs_for_t& aVal, const routing_t& aRoute,error_info_t const&);


	void MReceivedData(data_t::const_iterator aBegin,
			data_t::const_iterator aEnd);

	void MReceivedData(user_data_t const&);
	void MReceivedData(program_id_t const&, const routing_t&,error_info_t const&);
	void MReceivedData(demand_dgs_t const&, const routing_t&,error_info_t const&);
	void MReceivedData(demand_dgs_for_t const&, const routing_t&,error_info_t const&);

	void MCloseRequest();
	void MClose();

	descriptor_t MGetID() const;
	bool MIsOpened() const;
	bool MAccept();

	class CConnectionHandler;
	NSHARE::CConfig MSerialize() const;
	NSHARE::CConfig const& MBufSettingFor(NSHARE::CConfig const& aFrom) const;
private:
	typedef CInParser<CLocalLink> parser_t;


	enum eState //don't use pattern
	//as it's  difficult for the class
	{
		E_NOT_OPEN, //
		E_OPEN, //
		E_ERROR,//
	};
	template<class T>
	inline unsigned MFill(data_t* _buf, const T& _id, const routing_t& aRoute,error_info_t const& aError);


	template<class DG_T>
	void MProcess(DG_T const* aP, parser_t*);

	NSHARE::CText MGetMainChannelType(bool aDefOnly=false);
	bool MCreateMainChannel();
	int MCloseMain();
	int MSendMainChannelError(NSHARE::CText const& _channel_type, unsigned aError);
	bool MSendService(const data_t& aVal);

	bool MSetting();
	bool MOpen() ;
	bool MOpening(NSHARE::CBuffer::const_iterator aBegin,
			NSHARE::CBuffer::const_iterator aEnd);

	CLocalLink& operator=(CLocalLink const & aRht)
	{
		return *this;
	}


	SHARED_PTR<demand_dgs_t> FpDemands;
	SHARED_PTR<demand_dgs_for_t> FpDemandsDgFor;
	SHARED_PTR<user_data_t> FpUserDataFor;



	parser_t FServiceParser;

	smart_bridge_t FBridge;
	descriptor_t  Fd;

	eState FState;
	IMainChannel* FMainChannel;
	CLinkDiagnostic FDisgnostic;
	program_id_t const FCustomer;

	friend class CInParser<CLocalLink> ;
	//mingw bug. It does not like the typedef
};
inline descriptor_t CLocalLink::MGetID() const
{
	return Fd;
}
} /* namespace NUDT */
#endif /* CLINK_H_ */
