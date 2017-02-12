/*
 * CMSGSniffer.h
 *
 *  Created on: 06.02.2017
 *      Author: s.n.cherepanov
 */

#ifndef CMSGSNIFFER_H_
#define CMSGSNIFFER_H_

namespace NUDT
{
class CMSGSniffer:public NSHARE::CSingleton<CMSGSniffer>, public IState
{
public:
	static const NSHARE::CText NAME;
	CMSGSniffer();
	~CMSGSniffer();
	NSHARE::CConfig MSerialize() const;
};

} /* namespace UDT */
#endif /* CMSGSNIFFER_H_ */
