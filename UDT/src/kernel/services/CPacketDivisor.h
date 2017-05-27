/*
 * CPacketDivisor.h
 *
 * Copyright © 2016  https://github.com/CrazyLauren
 *
 *  Created on: 12.09.2016
 *      Author:  https://github.com/CrazyLauren
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */
#ifndef CPACKETDIVISOR_H_
#define CPACKETDIVISOR_H_

namespace NUDT
{

class CPacketDivisor: public NSHARE::CSingleton<CPacketDivisor>, public IState
{
public:
	static const NSHARE::CText NAME;

//	struct packet_divisor_t
//	{
//		std::vector<user_data_t> FDataList;
//		user_datas_t FFor;
//	};

	//typedef std::vector<packet_divisor_t > packed_user_data_t;


//	enum eError
//	{
//		E_DOES_NOT_NEED=-2,
//		//E_PACKET_SEQUENCE_FAIL=-3,
//	};
	CPacketDivisor();
	~CPacketDivisor();
	void MSetLimitsFor(descriptor_t const&,split_info const&);
	std::pair<split_info,bool>  MGetLimitsFor(descriptor_t const&) const;
	bool  MRemoveLimitsFor(descriptor_t const&);

	void MSplitOrMergeIfNeed(descriptor_t aFor,user_datas_t & aWhat,user_datas_t& aTo,fail_send_array_t & _non_sent); //When Send packet

	//std::pair<user_data_t const&, eError> MMergeIfNeed(user_data_t const& aWhat);//when receive packet

	NSHARE::CConfig MSerialize() const;
private:
	//todo handle close
	struct merge_key
	{
		//merge_key():FUUID(0),FFor(-1){}
		merge_key(NSHARE::uuid_t const& aUUID,descriptor_t const& aFor):FUUID(aUUID),FFor(aFor)
		{

		}
		inline bool operator<(const merge_key& aRht) const
		{
			return FUUID==aRht.FUUID?FFor<aRht.FFor:FUUID<aRht.FUUID;
		}
		NSHARE::uuid_t FUUID;
		descriptor_t FFor;
	};
	struct merge_operation_t
	{
		merge_operation_t(CPacketDivisor& aThis,user_data_info_t const& aFor,descriptor_t aDesc);
		void MMerge(user_datas_t & aVal,user_datas_t& aTo);
		static NSHARE::eCBRval sMMergeOperation(NSHARE::CThread const* WHO,
				NSHARE::operation_t* WHAT, void* YOU_DATA);
		NSHARE::eCBRval MMergeOperation(NSHARE::CThread const* WHO,
				NSHARE::operation_t* WHAT);
		bool MMergePacket(const user_data_t& aWhat);
		bool MCreatePacket(user_datas_t& aTo);
		bool MHasToBeRemoved() const;
		NSHARE::CBuffer FBufForMerge;
		user_datas_t FMergedPackets;
		user_datas_t FNewPackets;
		CPacketDivisor& FThis;
		bool FIsWorking;
		user_data_info_t const  FFor;
		descriptor_t const FDescriptor;
		std::map<unsigned, split_packet_t> FSplitLevel;
		eError FError;
		bool FIsMerged;
	private:
		bool MMerging(
				user_datas_t& aTo);
		bool MCheckingSequence(
				user_data_t const& aTo);

	};

	typedef std::map<descriptor_t, split_info> d_info_t;


	typedef std::pair<size_t, user_datas_t> limit_for_value_t;
	typedef std::map<merge_key,merge_operation_t> merge_operations_map_t;

	size_t MSplit(user_datas_t& aWhat, user_datas_t& aTo, size_t aPartSize);

	void MUpdatingSplitInfo(const user_data_t& aWhat);


	void MMerge(descriptor_t aFor, user_datas_t& _cannot_split,user_datas_t& aTo,user_datas_t& _fails, fail_send_array_t& _non_sent);

	d_info_t FLimitsInfo;
	merge_operations_map_t FMergeOp;
	mutable NSHARE::CMutex FLimitsMutex;
	mutable NSHARE::CMutex FMergeMutex;
	mutable NSHARE::CMutex FMergedPacketsMutex;
};

} /* namespace UDT */
#endif /* CPACKETDIVISOR_H_ */
