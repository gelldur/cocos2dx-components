#pragma once

#include "utils/Callback.h"
#include "component/Notification.h"

#include <vector>
#include <deque>
#include <typeindex>
#include <cstdlib>
#include <new>
#include <functional>

#define MAKE_NOTIFICATION( NAME, args... ) inline const KoalaComponent::Notification<args>& getNotification##NAME(){static const KoalaComponent::Notification<args> variable(691283); return variable;}

//TODO 10 posible bug.
//We keep pointer to object that we want remove but sometime object can already die and other object can be at this pointer?

namespace KoalaComponent
{

//Notifier can't retain listeners because sometimes we want unregister in destructor.
class Notifier
{
public:
	Notifier() {}

	virtual ~Notifier()
	{
		for( auto&& pVector : m_callbacks )
		{
			delete pVector;
		}
	}

	Notifier& operator= ( const Notifier& ) = delete;
	Notifier( const Notifier& ) = delete;
	Notifier& operator= ( Notifier && ) = delete;
	Notifier( Notifier&& ) = delete;

	static Notifier& getGlobal()
	{
		static Notifier globalNotifier;
		return globalNotifier;
	}

	template<typename... Args>
	void addNotification( const Notification<Args...>& notification,
						  const Utils::Callback<void ( Args... )>& callback )
	{
		using CallbackType = Utils::Callback<void ( Args... )>;
		CCAssert( callback.isCallable(), "Callback isn't set" );
		CCAssert( callback.getObject(), "Notifier is't ready for lambdas" );

		if(notification.tag >= (int)m_callbacks.size())
		{
			m_callbacks.resize(notification.tag + 256,nullptr);
		}

		if(m_callbacks[notification.tag] == nullptr)
		{
			auto pVector = new VectorImpl<CallbackType>();
			pVector->vector.reserve(256);

			m_callbacks[notification.tag] = pVector;
		}

		auto pVector = static_cast< VectorImpl<CallbackType>* >(m_callbacks[notification.tag]);

#ifdef DEBUG
		pVector->checkForDuplicates(callback.getObject(),callback.getFunctionPointer());
#endif

		pVector->vector.emplace_back(Element<CallbackType>(callback) );
	}

	template<typename NotificationType, typename... Args>
	void notify( const NotificationType& notification, Args&& ... params )
	{
		using CallbackType = Utils::Callback<void ( Args... )>;
		assert( notification.tag > NotificationConst::UNUSED_TAG );

		if( notification.tag >= static_cast<int>( m_callbacks.size() )
				|| m_callbacks[notification.tag] == nullptr )
		{
			return;
		}

		assert(dynamic_cast<VectorImpl<CallbackType>*>(m_callbacks[notification.tag]));
		auto pVector = static_cast< VectorImpl<CallbackType>* >(m_callbacks[notification.tag]);
		for( int i = static_cast<int>(pVector->vector.size()) - 1;i > -1;--i )
		{
			auto&& element = pVector->vector[i];
			if(element.toRemove == false)
			{
				element.callback.call(std::forward<Args> ( params )...);
			}
			else
			{
				std::swap(pVector->vector[i],pVector->vector.back());
				pVector->vector.pop_back();
			}
		}
	}

	void removeAllForObject( Utils::BaseClass* const pObject )
	{
		for(auto&& pVector : m_callbacks)
		{
			if(pVector != nullptr)
			{
				(pVector->*pVector->removeCallback)(pObject);
			}
		}
	}

	template<typename... Args>
	void removeNotification( Utils::BaseClass* pObject, const Notification<Args...>& notification )
	{
		if( notification.tag >= static_cast<int>( m_callbacks.size() )
				|| m_callbacks[notification.tag] == nullptr )
		{
			return;
		}

		auto&& pVector = m_callbacks[notification.tag];
		(pVector->*pVector->removeCallback)(pObject);
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////
private:

	template <typename Type>
	struct Element
	{
		Element(const Type& callback ):callback(callback){}
		bool toRemove = false;
		Type callback;
	};

	struct VectorInterface
	{
		virtual ~VectorInterface(){}
		typedef void (VectorInterface::*RemoveForObjectCallback)(Utils::BaseClass* const);
		RemoveForObjectCallback removeCallback = nullptr;

#ifdef DEBUG
		virtual void checkForDuplicates(Utils::BaseClass* pObject,void* pFunctionPointer) = 0;
#endif
	};

	template<typename Type>
	struct VectorImpl : public VectorInterface
	{
		std::vector< Element<Type> > vector;

		VectorImpl()
		{
			removeCallback = static_cast<RemoveForObjectCallback>(&VectorImpl<Type>::removeForObject);
		}

		virtual ~VectorImpl(){}

		void removeForObject(Utils::BaseClass* const pObject)
		{
			for(auto&& element : vector)
			{
				if(element.callback.getObject() == pObject)
				{
					element.toRemove = true;
				}
			}
		}

		/**
		 * I know that comparing function pointers can cause UB but we do
		 * it only in DEBUG for more safety
		 */
#ifdef DEBUG
		virtual void checkForDuplicates(Utils::BaseClass* pObject,void* pFunctionPointer)
		{
			for(auto&& element : vector)
			{
				if(element.toRemove == false
						&& element.callback.getObject() == pObject
						&& element.callback.getFunctionPointer() == pFunctionPointer)
				{
					assert(false && "You already add this notification. Please remove previous");
				}
			}
		}
#endif

	};

	std::vector < VectorInterface* > m_callbacks;
};


} /* namespace KoalaComponent */
