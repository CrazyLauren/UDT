/*
 * CKernelServerLink.h
 *
 * Copyright © 2016  https://github.com/CrazyLauren
 *
 *  Created on: 04.02.2016
 *      Author:  https://github.com/CrazyLauren
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */
#ifndef CLINK_KENREL_SERVER_H_
#define CLINK_KENREL_SERVER_H_

namespace NUDT
{
class IMainChannel;
class CKernelIOByTCP;

class CKernelServerLink: public ILink,NSHARE::CDenyCopying
{
public:
	static NSHARE::CText const MAIN_CHANNEL_TYPE;
	static NSHARE::CText const DEFAULT;
	static NSHARE::CText const DEFAULT_MAIN;
	static const NSHARE::CText NAME;
	CKernelServerLink(descriptor_t aFD, uint64_t FTime,
			ILinkBridge*,program_id_t const & aKernel);
	virtual ~CKernelServerLink();

	bool MSend(const program_id_t& aVal,const routing_t&,error_info_t const& aError);
	bool MSend(const kernel_infos_array_t& aVal,const routing_t&,error_info_t const& aError);
	bool MSend(const user_data_t& aVal);
	bool MSend(const data_t& aVal);
	bool MSend(const fail_send_t& aVal,const routing_t&,error_info_t const& aError);
	bool MSend(const demand_dgs_for_t& aVal,const routing_t&,error_info_t const& aError);


	bool MReceivedData(data_t::const_iterator aBegin,
			data_t::const_iterator aEnd);

	bool MReceivedData(user_data_t const&);
	bool MReceivedData(program_id_t const&,const routing_t&,error_info_t const& aError);
	bool MReceivedData(demand_dgs_t const&,const routing_t&,error_info_t const& aError);
	bool MReceivedData(demand_dgs_for_t const&,const routing_t&,error_info_t const& aError);
	bool MReceivedData(kernel_infos_array_t const& aInfo,const routing_t&,error_info_t const& aError);
	bool MReceivedData(fail_send_t const&, const routing_t& aFrom,error_info_t const& aError);
	void MCloseRequest();
	void MClose();

	descriptor_t MGetID() const;
	bool MIsOpened() const;
	bool MAccept();
	NSHARE::CConfig MSerialize() const;
	NSHARE::CConfig const& MBufSettingFor(NSHARE::CConfig const& aFrom) const;

	class CConnectionHandler;
private:

	typedef CInParser<CKernelServerLink> parser_t;

	enum eState //don't use pattern
	//as it's  difficult for the class
	{
		E_NOT_OPEN, //
		E_OPEN, //
		E_ERROR,//
	};
	template<class T>
	inline unsigned MFill(data_t* _buf, const T& _id,const routing_t&,error_info_t const& aError);



	template<class DG_T>
	void MProcess(DG_T const* aP, parser_t*);

	NSHARE::CText MGetMainChannelType(bool aDefOnly=false);
	bool MCreateMainChannel();
	bool MSetting();
	int MCloseMain();
	int MSendMainChannelError(NSHARE::CText const& _channel_type, unsigned aError);
	bool MSendService(const data_t& aVal);
	bool MSendIDInfo();
	bool MIsMainChannel() const;
	void MSendOpenedIfNeed();

	CKernelServerLink& operator=(CKernelServerLink const & aRht)
	{
		return *this;
	}
	void MChangeState(eState aNew);

	parser_t FServiceParser;

	smart_bridge_t FBridge;
	descriptor_t  Fd;

	eState FState;
	IMainChannel* FMainChannel;
	program_id_t const FKernel;
	CLinkDiagnostic FDiagnostic;
	bool FIsAccepted;


	friend class CInParser<CKernelServerLink> ;
	//mingw bug. It does not like the typedef
};
inline descriptor_t CKernelServerLink::MGetID() const
{
	return Fd;
}
} /* namespace NUDT */
#endif /* CLINK_KENREL_SERVER_H_ */
