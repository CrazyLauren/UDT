/*
 * CFactoryManager.h
 *
 * Copyright © 2016  https://github.com/CrazyLauren
 *
 *  Created on: 12.01.2016
 *      Author:  https://github.com/CrazyLauren
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */ 
#ifndef CFACTORYMANAGER_H_
#define CFACTORYMANAGER_H_

#if defined(_MSC_VER)
#	pragma warning (push)
#	pragma warning (disable:4661)
#endif

namespace NSHARE
{
/** \brief Класс работы с фабричными объектами
 *
 * Фабрики можно добавлять как после создания объекта, так и до
 * при помощи метода sMAddFactory. Фабрики храняться в SHARED_PTR и
 * автоматически уничтожаются после вызова деструктора.
 *
 * \tparam TFactory - тип фабрики
 * \warning Будьте внимательны шаблон, содержит статический метод,
 * который вызывается при создании объектов.
 */
template<class TFactory = IFactory>
class  CFactoryManager:public CSingleton<CFactoryManager<TFactory>  >
{
public:
	typedef std::map<CText, TFactory*> factory_registry_t;
	typedef TFactory factory_t;
	typedef CFactoryManager<TFactory> fac_manager_t;

	struct  factory_its_t
	{
		typename factory_registry_t::const_iterator FBegin;
		typename factory_registry_t::const_iterator FEnd;
	};

	CFactoryManager();
	virtual ~CFactoryManager()
	{
		VLOG(2) << "FactoryManager destroyed:" << this;
	}
	/** \brief add factory
	 * \warning don't remove factory as it will be removed automatically by SHARED_PTR
	*/
	void MAddFactory(TFactory* factory);
	
	/** \brief Creates a factory T and adds it to the  manager
	*/
	template<typename T>
	static T* sMAddFactory();

	void MRemoveFactory(const CText& name);
	void MRemoveFactory(TFactory* factory);
	void MRemoveAllFactories();

	TFactory* MGetFactory(const CText& type) const;

	bool MIsFactoryPresent(const CText& name) const;

	factory_its_t MGetIterator() const;
protected:
	typedef std::vector<TFactory* > owned_factorys_t;
	virtual void MFactoryAdded(factory_t* factory){;}
	virtual void MFactoryRemoved(factory_t* factory){;}
private:
	void MAddFactoryImpl(TFactory* factory);
	void MRemoveFactoryImpl(const CText& name);
	factory_registry_t FRegistry;

	// MINGW не может корректно слинковать шаблон
	static owned_factorys_t& sMCreatedOurselves(){//fixme add shared_ptr
		static owned_factorys_t _st;
		return _st;
	}
};

template<class TFactory>
template<typename T>
inline T* CFactoryManager<TFactory>::sMAddFactory()
{
	T* _p=new T;
	if(CFactoryManager<TFactory>::sMGetInstancePtr())
	{
		CFactoryManager<TFactory>::sMGetInstancePtr()->MAddFactory(_p);
	}else
		sMCreatedOurselves().push_back(_p);
	return _p;
}
template<class TFactory>
inline CFactoryManager<TFactory>::CFactoryManager()
{
	VLOG(2) << "CFactoryManagercreated";
	owned_factorys_t& _copy=sMCreatedOurselves();
	typename owned_factorys_t::iterator _it=_copy.begin();
	VLOG_IF(1, _it != _copy.end())
			<< "Adding pre-registered Factory objects";
	for(;_it!=_copy.end();++_it)
		MAddFactoryImpl(*_it);
	sMCreatedOurselves().clear();
}
template<class TFactory>
inline void CFactoryManager<TFactory>::MAddFactoryImpl(TFactory* aFactory)
{
	CHECK_NOTNULL(aFactory);

	LOG_IF(DFATAL, MIsFactoryPresent(aFactory->MGetType())) << "Factory \""
			<< aFactory->MGetType() << "\"is already registered.";

	FRegistry[aFactory->MGetType()] = aFactory;

	VLOG(2) << "Factory for " << aFactory->MGetType() << ":" << aFactory
			<< " added";
}

template<class TFactory>
inline void CFactoryManager<TFactory>::MAddFactory(TFactory* aFactory)
{
	MAddFactoryImpl(aFactory);
	MFactoryAdded(aFactory);
}
template<class TFactory>
inline void CFactoryManager<TFactory>::MRemoveFactory(const CText& name)
{
	typename factory_registry_t::iterator _it = FRegistry.find(name);

	DLOG_IF(WARNING, _it == FRegistry.end()) << "No factory exists for "
			<< name;
	if (_it == FRegistry.end())
		return;
	MFactoryRemoved(_it->second);
	MRemoveFactoryImpl(_it->first);
}
template<class TFactory>
inline void CFactoryManager<TFactory>::MRemoveFactoryImpl(const CText& name)
{
	typename factory_registry_t::iterator _it = FRegistry.find(name);

	DLOG_IF(WARNING, _it == FRegistry.end()) << "No factory exists for "
			<< name;
	if (_it == FRegistry.end())
		return;
	delete _it->second;
	FRegistry.erase(_it);

	VLOG(2) << "WindowFactory for " << name << ":" << (*_it).second
			<< " removed. ";

//	typename owned_factorys_t::iterator _jt = sMCreatedOurselves().begin();
//	for (; _jt != sMCreatedOurselves().end(); ++_jt)
//		if ((*_it).second == (*_jt))
//		{
//			VLOG(2) << "Deleted Factory for " + (*_jt)->MGetType() + " .";
//			delete *_jt;
//			sMCreatedOurselves().erase(_jt);
//			break;
//		}
}
template<class TFactory>
inline void CFactoryManager<TFactory>::MRemoveFactory(TFactory* factory)
{
	if (factory)
	{
		MRemoveFactory(factory->MGetType());
	}

}
template<class TFactory>
inline void CFactoryManager<TFactory>::MRemoveAllFactories()
{
	for (; !FRegistry.empty();)
		MRemoveFactory((*FRegistry.begin()).second);
}
template<class TFactory>
inline TFactory* CFactoryManager<TFactory>::MGetFactory(const CText& type) const
{
	typename factory_registry_t::const_iterator _it = FRegistry.find(type);
	if (_it != FRegistry.end())
		return _it->second;

	DLOG(WARNING) << " Factory " << type << " did not founded.";
	return NULL;
}
template<class TFactory>
inline bool CFactoryManager<TFactory>::MIsFactoryPresent(
		const CText& name) const
{
	return FRegistry.find(name) != FRegistry.end();
}
template<class TFactory>
inline typename CFactoryManager<TFactory>::factory_its_t CFactoryManager<TFactory>::MGetIterator(
		) const
{
	typedef typename CFactoryManager<TFactory>::factory_its_t _its_t;
	_its_t _it;
	_it.FBegin = FRegistry.begin();
	_it.FEnd = FRegistry.end();
	return _it;
}
} /* namespace NSHARE */
#if defined(_MSC_VER)
#	pragma warning (pop)
#endif
#endif /* CFACTORYMANAGER_H_ */
