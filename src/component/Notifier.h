/*
 * Notifier.h
 *
 *  Created on: Dec 30, 2013
 *      Author: Dawid Drozd
 */

#ifndef NOTIFIER_H__
#define NOTIFIER_H__

#include "cocos2d.h"
USING_NS_CC;

#include <vector>
#include <deque>
#include <typeindex>
#include <cstdlib>
#include <new>
#include <functional>
using namespace std;

#include "utils/Callback.h"

#define MAKE_NOTIFICATION( NAME, args... ) inline const KoalaComponent::Notification<args>& getNotification##NAME(){static const KoalaComponent::Notification<args> variable; return variable;}
#define UNUSED_TAG 0

//TODO 10 posible bug.
//We keep pointer to object that we want remove but sometime object can already die and other object can be at this pointer?

namespace KoalaComponent
{

inline int getUniqueId()
{
	static int id = UNUSED_TAG + 1;
	return ++id;
}

template<typename... Args>
class Notification
{
public:
	Notification() :
		tag( getUniqueId() )
		, hash( typeid( Utils::Callback<void ( Args... )> ).hash_code() )
	{
	}

	Notification& operator= ( const Notification& ) = delete;
	Notification& operator= ( Notification && ) = delete;

	Notification( const Notification& notification ) :
		tag( notification.tag )
		, hash( notification.hash )
	{
	}

	Notification( Notification&& notification ) :
		tag( notification.tag )
		, hash( notification.hash )
	{
	}

	const int tag;
	const size_t hash;
};

template<typename NotificationType>
struct notification_traits;

template<typename... Args>
struct notification_traits<Notification<Args...>>
{
	using callback_type = typename Utils::Callback<void ( Args... )>;
};

//Notifier can't retain listeners because sometimes we want unregister in destructor.
class Notifier
{
public:
	Notifier()
	{
	}
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

		const size_t hashForSpecialization = typeid( CallbackType ).hash_code();
		assert( notification.hash == hashForSpecialization );

		Element element;
		element.hash = hashForSpecialization;
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
	}

	template<typename NotificationType, typename... Args>
	void notify( const NotificationType& notification, Args&& ... params )
	{
		using CallbackType = typename notification_traits<NotificationType>::callback_type;
		assert( notification.tag > UNUSED_TAG );

		applyChanges();

		//Always first element is a semaphore
		if( notification.tag >= m_callbacks.size() || m_callbacks[notification.tag].empty() )
		{
			return;
		}

		assert( notification.tag < m_callbacks.size() );
		const auto& localVector = m_callbacks[notification.tag];

#ifdef DEBUG
		int i = 0;
		//Start working on this vector
		assert( notification.tag < m_semaphores.size() );
		assert( m_semaphores[notification.tag] == false );
		m_semaphores[notification.tag] = true;
#endif

		for( const Element& element : localVector )
		{
			CCAssert( element.pCallback, "You don't set callback?" );
			CCAssert( element.pIdentyfier->retainCount() > 0,
					  "Probably you release object during notification or you simply didn't unregister your previous object. Look for this notification usage" );
			CCAssert( ( notification.tag == element.tag ) ? typeid( CallbackType ).hash_code() ==
					  element.hash :
					  true, "Callback has different params!" );
			static_cast<CallbackType*>( element.pCallback )->call(
				std::forward<Args> ( params )... );

			//Check for use after release
#ifdef DEBUG

			if( i + 1 < localVector.size() )
			{
				++i;
				auto& localElement = localVector[i];

				for( auto && changeElement : m_changesStack )
				{
					if( changeElement.pCallback == nullptr && changeElement.pIdentyfier == localElement.pIdentyfier )
					{
						CCAssert( false,
								  "One of your listeners is removing another during this notification" );
					}
				}
			}

#endif
		}

#ifdef DEBUG
		//Stop working on this vector
		assert( notification.tag < m_semaphores.size() );
		assert( m_semaphores[notification.tag] == true );
		m_semaphores[notification.tag] = false;
#endif
	}

	void removeAllForObject( CCObject* const pObject )
	{
		assert( pObject );
		Element element;
		element.pIdentyfier = pObject;

		//See how do we remove objects it should be faster
		//We add objects at front and remove them on end
		m_changesStack.emplace_front( element );
	}

	template<typename... Args>
	void removeNotification( CCObject* pObject,
							 const Notification<Args...>& notification )
	{
		Element element;
		element.pIdentyfier = pObject;
		element.tag = notification.tag;

		m_changesStack.emplace_front( element );
	}

	/**
	 * For tests only
	 * @param tag of the notification
	 * @return count of listeners
	 */
	size_t getListenersCount( size_t tag )
	{
		if( tag >= m_callbacks.size() )
		{
			return 0;
		}

		return m_callbacks[tag].size();
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
#endif

private:

	struct Element
	{
		Element() :
			hash( 0 )
			, tag( UNUSED_TAG )
			, pCallback( nullptr )
			, pIdentyfier( nullptr )
		{}
		size_t hash;
		size_t tag;
		void* pCallback;
		CCObject* pIdentyfier;
	};

	/**
	 * If Element::pCallback is nullptr it means that this is remove command :)
	 * If tag != UNUSED_TAG it means that we want remove elements only for such tag
	 * Else we remove all elements for Element::pIdentyfier
	 */
	deque<Element> m_changesStack;

	vector<vector <Element>> m_callbacks;
#ifdef DEBUG
	vector<char> m_semaphores;
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
			if( elementChanges.pCallback == nullptr )
			{
				assert( elementChanges.pIdentyfier );

				if( elementChanges.tag == UNUSED_TAG )
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

	void removeForObject( CCObject* const pObject )
	{
#ifdef DEBUG
		unsigned i = 0;
#endif

		for( auto && localVector : m_callbacks )
		{
#ifdef DEBUG
			assert( i < m_semaphores.size() );
			assert( m_semaphores[i] == false );
			++i;
#endif

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
	}

	void removeForTag( CCObject* const pObject, size_t tag )
	{
		if( tag >= m_callbacks.size() )
		{
			return;
		}

		assert( tag >= 0 );
		assert( tag <  m_callbacks.size() );
#ifdef DEBUG
		assert( tag < m_semaphores.size() );
		assert( m_semaphores[tag] == false );
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
};

} /* namespace KoalaComponent */
#undef UNUSED_TAG
#endif /* NOTIFIER_H__ */
