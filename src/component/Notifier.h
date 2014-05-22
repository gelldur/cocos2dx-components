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
		for( auto && localVector : m_callbacks )
		{
			for( auto && element : localVector )
			{
				free( element.pCallback );
			}
		}

		m_callbacks.clear();

		for( const Element& element : m_changesStack )
		{
			free( element.pCallback );
		}

		m_changesStack.clear();
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

		Element element;
		element.tag = notification.tag;

		const size_t sizeOfCallback = sizeof( CallbackType );
		CallbackType* pCallback = static_cast<CallbackType*>( malloc(
									  sizeOfCallback ) );
		//Placement new
		new( pCallback ) CallbackType( callback );

		element.pCallback = pCallback;
		element.pIdentyfier = callback.getObject();

		//See how do we remove objects it should be faster
		m_changesStack.emplace_front( element );

#ifdef DEBUG
		test_isElementDuplicated( element );
#endif
	}

	template<typename NotificationType, typename... Args>
	void notify( const NotificationType& notification, Args&& ... params )
	{
		using CallbackType = typename notification_traits<NotificationType>::callback_type;
		assert( notification.tag > NotificationConst::UNUSED_TAG );

		applyChanges();

		//Always first element is a semaphore
		if( notification.tag >= static_cast<int>( m_callbacks.size() )
				|| m_callbacks[notification.tag].empty() )
		{
			return;
		}

		assert( notification.tag < static_cast<int>( m_callbacks.size() ) );
		const auto& localVector = m_callbacks[notification.tag];

#ifdef DEBUG
		//Start working on this vector
		assert( notification.tag < static_cast<int>( m_semaphores.size() ) );
		++m_semaphores[notification.tag];
#endif

		for( const Element& element : localVector )
		{
			CCAssert( element.pCallback, "You don't set callback?" );
			CCAssert( element.pIdentyfier->retainCount() > 0,
					  "Probably you release object during notification or you simply didn't "
					  "unregister your previous object. Look for this notification usage" );
			static_cast<CallbackType*>( element.pCallback )->call(
				std::forward<Args> ( params )... );
		}

#ifdef DEBUG
		//Stop working on this vector
		assert( notification.tag < static_cast<int>( m_semaphores.size() ) );
		--m_semaphores[notification.tag];
#endif
	}

	void removeAllForObject( Utils::BaseClass* const pObject )
	{
		assert( pObject );
		Element element;
		element.pIdentyfier = pObject;

		//See how do we remove objects it should be faster
		//We add objects at front and remove them on end
		m_changesStack.emplace_front( element );
	}

	template<typename... Args>
	void removeNotification( Utils::BaseClass* pObject, const Notification<Args...>& notification )
	{
		Element element;
		element.pIdentyfier = pObject;
		element.tag = notification.tag;

		m_changesStack.emplace_front( element );
	}

#ifdef DEBUG
	bool isAnyRunning()
	{
		for( auto && element : m_semaphores )
		{
			if( element == true )
			{
				return true;
			}
		}

		return false;
	}

	size_t getListenersCount( size_t tag )
	{
		if( tag >= m_callbacks.size() )
		{
			return 0;
		}

		return m_callbacks[tag].size();
	}
#endif
	////////////////////////////////////////////////////////////////////////////////////////////////////
private:

	struct Element
	{
		size_t tag = NotificationConst::UNUSED_TAG;
		void* pCallback = nullptr;
		Utils::BaseClass* pIdentyfier = nullptr;

		bool isSignedAsToDelete()const
		{
			return pCallback == nullptr;
		}
	};

	/**
	 * If Element::pCallback is nullptr it means that this is remove command :)
	 * If tag != UNUSED_TAG it means that we want remove elements only for such tag
	 * Else we remove all elements for Element::pIdentyfier
	 */
	std::deque<Element> m_changesStack;

	std::vector<std::vector <Element>> m_callbacks;
#ifdef DEBUG
	std::vector<char> m_semaphores;
#endif

	void applyChanges()
	{
		//We wan't save this value because during this operation someone maybe will add/delete something new.
		//This is simply stack of changes. We remove at end and add at front
		int countOfChanges = static_cast<int>( m_changesStack.size() );

		while( --countOfChanges > -1 )
		{
			assert( m_changesStack.empty() == false );
			const Element& elementChanges = m_changesStack.back();

			//It means we want delete it
			if( elementChanges.isSignedAsToDelete() )
			{
				assert( elementChanges.pIdentyfier );

				if( elementChanges.tag == NotificationConst::UNUSED_TAG )
				{
					removeForObject( elementChanges.pIdentyfier );
				}
				else
				{
					removeForTag( elementChanges.pIdentyfier, elementChanges.tag );
				}
			}
			else
			{
				//If it isn't to delete we probably want to add
				if( m_callbacks.size() <= elementChanges.tag )
				{
					m_callbacks.resize( elementChanges.tag + 64 );
#ifdef DEBUG
					m_semaphores.resize( m_callbacks.size() );
#endif
				}

				assert( elementChanges.tag < m_callbacks.size() );
				m_callbacks[elementChanges.tag].emplace_back( elementChanges );
			}

			m_changesStack.pop_back();
		}
	}

	void removeForObject( Utils::BaseClass* const pObject )
	{
#ifdef DEBUG
		unsigned z = 0;
#endif

		for( auto && localVector : m_callbacks )
		{
#ifdef DEBUG
			++z;
#endif

			for( int i = static_cast<int>( localVector.size() ) - 1 ; i > -1; --i )
			{
				Element& element = localVector[i];

				if( pObject == element.pIdentyfier )
				{
#ifdef DEBUG
					assert( z < m_semaphores.size() );
					assert( m_semaphores[z] == 0 );
#endif
					free( element.pCallback );
					element.pCallback = nullptr;
					//order isn't important
					std::swap( localVector[i], localVector.back() );
					localVector.pop_back();
				}
			}
		}
	}

	void removeForTag( Utils::BaseClass* const pObject, size_t tag )
	{
		if( tag >= m_callbacks.size() )
		{
			return;
		}

		assert( tag <  m_callbacks.size() );
#ifdef DEBUG
		assert( tag < m_semaphores.size() );
		assert( m_semaphores[tag] == 0 );
#endif
		auto& localVector = m_callbacks[tag];

		for( int i = static_cast<int>( localVector.size() ) - 1 ; i > -1; --i )
		{
			Element& element = localVector[i];

			if( pObject == element.pIdentyfier )
			{
				free( element.pCallback );
				element.pCallback = nullptr;
				//order isn't important
				std::swap( localVector[i], localVector.back() );
				localVector.pop_back();
			}
		}
	}

	void test_isElementDuplicated( Element& element )
	{
		int balance = 0;//Because we add it already 1 line up

		for( auto && elementLocal : m_changesStack )
		{
			if( elementLocal.pIdentyfier == element.pIdentyfier
					&& ( elementLocal.tag == element.tag
						 || elementLocal.tag == NotificationConst::UNUSED_TAG ) )
			{
				if( elementLocal.isSignedAsToDelete() )
				{
					--balance;

					if( balance < 0 )
					{
						balance = 0;
					}
				}
				else
				{
					++balance;
				}
			}
		}

		assert( balance <= 1 &&
				"You already add this notification it is waiting on changes stack."
				"You should remove your previous listener" );

		//Always first element is a semaphore
		if( element.tag >= m_callbacks.size()
				|| m_callbacks[element.tag].empty() )
		{
			return;
		}

		const auto& localVector = m_callbacks[element.tag];

		for( const Element& elementLocal : localVector )
		{
			if( elementLocal.pIdentyfier == element.pIdentyfier )
			{
				assert( false && "You already add this notification. You should remove your previous listener" );
			}
		}
	}
};

} /* namespace KoalaComponent */
