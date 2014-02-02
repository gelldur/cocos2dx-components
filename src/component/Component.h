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
		assert ( m_pOwner );
		return *m_pOwner;
	}


	CCNode* getWorkingNode()
	{
		assert ( m_pOwner );
		assert ( m_pOwner->getWorkingNode() );
		return m_pOwner->getWorkingNode();
	}

	ComponentManager& getManager()
	{
		if ( m_pManager == nullptr )
		{
			//Lazyload
			m_pManager = new ComponentManager ( getWorkingNode(),
												getOwner().getNodeNotifier() );
			m_pManager->setOwner ( this );
		}

		assert ( m_pManager );
		return *m_pManager;
	}

	virtual void initComponent ( ComponentManager& manager );

private:
	ComponentManager* m_pOwner;
	ComponentManager* m_pManager;
	void setOwner ( ComponentManager* pManager );
};

} /* namespace KoalaComponent */

#endif /* KOALA_COMPONENT_H_ */
