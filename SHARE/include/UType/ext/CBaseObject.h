/*
 * CBaseObject.h
 *
 * Copyright © 2016 Sergey Cherepanov (sergey0311@gmail.com)
 *
 *  Created on: 11.09.2015
 *      Author: Sergey Cherepanov (https://github.com/CrazyLauren)
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */  
#ifndef CBASEOBJECT_H_
#define CBASEOBJECT_H_

#include <deftype>

#include <UType/ext/name_type.h>
#include <UType/ext/CProperties.h>
#include <UType/ext/Visitor.h>
#include <UType/version.h>

#define ADD_VERSION(Class,_major,_minor)\
	typedef NSHARE::impl_version_t<Class, (_major), (_minor)> version;

/*	! changelog
 *  0.1 -release
 *  0.2 - Added log to source, reflactoring.
 *
 */
namespace boost
{
namespace property_tree
{
template<class Key, class Data, class KeyCompare>
class basic_ptree;
}
}
//todo maybe deprecated
namespace NSHARE
{
class SHARE_EXPORT CBaseObject: public CProperties<CMutex>, public CBaseVisitable<>
{
	friend class CBaseVisitable<> ;
public:
	ADD_VERSION(CBaseObject,0,2)
//!@struct ptree_t
//!@brief Тип внешнего дерева свойств, необходимго для чтения, записи настроек граф сцены
typedef	boost::property_tree::basic_ptree<std::string, std::string,
	std::less<std::string> > ptree_t;
//!@struct EUnknowenPath
//!@brief  Исключение - Элмента по этому пути не существует.
	struct EUnknowenPath//TODO
	{

	};
	typedef CProperties<CMutex> CPTree;
	explicit CBaseObject(const name_t& name = name_t());
	CBaseObject(const CBaseObject& aRht);
	CBaseObject& operator=(const CBaseObject& aRht);
	virtual ~CBaseObject();

	//!@param name ID
	name_t::full_name_t MSetType(const name_t::type_t& name,
			name_t::number_t aNumber);
	virtual name_t::full_name_t MSetName(const name_t&);
	name_t::full_name_t MSetNubmer(name_t::number_t);
	name_t::full_name_t MSetFullName(const name_t::full_name_t&);

	inline name_t::type_t const& MGetType() const;
	inline name_t::full_name_t MGetFullName() const;
	inline name_t const& MGetName() const;

	//!@brief Сохранение всех свойств объекта  в ptree_t
	virtual NSHARE::CText MWritePropeties(ptree_t *, NSHARE::CText aPrefix="") const;
	//!@brief Добавить свойства из ptree_t
	virtual void MReadPropeties(ptree_t const&) throw (EUnknowenPath);
	virtual std::ostream& MPrintPTree(std::ostream & aStream) const;

	virtual std::ostream& MPrint(std::ostream & aStream) const;

	template<class T>
	static NSHARE::version_t const& sMVersion();//FIXME
	virtual NSHARE::version_t const& MVersion() const=0;
	virtual unsigned MRevision() const;

	virtual ReturnType MAccept(CBaseVisitor& guest)
	{
		return sMAcceptImpl<CBaseObject>(*this, guest);
	}
	virtual ReturnType MAccept(CBaseVisitor& guest) const
	{
		return sMAcceptImpl<CBaseObject>(*this, guest);
	}
protected:
	virtual bool MPropertyChanged(properties_t::key_type const& aKey,
			property_value_type const& aPrevVal);
private:
	name_t FName;

	friend std::ostream& operator <<(std::ostream& aStream, const CBaseObject&);
	friend ptree_t& operator <<(ptree_t& aStream, const CBaseObject&);
	friend ptree_t const& operator>>(ptree_t const& aPTree, CBaseObject& aBase);
};
inline name_t::full_name_t CBaseObject::MGetFullName() const
{
	return MGetName().MGetFullName();
}
inline name_t const& CBaseObject::MGetName() const
{
	return FName;
}
inline name_t::type_t const& CBaseObject::MGetType() const
{
	return MGetName().MGetType();
}

template<class T>
NSHARE::version_t const& CBaseObject::sMVersion()
{
	return ver_impl_t<T>::_v::sV();
}

inline NSHARE::CBaseObject::ptree_t& operator <<(
		NSHARE::CBaseObject::ptree_t& aStream, const NSHARE::CBaseObject& aBase)
{
	aBase.MWritePropeties(&aStream);
	return aStream;
}
inline NSHARE::CBaseObject::ptree_t const& operator>>(
		NSHARE::CBaseObject::ptree_t const& aPTree, NSHARE::CBaseObject& aBase)
{
	aBase.MReadPropeties(aPTree);
	return aPTree;
}
} /* namespace NSHARE */

namespace std
{
inline std::ostream& operator <<(std::ostream& aStream,
		const NSHARE::CBaseObject& aBase)
{
	return aBase.MPrint(aStream);
}
}
#endif /* CBASEOBJECT_H_ */
