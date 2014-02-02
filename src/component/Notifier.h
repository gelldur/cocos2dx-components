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

namespace KoalaComponent
{

inline int getUniqueId()
{
	static int id = 1;
	return ++id;
}

template< typename... Args>
class Notification
{
public:
	Notification() :
		tag ( getUniqueId() )
		, hash ( typeid ( Utils::Callback<void ( Args... ) > ).hash_code() )
	{
	}

	Notification& operator= ( const Notification& ) = delete;
	Notification ( const Notification& ) = delete;
	Notification& operator= ( Notification && ) = delete;
	Notification ( Notification&& ) = delete;

	const int tag;
	const size_t hash;
};

class Notifier
{
public:
	Notifier() {}
	virtual ~Notifier()
	{
		for ( auto && localVector : m_callbacks )
		{
			for ( auto && element : localVector )
			{
				free ( element.pCallback );
			}
		}

		m_callbacks.clear();

		for ( const Element& element : m_changesStack )
		{
			free ( element.pCallback );
		}


		m_changesStack.clear();
	}

	Notifier& operator= ( const Notifier& ) = delete;
	Notifier ( const Notifier& ) = delete;
	Notifier& operator= ( Notifier && ) = delete;
	Notifier ( Notifier&& ) = delete;

	static Notifier& getGlobal()
	{
		static Notifier globalNotifier;
		return globalNotifier;
	}

	template<typename... Args>
	void addNotification ( const Notification<Args...>& notification,
						   const Utils::Callback<void ( Args... ) >& callback )
	{
		typedef Utils::Callback<void ( Args... ) > CallbackType;
		CCAssert ( callback.isCallable(), "Callback isn't set" );
		CCAssert ( callback.getObject(), "Notifier is't ready for lambdas" );

		const size_t hashForSpecialization = typeid ( CallbackType ).hash_code();
		assert ( notification.hash == hashForSpecialization );

		Element element;
		element.hash = hashForSpecialization;
		element.tag = notification.tag;

		const size_t sizeOfCallback = sizeof ( CallbackType );
		CallbackType* pCallback = static_cast<CallbackType*> ( malloc (
									  sizeOfCallback ) );
		//Placement new
		new ( pCallback ) CallbackType ( callback );

		element.pCallback = pCallback;
		element.pIdentyfier = callback.getObject();

		//See how do we remove objects it should be faster
		m_changesStack.emplace_front ( element );
	}

	template<typename... Args>
	void notify ( const Notification<Args...>& notification, Args... params )
	{
		typedef Utils::Callback<void ( Args... ) > CallbackType;
		const size_t hashForSpecialization = typeid ( CallbackType ).hash_code();

		applyChanges();

		auto iterator = m_indexMap.find ( notification.tag );

		if ( iterator == m_indexMap.end() )
		{
			return;
		}

		const int index = iterator->second;
		assert ( index >= 0 );
		assert ( index < m_callbacks.size() );
		const auto& localVector = m_callbacks[index];
		const int localTag = notification.tag;

		for ( const Element& element : localVector )
		{
			CCAssert ( ( localTag == element.tag ) ? hashForSpecialization == element.hash :
					   true,
					   "Callback has different params!" );
			static_cast<CallbackType*> ( element.pCallback )->call ( params... );
		}
	}

	void removeAllForObject ( CCObject* const pObject )
	{
		assert ( pObject );
		Element element;
		element.pIdentyfier = pObject;

		//See how do we remove objects it should be faster
		//We add objects at front and remove them on end
		m_changesStack.emplace_front ( element );
	}

private:

	struct Element
	{
		Element() :
			hash ( 0 )
			, tag ( 0 )
			, pCallback ( nullptr )
			, pIdentyfier ( nullptr )
		{}
		size_t hash;
		size_t tag;
		void* pCallback;
		CCObject* pIdentyfier;
	};

	//If Element.pCallback == nullptr that mean
	// Element.pIdentyfier is to remove
	deque<Element> m_changesStack;
	/**
	 * unique id - index in m_callbacks
	 */
	map<size_t, int> m_indexMap;
	vector< vector < Element > > m_callbacks;

	void applyChanges()
	{
		//We wan't save this value because during this operation someone maybe will add/delete something new.
		//This is simply stack of changes. We remove at end and add at front
		int countOfChanges = static_cast<int> ( m_changesStack.size() );

		while ( --countOfChanges > -1 )
		{
			assert ( m_changesStack.empty() == false );
			const Element& elementChanges = m_changesStack.back();

			//It means we want delete it
			if ( elementChanges.pCallback == nullptr )
			{
				assert ( elementChanges.pIdentyfier );
				removeForObject ( elementChanges.pIdentyfier );
			}
			else
			{
				//If it isn't to delete we probably want to add
				auto iterator = m_indexMap.find ( elementChanges.tag );

				if ( iterator == m_indexMap.end() )
				{
					m_callbacks.emplace_back ( vector<Element>() );
					iterator = m_indexMap.emplace_hint ( m_indexMap.end(),
														 std::make_pair ( elementChanges.tag, m_callbacks.size() - 1 ) );
				}

				const int index = iterator->second;
				assert ( index < ( int ) m_callbacks.size() );
				auto& localVector = m_callbacks[index];
				localVector.emplace_back ( elementChanges );
			}

			m_changesStack.pop_back();
		}
	}

	void removeForObject ( CCObject* const pObject )
	{
		for ( auto& localVector : m_callbacks )
		{
			for ( int i = static_cast<int> ( localVector.size() ) - 1 ; i > -1; --i )
			{
				Element& element = localVector[i];

				if ( pObject == element.pIdentyfier )
				{
					free ( element.pCallback );
					element.pCallback = nullptr;
					//order isn't important
					std::swap ( localVector[i], localVector.back() );
					localVector.pop_back();
				}
			}
		}
	}
};

} /* namespace KoalaComponent */

#endif /* NOTIFIER_H__ */
