/*
 * Component.h
 *
 *  Created on: Dec 17, 2013
 *      Author: Dawid Drozd
 */

#ifndef KOALA_COMPONENT_H_
#define KOALA_COMPONENT_H_

#include "cocos2d.h"
#include "utils/Callback.h"
#include "component/ComponentManager.h"
using namespace Utils;

namespace KoalaComponent
{

class Component : public cocos2d::CCObject
{
public:
	friend class ComponentManager;
	Component();
	virtual ~Component();

	ComponentManager& getOwner()
	{
		assert( getWorkingNode() );
		assert( m_pOwner != nullptr );
		return *m_pOwner;
	}


	CCNode* getWorkingNode()
	{
		assert( m_pOwner );
		assert( m_pOwner->getWorkingNode() );
		return m_pOwner->getWorkingNode();
	}

	bool isOwned() const
	{
		return m_pOwner != nullptr;
	}

	virtual void initComponent( ComponentManager& manager );

private:
	ComponentManager* m_pOwner;
	void setOwner( ComponentManager* pManager );
	void removeOwner();
};

} /* namespace KoalaComponent */

#endif /* KOALA_COMPONENT_H_ */
