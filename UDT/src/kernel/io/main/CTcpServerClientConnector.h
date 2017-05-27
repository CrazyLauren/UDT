/*
 * CTcpServerClientConnector.h
 *
 * Copyright © 2016  https://github.com/CrazyLauren
 *
 *  Created on: 01.11.2016
 *      Author:  https://github.com/CrazyLauren
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */
#ifndef CTCPSERVERCLIENTCONNECTOR_H_
#define CTCPSERVERCLIENTCONNECTOR_H_

namespace NUDT
{

class CTcpServerMainChannel::CMainClientConnector:public NSHARE::CIntrusived
{
public:
	struct new_link_t
	{
		ILink* FLink;
		program_id_t FId;
	};
	typedef std::map<descriptor_t, new_link_t> new_links_t;
	typedef CInParser<CMainClientConnector, CMainClientConnector*> parser_t;
	CMainClientConnector();

	void MProcess(main_channel_param_t const* aP, parser_t*);
	void MReceivedData(data_t::const_iterator const& _buf_begin,
			data_t::difference_type aSize);
	bool MAddNew(ILink* aHandler, program_id_t const& aId);
	bool MHandleServiceDG(main_channel_error_param_t const* aP,
			descriptor_t aFor);
	bool MClose(descriptor_t aFor);
	bool MCloseRequest(descriptor_t aFor);
	NSHARE::smart_field_t<new_link_t> const& MGetLastNewLink() const;
private:
	friend class CInParser<CMainClientConnector, CMainClientConnector*>;

	parser_t FParser;
	new_links_t FNewLinks;
	NSHARE::CMutex FMutexNewLinks;
	NSHARE::smart_field_t<new_link_t> FLastLink;
};

} /* namespace NUDT */
#endif /* CTCPSERVERCLIENTCONNECTOR_H_ */
