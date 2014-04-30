/*
 * ClickComponent.h
 *
 *  Created on: Dec 17, 2013
 *      Author: Dawid Drozd
 */

#ifndef CLICKCOMPONENT_H_
#define CLICKCOMPONENT_H_

#include "component/Component.h"
using namespace KoalaComponent;
namespace KoalaJump
{

/**
 * This component is responsible for detecting click event on Owner.
 * We can pass here callback for clickOnNode also margin for click.
 *
 * We use here TouchManager for detecting TouchManager::detectTouchOnNode
 */
class ClickComponent : public Component, public CCTouchDelegate
{
public:
	static ClickComponent* create( const Callback<void ( CCNode* )>& callback,
								   const CCSize& touchMargin = CCSizeZero )
	{
		ClickComponent* pRet = new ClickComponent( callback, touchMargin );

		if( pRet )
		{
			pRet->autorelease();
			return pRet;
		}

		CC_SAFE_DELETE( pRet );
		return pRet = nullptr;
	}

	virtual ~ClickComponent();

	/**
	 * Initialize your component
	 * @param manager manager that is owner of this Component
	 */
	virtual void initComponent( ComponentManager& manager ) override;

	/**
	 * Callback for onEnter
	 */
	void onEnter();

	/**
	 * Callback for onExit
	 */
	void onExit();

	void onTouchBegan( CCTouch* pTouch, bool* pReturnValue );
	void onTouchEnded( CCTouch* pTouch );

	/**
	 * Margin for touch it is used by TouchManager::detectTouchOnNode simply if your
	 * Node has content size eg. 20 x 20 and you pass here margin (5,5) touch will be detected
	 * on 30 x 30 square (add 5 to each side of node)
	 * @param margin
	 */
	void setMarginForClick( const CCSize&  margin );

	/**
	 * @param callback callback for notification that we clicked Node. If callback isn't callable we won't call it
	 */
	void setOnClickCallback( Callback<void ( CCNode* )> callback );

private:
	CCSize m_marginForTouch;
	Callback<void ( CCNode* )> m_callback;

	ClickComponent( const Callback<void ( CCNode* )>& callback,
					const CCSize& touchMargin = CCSizeZero );
};

} /* namespace KoalaJump */

#endif /* CLICKCOMPONENT_H_ */
