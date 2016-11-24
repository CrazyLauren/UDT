/*
 * CPacketDivisor.h
 *
 * Copyright © 2016 Sergey Cherepanov (sergey0311@gmail.com)
 *
 *  Created on: 12.09.2016
 *      Author: Sergey Cherepanov (https://github.com/CrazyLauren)
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
	typedef CKernelIo::output_decriptors_for_t input_t;
	typedef CKernelIo::output_vector_t limits_t;

	struct packet_divisor_t
	{
		std::vector<user_data_t> FDataList;
		limits_t FFor;
	};

	typedef std::vector<packet_divisor_t > packed_user_data_t;


	enum eError
	{
		E_EOK,
		E_PACKET_LOST=-1,
		E_DOES_NOT_NEED=-2,
		//E_PACKET_SEQUENCE_FAIL=-3,
	};
	CPacketDivisor();
	~CPacketDivisor();
	void MSetLimitsFor(descriptor_t const&,split_info const&);
	std::pair<split_info,bool>  MGetLimitsFor(descriptor_t const&) const;
	bool  MRemoveLimitsFor(descriptor_t const&);

	eError MSplitOrMergeIfNeed(user_data_t const& aWhat,
			input_t const& aSendTo,packed_user_data_t& aTo,fail_send_array_t & _non_sent); //When Send packet

	//std::pair<user_data_t const&, eError> MMergeIfNeed(user_data_t const& aWhat);//when receive packet

	NSHARE::CConfig MSerialize() const;
private:
	//todo handle close

	struct split_packet_info_t
	{
		std::map<unsigned, split_packet_t> FSplitLevel;
		user_data_info_t FInform;
	};
	struct merge_operation_t
	{
		merge_operation_t(CPacketDivisor& aThis, const limits_t& aFor) :
				FThis(aThis),//
				FIsWorking(false),//
				FFor(aFor)//
		{

		}
		void MMerge(const user_data_t & aVal);
		static int sMMergeOperation(NSHARE::CThread const* WHO,
				NSHARE::operation_t* WHAT, void* YOU_DATA);
		void MMergeOperation(NSHARE::CThread const* WHO,
				NSHARE::operation_t* WHAT);
		bool MMergePacket(const user_data_t& aWhat);
		void MFilteringForMaxSize(
				const size_t aSize, uuids_t& _non_sent);

		NSHARE::CBuffer FBufForMerge;
		std::vector<user_data_t> FSplitPackets;
		CPacketDivisor& FThis;
		bool FIsWorking;
		limits_t  FFor;
	private:
		void MSendPacket(fail_send_array_t& _non_sent,user_data_info_t const&);
	};
	typedef std::map<NSHARE::uuid_t,split_packet_info_t > last_packet_info;

	typedef std::map<descriptor_t, split_info> d_info_t;


	typedef std::pair<size_t, limits_t> limit_for_value_t;
	typedef std::vector<limit_for_value_t> limits_for_t;

	void MGetLimitsFor(const user_data_t& aWhat, const input_t& aSendTo,
			limits_t& _cannot_split, limits_t& _no_limit,
			limits_for_t& _has_limits);
	size_t MSplit(const user_data_t& aWhat, std::vector<user_data_t>& aTo, size_t aPartSize);
	eError MCheckingSequence(const user_data_t& aWhat,
			fail_send_array_t& _non_sent);
	eError MProcessNotSplitedPacket(const user_data_t& aWhat, limits_t & _no_limit,
			limits_t & _cannot_split, limits_for_t & _has_limits,
			packed_user_data_t& aTo);
	eError MProcessSpltedPacket(const user_data_t& aWhat, limits_t& _no_limit,
			limits_t& _cannot_split, limits_for_t& _has_limits,
			packed_user_data_t& aTo,fail_send_array_t & _non_sent);
	split_packet_info_t& MGetSplitInfoFor(const user_data_t& aWhat);
	void MUpdatingSplitInfo(const user_data_t& aWhat);
	void MPushPacketWithoutChange(const user_data_t& aWhat, limits_t& aLimits, packed_user_data_t& aTo);
	void MLimitPacketSize(const user_data_t& aWhat, limits_for_t & _has_limits,
			packed_user_data_t& aTo);

	void MMerge(const user_data_t& aWhat, limits_t& _cannot_split, fail_send_array_t& _non_sent);

	d_info_t FLimitsInfo;
	last_packet_info FSplitInfo;
	std::map<NSHARE::uuid_t,merge_operation_t> FMergeOp;
	mutable NSHARE::CMutex FLimitsMutex;
	mutable NSHARE::CMutex FMergeMutex;
};

} /* namespace UDT */
#endif /* CPACKETDIVISOR_H_ */
