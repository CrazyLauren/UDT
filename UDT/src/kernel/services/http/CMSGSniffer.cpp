/*
 * CMSGSniffer.cpp
 *
 *  Created on: 06.02.2017
 *      Author: s.n.cherepanov
 */
#include <deftype>
#include <algorithm>
#include <iterator>
#include <core/IState.h>
#include <core/CDescriptors.h>
#include <core/CDataObject.h>
#include <core/kernel_type.h>
#include <io/CKernelIo.h>

#include "CMSGSniffer.h"
template<>
NUDT::CMSGSniffer::singleton_pnt_t NUDT::CMSGSniffer::singleton_t::sFSingleton =
		NULL;

namespace NUDT
{
const NSHARE::CText CMSGSniffer::NAME = "msg_sniffer";
CMSGSniffer::CMSGSniffer():IState(NAME)
{

}

CMSGSniffer::~CMSGSniffer()
{
}
NSHARE::CConfig CMSGSniffer::MSerialize() const
{
	NSHARE::CConfig _config(NAME);
	return _config;
}
} /* namespace UDT */
