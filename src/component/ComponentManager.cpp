/*
 * ComponentManager.cpp
 *
 *  Created on: Dec 17, 2013
 *      Author: Dawid Drozd
 */

#include "component/ComponentManager.h"
#include "component/Component.h"

#define UNUSED_TAG -1

namespace KoalaComponent
{

ComponentManager::ComponentManager ( CCNode* pWorkingNode ,
									 Notifier& pNotifierNode ) :
	m_pWorkingNode ( pWorkingNode )
	, m_nodeNotifier ( pNotifierNode )
	, m_pOwner ( nullptr )
{
	m_componentTags.reserve ( 16 );
	m_components.reserve ( 16 );
}

ComponentManager::~ComponentManager()
{
	removeAllComponents();
}

void ComponentManager::addComponent ( Component* const pComponent )
{
	addComponent ( pComponent, UNUSED_TAG );
}

void ComponentManager::addComponent ( Component* const pComponent ,
									  const int tag )
{
#ifdef DEBUG

	for ( auto && tagAdded : m_componentTags )
	{
		if ( tagAdded == tag && tag != UNUSED_TAG )
		{
			CCAssert ( false, "You can't add component with the same tag again." );
		}
	}

	for ( auto pElement : m_components )
	{
		if ( pElement == pComponent )
		{
			CCAssert ( false, "This component was already added" );
		}
	}

#endif
	m_componentTags.push_back ( tag );
	m_components.push_back ( pComponent );
	pComponent->retain();

	pComponent->setOwner ( this );
}

void ComponentManager::removeAllComponents()
{
	m_componentTags.clear();

	//We dpnt want situation tahat one compoenent can remove other component or even add new one
	for ( auto* pElement : m_components )
	{
		pElement->release();
	}
}

void ComponentManager::removeComponent ( const int tag )
{
	CCAssert ( tag != UNUSED_TAG,
			   "can't delete component with this kind of tag" );
	int i = 0;

	for ( auto && componentTag : m_componentTags )
	{
		if ( componentTag == tag )
		{
			removeComponentAtPosition ( i );
			return;
		}

		++i;
	}
}

void ComponentManager::removeComponent ( Component* const pComponent )
{
	CCAssert ( pComponent != nullptr,
			   "pComponent can't be null" );
	int i = 0;

	for ( Component* const pElement : m_components )
	{
		if ( pElement == pComponent )
		{
			removeComponentAtPosition ( i );
			return;
		}

		++i;
	}
}

Component* ComponentManager::getComponent ( int tag )
{
	CCAssert ( tag != UNUSED_TAG,
			   "can't find component with this kind of tag" );
	int i = 0;

	for ( int componentTag : m_componentTags )
	{
		if ( componentTag == tag )
		{
			return m_components[i];
		}

		++i;
	}

	return nullptr;
}

void ComponentManager::removeComponentAtPosition ( const int index )
{
	assert ( index < ( int ) m_components.size() );
	Component* pComponent = m_components[index];

	m_nodeNotifier.notify ( getNotificationOnBeforeRemoveFromComponentNode(), pComponent );

	std::swap ( m_components[index], m_components.back() );
	std::swap ( m_componentTags[index], m_componentTags.back() );
	m_components.pop_back();
	m_componentTags.pop_back();

	m_nodeNotifier.removeAllForObject ( pComponent );

	pComponent->release();
}

void ComponentManager::setOwner ( Component* pComponentOwner )
{
	assert ( m_pOwner == nullptr );
	m_pOwner = pComponentOwner;
}


} /* namespace KoalaComponent */

