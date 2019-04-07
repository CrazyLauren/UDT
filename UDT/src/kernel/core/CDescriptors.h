/*
 * CDescriptors.h
 *
 * Copyright © 2016  https://github.com/CrazyLauren
 *
 *  Created on: 18.01.2016
 *      Author:  https://github.com/CrazyLauren
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */
#ifndef CDESCRIPTORS_H_
#define CDESCRIPTORS_H_

#include "IState.h"
#include "kernel_type.h"
namespace NUDT
{
class CDescriptors:public NSHARE::CSingleton<CDescriptors>,public IState
{
public:
	static const NSHARE::CText NAME;
	static const NSHARE::CText DESCRIPTOR_NAME;
	//static const descriptor_t MAX;
	static const descriptor_t INVALID;
	typedef NSHARE::smart_field_t<descriptor_info_t> smart_info_t;
	typedef std::map<descriptor_t, smart_info_t> d_list_t;

	CDescriptors();

	//int MOpen(const descriptor_info_t&);
	int MCreate();
	bool MOpen(descriptor_t ,const descriptor_info_t&);
	void MClose(descriptor_t);
	bool MIs(descriptor_t )const;
	bool MIs(const NSHARE::uuid_t& )const;
	bool MIsInfo(descriptor_t )const;
	void  MGetAll(d_list_t& aTo) const;
	d_list_t MGetAll(eProgramType) const;

	descriptor_t MGet(const descriptor_info_t&) const;
	descriptor_t MGet(const NSHARE::uuid_t&) const;
	std::pair<descriptor_info_t,bool> MGet(descriptor_t const&) const;
	bool MIsCustomer(descriptor_t const&) const;


	static bool  sMIsValid(descriptor_t);
	NSHARE::CConfig MSerialize() const;
private:
	int MGetFree();
	//void MIncreaseNumberOfFree();
	void MCloseInfo(const descriptor_info_t& _info, descriptor_t aVal);
	void MOpenInfo(d_list_t::iterator _it, const descriptor_info_t& aInfo,
			descriptor_t aVal);

	typedef std::map<NSHARE::uuid_t, d_list_t::iterator> uuid_list_t;
	descriptor_t FLast;
	d_list_t FDescriptors;
	uuid_list_t FByUUIDs;
	mutable NSHARE::CMutex FBLock;
};
}
#endif /* CDESCRIPTORS_H_ */
