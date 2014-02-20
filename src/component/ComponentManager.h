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

/**
 * In first argument passed what component is goiing to be removed.
 */
MAKE_NOTIFICATION ( OnBeforeRemoveFromComponentNode, Component* );

class ComponentManager : public CCObject
{
public:
	template<class> friend class ComponentNode;
	friend class Component;
	virtual ~ComponentManager();

	ComponentManager& operator= ( const ComponentManager& ) = delete;
	ComponentManager ( const ComponentManager& ) = delete;

	void addComponent ( Component* const pComponent );
	void addComponent ( Component* const pComponent , const int tag );
	void removeComponent ( const int tag );
	void removeComponent ( Component* const pComponent );

	void removeAllComponents();

	Component* getComponent ( const int tag );

	CCNode* getWorkingNode()
	{
		assert ( m_pWorkingNode != nullptr );
		return m_pWorkingNode;
	}

	Notifier& getNodeNotifier()
	{
		return m_nodeNotifier;
	}

	/**
	 * @return true if this is root component manager
	 */
	bool isRoot() const;
	void setOwner ( Component* pComponentOwner );

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

	ComponentManager ( CCNode* pWorkingNode, Notifier& pNotifierNode );

	void removeComponentAtPosition ( const int index );
};


} /* namespace KoalaComponent */

#endif /* COMPONENTMANAGER_H_ */
