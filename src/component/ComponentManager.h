/*
 * ComponentManager.h
 *
 *  Created on: Dec 17, 2013
 *      Author: Dawid Drozd
 */

#ifndef COMPONENTMANAGER_H_
#define COMPONENTMANAGER_H_

#include "cocos2d.h"
USING_NS_CC;

#include <vector>
using namespace std;

#include "component/Notifier.h"

namespace KoalaComponent
{

class Component;

inline int _UniqueComponentIdNonDeterministic()
{
	static int id = 1001;
	return ++id;
}

template <typename Type>
inline int getIdForType()
{
	static int id_for_type = _UniqueComponentIdNonDeterministic();
	return id_for_type;
}

/**
 * In first argument passed what component is goiing to be removed.
 * In second argument we will pass tag of the component
 */
MAKE_NOTIFICATION( OnBeforeRemoveFromComponentNode, Component*, int );

class ComponentManager : public CCObject
{
public:
	template<class> friend class ComponentNode;
	friend class Component;
	virtual ~ComponentManager();

	ComponentManager& operator= ( const ComponentManager& ) = delete;
	ComponentManager( const ComponentManager& ) = delete;

	ComponentManager& addComponent( Component* const pComponent );
	ComponentManager& addComponent( Component* const pComponent , const int tag );

	template<typename Type>
	ComponentManager& addComponent( Type* const pComponent )
	{
		return addComponent( pComponent, getIdForType<Type>() );
	}

	void removeComponent( const int tag );

	template<typename Type>
	void removeComponent()
	{
		removeComponent( getIdForType<Type>() );
	}

	void removeComponent( Component* const pComponent );

	void removeAllComponents();

	Component* getComponent( const int tag );

	CCNode* getWorkingNode()const
	{
		assert( m_pWorkingNode != nullptr );
		return m_pWorkingNode;
	}

	Notifier& getNodeNotifier()const
	{
		return m_nodeNotifier;
	}

	void setOwner( Component* pComponentOwner );

	template<typename Type>
	inline Type* getComponent()
	{
		assert( getComponent( getIdForType<Type>() ) == nullptr
				||  dynamic_cast<Type*>( getComponent( getIdForType<Type>() ) ) );
		return static_cast<Type*>( getComponent( getIdForType<Type>() ) );
	}

private:
	CCNode* m_pWorkingNode;
	Notifier& m_nodeNotifier;

	//Minus is keeping proper order but i want cache hit :)
	vector<int> m_componentTags;
	vector<Component*> m_components;

	/**
	 * If m_pOwner == nullptr it means that that this is root manager
	 */
	Component* m_pOwner;

	ComponentManager( CCNode* pWorkingNode, Notifier& pNotifierNode );

	void removeComponentAtPosition( const int index );
};


} /* namespace KoalaComponent */

#endif /* COMPONENTMANAGER_H_ */
