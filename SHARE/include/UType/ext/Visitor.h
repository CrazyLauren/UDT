/*
 * Visitor.h
 *
 * Copyright © 2016 Sergey Cherepanov (sergey0311@gmail.com)
 *
 *  Created on: 17.04.2015
 *      Author: Sergey Cherepanov (https://github.com/CrazyLauren)
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */ 
#ifndef VISITOR_H_
#define VISITOR_H_


namespace NSHARE
{
class CBaseObject;
class CBaseVisitor
{
public:
	enum EBaseVisitType
	{
		VISIT_ALL, VISIT_ONCE, VISIT_PARENT, VISIT_CHILD_ONLY
	};
	CBaseVisitor()
	{
		VLOG(2) << "Construct CBaseVisitor:" << this;
		MSetMode(VISIT_ALL);
	}
	virtual ~CBaseVisitor()
	{
		VLOG(2) << "Destroy CBaseVisitor:" << this;
	}
	virtual bool MOnUnknownVisitor(CBaseObject* aVal)
	{
		VLOG(1) << "Unknown Visitor for:" << aVal << " :" << this;
		return false;
	}
	inline EBaseVisitType MSetMode(EBaseVisitType aMode)
	{
		VLOG(1) << "SetMode:" << aMode << " :" << this;
		return FMode = aMode;
	}
	inline EBaseVisitType MGetMode() const
	{
		return FMode;
	}
	void operator+=(EBaseVisitType const& aVal)
	{
		MSetMode(aVal);
	}
private:
	EBaseVisitType FMode;
};

template<class T>
class CVisitor
{
public:
	typedef bool ReturnType;
	typedef T ParamType;

	CVisitor()
	{
		VLOG(2) << "Construct CVisitor:" << this;
	}
	virtual ~CVisitor()
	{
		VLOG(2) << "Destroy CVisitor:" << this;
	}
	virtual bool MVisit(ParamType&) = 0;
	virtual bool MVisit(ParamType const&) = 0;

};

template<typename Visited>
struct CDefaultCatchAll
{
	static bool sMOnUnknownVisitor(Visited& aVisited, CBaseVisitor& aVal)
	{
		aVal.MOnUnknownVisitor(&aVisited);
		return false;
	}
};

template<template<class > class CatchAll = CDefaultCatchAll>
class SHARE_EXPORT CBaseVisitable
{
public:
	typedef bool ReturnType;
	CBaseVisitable()
	{
		VLOG(2) << "Construct CBaseVisitable:" << this;
	}
	virtual ~CBaseVisitable()
	{
		VLOG(2) << "Destroy CBaseVisitable:" << this;
	}
	virtual ReturnType MAccept(CBaseVisitor&) = 0;
	virtual ReturnType MAccept(CBaseVisitor&) const = 0;
	virtual void MVisitChild(CBaseVisitor& aVisitor) const
	{
		LOG(WARNING) << "const MVisitChildisn't overload" << this;
	}
	virtual void MVisitChild(CBaseVisitor& aVisitor)
	{
		LOG(WARNING) << "MVisitChild isn't overload" << this;
	}
	virtual void MVisitParent(CBaseVisitor& aVisitor) const
	{
		LOG(WARNING) << "const MVisitParent isn't overload" << this;
	}
	virtual void MVisitParent(CBaseVisitor& aVisitor)
	{
		LOG(WARNING) << "MVisitParent isn't overload" << this;
	}
protected:

	template<class T>
	static ReturnType sMAcceptImpl(T& aVisited, CBaseVisitor& aVisitor)
	{
		VLOG(2) << "Accept to "<<CTypeInfo(aVisitor).MName() << &aVisited <<", visitor:"<<&aVisitor;
		bool _is = false;
		ReturnType _rval = ReturnType();
		if (CVisitor<T>* p = dynamic_cast<CVisitor<T>*>(&aVisitor))
		{
			VLOG(2) << "Type " << CTypeInfo(aVisitor).MName() << "is equal to "
					<<get_type_info<CVisitor<T> >().MName()
					<< ", visitor:"
					<< &aVisitor;
			_is = true;
			_rval = p->MVisit(aVisited);
		}
		sMVisitOther(aVisited, aVisitor);
		if (_is)
			return _rval;
		else
			return CatchAll<T>::sMOnUnknownVisitor(aVisited, aVisitor);
	}

	template<class T>
	static ReturnType sMAcceptImpl(const T& aVisited, CBaseVisitor& aVisitor)
	{
		VLOG(2) << "Accept to "<<CTypeInfo(aVisitor).MName() << &aVisited <<", visitor:"<<&aVisitor;
		bool _is = false;
		ReturnType _rval = ReturnType();
		if (CVisitor<T>* p = dynamic_cast<CVisitor<T>*>(&aVisitor))
		{
			VLOG(2) << "Type " << CTypeInfo(aVisitor).MName() << "is equal to "
								<< get_type_info<CVisitor<T> >().MName() << ", visitor:"
								<< &aVisitor;
			_is = true;
			_rval = p->MVisit(aVisited);
		}
		sMVisitOther(aVisited, aVisitor);
		if (_is)
			return _rval;
		else
			return CatchAll<T>::sMOnUnknownVisitor(const_cast<T&>(aVisited),
					aVisitor);

	}
protected:
	template<class T>
	static void sMVisitOther(T const& aVisited, CBaseVisitor& aVisitor)
	{
		VLOG(2) << "Visit mode "<<aVisitor.MGetMode()<< &aVisited <<", visitor:"<<&aVisitor;
		switch (aVisitor.MGetMode())
		{
		case CBaseVisitor::VISIT_ALL:
			aVisited.MVisitChild(aVisitor);
			break;
		case CBaseVisitor::VISIT_PARENT:
			aVisited.MVisitParent(aVisitor);
			break;
		case CBaseVisitor::VISIT_ONCE:
			break;
		case CBaseVisitor::VISIT_CHILD_ONLY:
			aVisitor.MSetMode(CBaseVisitor::VISIT_ONCE);
			aVisited.MVisitChild(aVisitor);
			aVisitor.MSetMode(CBaseVisitor::VISIT_CHILD_ONLY);
			break;
		default:
			MASSERT_1(false);
			break;

		}
	}
	template<class T>
	static void sMVisitOther(T & aVisited, CBaseVisitor& aVisitor)
	{
		VLOG(2) << "Visit mode "<<aVisitor.MGetMode()<< &aVisited <<", visitor:"<<&aVisitor;
		switch (aVisitor.MGetMode())
		{
		case CBaseVisitor::VISIT_ALL:
			aVisited.MVisitChild(aVisitor);
			break;
		case CBaseVisitor::VISIT_PARENT:
			aVisited.MVisitParent(aVisitor);
			break;
		case CBaseVisitor::VISIT_ONCE:
			break;
		case CBaseVisitor::VISIT_CHILD_ONLY:
			aVisitor.MSetMode(CBaseVisitor::VISIT_ONCE);
			aVisited.MVisitChild(aVisitor);
			aVisitor.MSetMode(CBaseVisitor::VISIT_CHILD_ONLY);
			break;
		default:
			MASSERT_1(false);
			break;

		}
	}
};
}
namespace std
{
//VISIT_ALL, VISIT_ONCE, VISIT_PARENT, VISIT_CHILD_ONLY
inline std::ostream& operator <<(std::ostream& aStream,
		NSHARE::CBaseVisitor::EBaseVisitType const& aPrior)
{
	switch (aPrior)
	{
	case NSHARE::CBaseVisitor::VISIT_ALL:
		aStream << "Max";
		break;
	case NSHARE::CBaseVisitor::VISIT_ONCE:
		aStream << "High";
		break;
	case NSHARE::CBaseVisitor::VISIT_PARENT:
		aStream << "Normal";
		break;
	case NSHARE::CBaseVisitor::VISIT_CHILD_ONLY:
		aStream << "Low";
		break;
	default:
		LOG(ERROR) << "Unknown visitor";
		aStream << "Unknown visitor";
		break;
	}
	return aStream;
}
}
//см CObject and Root

#define DEFINE_VISIT_AS_PARENT(_class) \
    virtual ReturnType MAccept(NSHARE::CBaseVisitor& guest) \
    {  return sMAcceptImpl<_class>(*this, guest); }\
    virtual ReturnType MAccept(NSHARE::CBaseVisitor& guest) const \
       { return sMAcceptImpl<_class>(*this, guest); }

#endif /* VISITOR_H_ */
