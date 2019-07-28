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
/**\brief Класс работы с фабричными объектами
 *
 * Фабрики можно добавлять как после создания объекта, так и до
 * при помощи метода sMAddFactory. Фабрики храняться в SHARED_PTR и
 * автоматически уничтожаются после вызова деструктора.
 *
 *\tparam TFactory - тип фабрики
 *\tparam TMutexType - если нужно сделать класс потокобезопасным укажите в качестве этого параметра CMutex
 *\warning Будьте внимательны шаблон, содержит статический метод,
 * который вызывается при создании объектов.
 * @warning MFactoryAdded is not called for factory which is added by sMAddFactory method
 */
template<class TFactory = IFactory,class TMutexType = CNoMutex>
class  CFactoryManager:public CSingleton<CFactoryManager<TFactory,TMutexType>  >
{
public:
	typedef std::map<CText, TFactory*> factory_registry_t;
	typedef TFactory factory_t;
	typedef CFactoryManager<TFactory,TMutexType> fac_manager_t;
	typedef TMutexType mutex_t;

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
	/**\brief add factory
	 *\warning don't remove factory as it will be removed automatically by SHARED_PTR
	*/
	void MAddFactory(TFactory* factory);
	
	/**\brief Creates a factory T and adds it to the  manager
	 * @warning Of course, #MFactoryAdded will not be called
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
	factory_registry_t FRegistry;
	mutable mutex_t	 FMutex;

	// MINGW не может корректно слинковать шаблон
	static owned_factorys_t& sMCreatedOurselves(){//fixme add shared_ptr
		static owned_factorys_t _st;
		return _st;
	}
};

template<class TFactory,class TMutexType>
template<typename T>
inline T* CFactoryManager<TFactory,TMutexType>::sMAddFactory()
{
	T* _p=new T;
	if(CFactoryManager<TFactory,TMutexType>::sMGetInstancePtr())
	{
		CFactoryManager<TFactory,TMutexType>::sMGetInstancePtr()->MAddFactory(_p);
	}else
		sMCreatedOurselves().push_back(_p);
	return _p;
}
template<class TFactory,class TMutexType>
inline CFactoryManager<TFactory,TMutexType>::CFactoryManager()
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
template<class TFactory,class TMutexType>
inline void CFactoryManager<TFactory,TMutexType>::MAddFactoryImpl(TFactory* aFactory)
{
	CHECK_NOTNULL(aFactory);

	LOG_IF(DFATAL, MIsFactoryPresent(aFactory->MGetType())) << "Factory \""
			<< aFactory->MGetType() << "\"is already registered.";
	CRAII<mutex_t> _blocked(FMutex);
	FRegistry[aFactory->MGetType()] = aFactory;

	VLOG(2) << "Factory for " << aFactory->MGetType() << ":" << aFactory
			<< " added";
}

template<class TFactory,class TMutexType>
inline void CFactoryManager<TFactory,TMutexType>::MAddFactory(TFactory* aFactory)
{
	MAddFactoryImpl(aFactory);
	MFactoryAdded(aFactory);
}
template<class TFactory,class TMutexType>
inline void CFactoryManager<TFactory,TMutexType>::MRemoveFactory(const CText& name)
{
	factory_t* _factory=NULL;
	{
		CRAII<mutex_t> _blocked(FMutex);
		typename factory_registry_t::iterator _it = FRegistry.find(name);

		DLOG_IF(WARNING, _it == FRegistry.end()) << "No factory exists for "
				<< name;
		if (_it == FRegistry.end())
			return;

		_factory=_it->second;
		_it->second = NULL;

		FRegistry.erase(_it);
		VLOG(2) << "WindowFactory for" << _it->first << " :" << _it->second
			<< " removed. ";
	}

	MFactoryRemoved(_factory);

	DVLOG(3)<<"Delete factory "<<name<<" : "<<_factory;
	delete _factory;
}
template<class TFactory,class TMutexType>
inline void CFactoryManager<TFactory,TMutexType>::MRemoveFactory(TFactory* factory)
{
	if (factory)
	{
		MRemoveFactory(factory->MGetType());
	}

}
template<class TFactory,class TMutexType>
inline void CFactoryManager<TFactory,TMutexType>::MRemoveAllFactories()
{
	for (; !FRegistry.empty();)
		MRemoveFactory((*FRegistry.begin()).second);
}
template<class TFactory,class TMutexType>
inline TFactory* CFactoryManager<TFactory,TMutexType>::MGetFactory(const CText& type) const
{
	CRAII<mutex_t> _blocked(FMutex);
	typename factory_registry_t::const_iterator _it = FRegistry.find(type);
	if (_it != FRegistry.end())
		return _it->second;

	DLOG(WARNING) << " Factory " << type << " did not founded.";
	return NULL;
}
template<class TFactory,class TMutexType>
inline bool CFactoryManager<TFactory,TMutexType>::MIsFactoryPresent(
		const CText& name) const
{
	CRAII<mutex_t> _blocked(FMutex);
	return FRegistry.find(name) != FRegistry.end();
}
template<class TFactory,class TMutexType>
inline typename CFactoryManager<TFactory,TMutexType>::factory_its_t CFactoryManager<TFactory,TMutexType>::MGetIterator(
		) const
{
	CRAII<mutex_t> _blocked(FMutex);

	typedef typename CFactoryManager<TFactory,TMutexType>::factory_its_t _its_t;
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
