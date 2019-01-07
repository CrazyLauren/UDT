/*
 * CChannelDiagnostics.h
 *
 * Copyright © 2016  https://github.com/CrazyLauren
 *
 *  Created on: 12.04.2016
 *      Author:  https://github.com/CrazyLauren
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */
#ifndef CCHANNELDIAGNOSTICS_H_
#define CCHANNELDIAGNOSTICS_H_

namespace NUDT
{
template<class TFrom>
struct overload_t
{
	bool FIsExist;
	bool FResult;
	std::vector<TFrom> FWho;
};
template<class TFrom>
struct COverload
{
	typedef TFrom from_t;
	typedef overload_t<TFrom> over_t;//fix fucking mingw

	unsigned const RETRY_COUNT;
	unsigned const MAX_DELAY; //ms
	COverload(unsigned aCount = 7, unsigned aDelay = 7);

	unsigned FOveloadCount; //if you want to get max overload
	//time you have to multiply MAX_DELAY and FOveloadCount

	void MStartCheking();
	void MCheking(TFrom const& FWho, size_t aCountBytes);
	void MFinishCheking(over_t& aResult);

	static void sMPrint(std::ostream & aStream, over_t const& aVal); //fix bug of mingw
private:
	typedef std::map<TFrom, std::size_t> fbytes_t;
	void MGetFrom(std::vector<TFrom>&);
	uint64_t FTime;
	uint64_t FCount;
	unsigned FDelta;
	fbytes_t FBytes;
};
template<class TFrom>
COverload<TFrom>::COverload(unsigned aCount, unsigned aDelay) :
		RETRY_COUNT(aCount), MAX_DELAY(aDelay)
{
	FOveloadCount = 0;
	FTime = NSHARE::get_unix_time();
	FCount = 0;
	FDelta = 0;
}
template<class TFrom>
inline void COverload<TFrom>::MStartCheking()
{
	FTime = NSHARE::get_unix_time();
}
template<class TFrom>
inline void COverload<TFrom>::MCheking(TFrom const& FWho, size_t aCount)
{
	FDelta += static_cast<unsigned>(NSHARE::get_unix_time() - FTime);
	FBytes[FWho] += aCount;
}
template<class TFrom>
inline void COverload<TFrom>::MFinishCheking(over_t& aResult)
{
	if (++FCount % RETRY_COUNT != 0) //every RETRY_COUNT
	{
		aResult.FIsExist = false;
		return;
	}

	if (FDelta > MAX_DELAY)
		aResult.FResult = false;
	else
	{
		++FOveloadCount;
		LOG(INFO) << "Overload " << FOveloadCount;
		aResult.FResult = true;
		MGetFrom(aResult.FWho);
		FBytes.clear();
	}
	FDelta = 0;
	aResult.FIsExist = true;
}
template<class TFrom>
inline void COverload<TFrom>::MGetFrom(std::vector<TFrom>& aTo)
{
	using namespace std;
	typename fbytes_t::iterator _it = FBytes.begin();
	size_t _max = 0;
	for (; _it != FBytes.end(); ++_it)
		_max += _it->second;
	if (!_max || FBytes.empty())
		return;
	_max /= FBytes.size();
	for (_it = FBytes.begin(); _it != FBytes.end(); ++_it)
		if (_it->second >= _max)
			aTo.push_back(_it->first);
}
} /* namespace NUDT */

namespace std
{
template<class TFrom>
inline std::ostream& operator<<(std::ostream & aStream,
		typename NUDT::overload_t<TFrom> const& aVal)
{
	if (aVal.FIsExist)
	{
		if (!aVal.FResult)
			aStream << "no overload";
		else
			for (typename std::vector<TFrom>::const_iterator _it =
					aVal.FWho.begin(); _it != aVal.FWho.end();)
			{
				aStream << *_it;

				if (++_it != aVal.FWho.end())
					aStream << ";";
				else
					break;
			}
	}
	return aStream;
}
}
#endif /* CCHANNELDIAGNOSTICS_H_ */
