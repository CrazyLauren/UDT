/*
 * CBaseObject.cpp
 *
 * Copyright © 2016 Sergey Cherepanov (sergey0311@gmail.com)
 *
 *  Created on: 11.09.2015
 *      Author: Sergey Cherepanov (https://github.com/CrazyLauren)
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */   
#include <boost/property_tree/ptree.hpp>
#include <revision.h>
#include <UType/ext/CBaseObject.h>

namespace NSHARE
{
#define VERSION_KEY version_t::NAME
CBaseObject::CBaseObject(const name_t& name) :
		FName(name)
{
	VLOG(2) << "Create Object:" << name;
}
CBaseObject::CBaseObject(const CBaseObject& aRht) :
		CPTree(aRht)
{
	VLOG(2) << "Construct object: From " << aRht.MGetFullName() << ":"
						<< &aRht;
}
CBaseObject& CBaseObject::operator=(const CBaseObject& aRht)
{
	VLOG(2) << "Copy object: From " << aRht.MGetFullName() << ":" << &aRht;
	CHECK_NE(&aRht, this);
	FName = aRht.MGetName();
	return *this;
}
CBaseObject::~CBaseObject()
{
	VLOG(2) << "Destruct object: From " << MGetFullName() << ":" << this;
}

name_t::full_name_t CBaseObject::MSetName(const name_t& aName)
{
	VLOG(2) << "Set new name: " << aName << ":" << this;
	FName = aName;
	return MGetFullName();
}

name_t::full_name_t CBaseObject::MSetType(const name_t::type_t& name,
		name_t::number_t aNumber)
{
	VLOG(2) << "Set new type: " << name << ", Number:" << aNumber << ":"
						<< this;
	return MSetName(name_t(name, aNumber));
}
name_t::full_name_t CBaseObject::MSetNubmer(name_t::number_t aVal)
{
	VLOG(2) << "Set new number:" << aVal << ":" << this;
	name_t _name = MGetName();
	if (_name.MSetName(MGetType(), aVal))
		return MSetName(_name);
	else
		return MGetFullName();
}
name_t::full_name_t CBaseObject::MSetFullName(const name_t::full_name_t& name)
{
	VLOG(2) << "Set by full name:" << name << ":" << this;
	name_t _tmp = MGetName();
	if (_tmp.MSetByFullName(name))
		return MSetName(_tmp);
	else
		return MGetFullName();
}
std::ostream& CBaseObject::MPrintPTree(std::ostream & aStream) const
{
	VLOG(2) << "MPrintPTree to " << &aStream;
	if (!MIsPropertyEmpty())
	{
		aStream << "properties:" << std::endl;
		CPTree::MPrintPTree(aStream);
	}
	return aStream;
}
bool CBaseObject::MPropertyChanged(properties_t::key_type const& aKey,
		property_value_type const& aPrevVal)
{
	VLOG(2) << "MPropertyChanged " << aKey << " PrevVal:" << aPrevVal
						<< " in " << MGetFullName() << ":" << this;
	if (VERSION_KEY == aKey )
	{
		if (aPrevVal != property_value_type()) //not new
		{
			LOG(WARNING)<< "Try to change Version property in "
			<< MGetFullName() << ":" << this;
			return false;
		}
		else
		VLOG(1)<<"Version of class "<< MVersion();
	}

	return CPTree::MPropertyChanged(aKey, aPrevVal);
}

unsigned CBaseObject::MRevision() const
{
	return 0;
}
std::ostream& CBaseObject::MPrint(std::ostream & aStream) const
{
	return aStream << "The MPrint method isn't implemented for \""
			<< MGetFullName() << "\".";
}
struct _for_each
{
	CBaseObject::ptree_t *FTree;
	NSHARE::CText& FPrefix;
	_for_each(CBaseObject::ptree_t * aVal, NSHARE::CText& aVal2) :
			FTree(aVal), FPrefix(aVal2)
	{
		VLOG(2) << "Put properties to :" << aVal << "; Prefix:" << FPrefix
							<< ":" << this;
	}
	void operator()(CBaseObject::properties_t::key_type const& aVal1,
			CBaseObject::property_value_type const& aVal2)
	{
		VLOG(3) << "Put property " << aVal1 << " = " << aVal2 << " :"
							<< this;
		FTree->put((FPrefix + aVal1).MToStdString(), aVal2.MToStdString());
	}
};
NSHARE::CText CBaseObject::MWritePropeties(ptree_t * aPtree,
		NSHARE::CText aPrefix) const //TODO
{
	VLOG(2) << "Write properties to :" << aPtree << "; Prefix:" << aPrefix
						<< ":" << this;
	_for_each _val(aPtree, aPrefix);
	aPrefix += MGetFullName().MToStdString() + ".";
	MForEach(_val);
	return aPrefix;
}
void CBaseObject::MReadPropeties(ptree_t const& aTree) throw (EUnknowenPath)
{
	VLOG(2) << "Read properties from :" << &aTree << " :" << this;
	if (aTree.empty() && aTree.data().empty())
	{
		VLOG(1) << "Ptree :" << &aTree << "  is empty:" << this;
		MCleanAllProperties();
	}
	else
	{
		ptree_t::const_iterator _it = aTree.begin();
		for (; _it != aTree.end(); ++_it)
		{
			VLOG(3) << "Property " << _it->first << " :" << this;
			ptree_t::value_type const& _val = *_it;
			if (_val.second.empty())
			{
				VLOG(3) << "Put Property " << _it->first << " = "
									<< _val.second.data() << " :" << this;
				MPutProperty(_val.first, _val.second.data());
			}
			else if (_val.first == MGetFullName())
				MReadPropeties(_val.second);
			else
				throw EUnknowenPath();
		}
	}
}
} /* namespace NSHARE */

