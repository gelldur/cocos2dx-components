/*
 * ComponentNode.h
 *
 *  Created on: Dec 27, 2013
 *      Author: dawid
 */

#ifndef __COMPONENTNODE_H_
#define __COMPONENTNODE_H_

#include <type_traits>
#include "cocos2d.h"

#include "component/ComponentManager.h"

namespace KoalaComponent
{

MAKE_NOTIFICATION ( OnEnter );
MAKE_NOTIFICATION ( OnExit );
MAKE_NOTIFICATION ( OnUpdateComponentNode, float );
MAKE_NOTIFICATION ( OnVisitComponentNode, Utils::Callback<void () >, bool* );

template <typename BaseClazz>
class ComponentNode : public BaseClazz
{
private:
	typedef BaseClazz inherited;
public:

	static ComponentNode* create()
	{
		static_assert ( std::is_base_of<cocos2d::CCNode, BaseClazz>::value,
						"ComponentNode can be only created with BaseClazz derived from CCNode" );
		ComponentNode* pRet = new ComponentNode();

		if ( pRet )
		{
			pRet->autorelease();
			return pRet;
		}

		CC_SAFE_DELETE ( pRet );
		return pRet = nullptr;
	}

	virtual ~ComponentNode()
	{
#ifdef DEBUG
		assert ( m_pNotifier.isWorking == false );
#endif
	}

	/**
	 * If you want add here overriden methods
	 */

	virtual void onEnter() override
	{
		inherited::onEnter();
		m_pNotifier.notify ( getNotificationOnEnter() );
	}

	virtual void onExit() override
	{
		inherited::onExit();
		m_pNotifier.notify ( getNotificationOnExit() );
	}

	virtual void update ( float dt ) override
	{
		inherited::update ( dt );
		m_pNotifier.notify ( getNotificationOnUpdateComponentNode(), dt );
	}

	ComponentManager& getComponentManager()
	{
		return m_manager;
	}

	Notifier& getNotifier()
	{
		return m_pNotifier;
	}

protected:
	ComponentNode() :
		m_manager ( this, m_pNotifier )
	{
	}

private:
	Notifier m_pNotifier;
	ComponentManager m_manager;
};

} /* namespace KoalaComponent */

#endif /* __COMPONENTNODE_H_ */
