/*
 * CRequiredDG.h
 *
 * Copyright © 2016 Sergey Cherepanov (sergey0311@gmail.com)
 *
 *  Created on: 18.02.2016
 *      Author: Sergey Cherepanov (https://github.com/CrazyLauren)
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */
#ifndef CREQUIREDDG_H_
#define CREQUIREDDG_H_
#include <udt_types.h>
#include <shared_types.h>
namespace NUDT
{

class CRequiredDG
{
public:
	typedef std::set<NSHARE::uuid_t> unique_uuids_t;
	//typedef std::map<NSHARE::CText,unique_uuids_t> uuids_of_t;
	typedef std::map<required_header_t, unique_uuids_t, CReqHeaderFastLessCompare> uuids_of_expecting_dg_t;
	typedef std::map<NSHARE::CText, uuids_of_expecting_dg_t,
			NSHARE::CStringFastLessCompare> protocols_t;

	typedef std::map<NSHARE::uuid_t, protocols_t> protocol_of_uuid_t;

	static const NSHARE::CText NAME;
	CRequiredDG();
	 ~CRequiredDG();

	 //first added, second removed
	std::pair<demand_dgs_for_t, demand_dgs_for_t> MSetDemandsDGFor(id_t const&, demand_dgs_t const&); //return who will send data and what
	//return added
	demand_dgs_for_t MAddClient(id_t const& aId );//return who will send data and what
	//return removed
	demand_dgs_for_t MRemoveClient(NSHARE::uuid_t const& aUUID);

	//bool MUpdateDemandsDG(id_t const&, demand_dgs_t const&, demand_dgs_t& aNew);
	NSHARE::smart_field_t<uuids_t> MGetCustomersFor(NSHARE::uuid_t const& aFor,
			NSHARE::CText const& aProtocol, void const*,
			unsigned const aSize) const;
	NSHARE::smart_field_t<uuids_t> MGetCustomersFor(NSHARE::uuid_t const& aFor,
			 unsigned const aNumber) const;



	unique_uuids_t  MGetUUIDFor(NSHARE::CRegistration const&) const;
	NSHARE::CConfig MSerialize() const;
	demand_dgs_for_t const& MGetDemands() const;
private:


	void MSendPacketFromTo(NSHARE::uuid_t const& aFrom,NSHARE::uuid_t const& aTo,demand_dg_t const& aWhat,demand_dgs_for_t& aNew);
	void MUnSendPacketToFrom(NSHARE::uuid_t const& aTo,demand_dgs_t const& aFrom,demand_dgs_for_t & aOld);
	void MUpdateRequrementFor(const demand_dgs_t& aReqDgs, const id_t& aFor,demand_dgs_for_t& aNew);
	void MUpdateRecipientOf(id_t const& aId,demand_dgs_for_t& aNew);
	bool MRemoveDemandsDG(NSHARE::uuid_t const&,demand_dgs_for_t & aOld);

	bool MGetDiffDemandsDG(id_t const& aFor, demand_dgs_t const& aNew, demand_dgs_t& aRem,demand_dgs_t& aAdd);
	bool MDoesIdConformTo(id_t const& aFor,NSHARE::CRegistration const&) const;
	protocol_of_uuid_t FNeedSendTo;
	demand_dgs_for_t FDGs;
	std::set<id_t> FIds;
};

} /* namespace NUDT */
#endif /* CREQUIREDDG_H_ */
