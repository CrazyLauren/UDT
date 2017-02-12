/*
 * CBuffering.h
 *
 * Copyright © 2016 Sergey Cherepanov (sergey0311@gmail.com)
 *
 *  Created on: 10.08.2016
 *      Author: Sergey Cherepanov (https://github.com/CrazyLauren)
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */
#ifndef CBUFFERING_H_
#define CBUFFERING_H_

namespace NUDT
{

class CBuffering
{
public:
	static const NSHARE::CText NAME;
	typedef user_datas_t data_list_t;

	static const NSHARE::CText MAX_BUFFER_SIZE_FOR_CHANNEL;
	static const NSHARE::CText MAX_SM_BUFFER;
	static const NSHARE::CText SM_FLAG;

	CBuffering(NSHARE::CConfig const&);
	CBuffering(unsigned const aMaxBufferSizeForChannel);

	CBuffering(unsigned const aMaxBufferSizeForChannel,
			unsigned const aMaxSMBufferSize,uint32_t& aSMBuffer);
	~CBuffering();

	bool MIsEmpty() const;

	bool MTry(data_list_t & aTo);
	void MFinish(data_list_t & aNoSent,bool aTryAgain=true);
	bool MIsWorking() const;

	bool MPut(user_datas_t & aData);
	NSHARE::CConfig MSerialize() const;
	void MClearImidiatle(data_list_t& aTo);
private:
	inline void MInit();

	data_list_t FData;
	unsigned const FMaxBufferSizeForChannel;
	unsigned const FMaxSMBufferSize;

	size_t FBuffer;
	size_t FSMBuffer;

	size_t FHandlingBuffer;

	uint32_t& FCurrentSMBuffer;
	unsigned FDoingThread;

	struct diagnostic_t
	{
		size_t FMaxBuffering;
		size_t FMaxSmBuffering;
		size_t FHandledDataSize;
	} FDiagnostic;
};
inline bool CBuffering::MIsEmpty() const
{
	return FData.empty();
}
inline void CBuffering::MClearImidiatle(data_list_t& aTo)
{
	aTo.splice(aTo.end(),FData);
}
} /* namespace NUDT */
#endif /* CBUFFERING_H_ */
