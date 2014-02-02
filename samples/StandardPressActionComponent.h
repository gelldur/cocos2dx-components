/*
 * StandardPressActionComponent.h
 *
 *  Created on: Jan 30, 2014
 *      Author: Dawid Drozd
 */

#ifndef STANDARDPRESSACTIONCOMPONENT_H_
#define STANDARDPRESSACTIONCOMPONENT_H_

#include "component/Component.h"
using namespace KoalaComponent;
namespace KoalaJump
{

class StandardPressActionComponent : public Component
{
public:
	//KK_CREATE_METHOD is similar to CREATE_FUNC but we don't call init()
	KK_CREATE_METHOD ( StandardPressActionComponent );
	virtual ~StandardPressActionComponent();

	/**
	 * Initialize your component
	 * @param manager manager that is owner of this Component
	 */
	virtual void initComponent ( ComponentManager& manager ) override;

private:
	void resetView();

	void onPress ( );
	void onUnPress ( );

	StandardPressActionComponent();
	enum ID
	{
		BUMP_ACTION = 2561, RESET_ACTION
	};
};

} /* namespace KoalaJump */

#endif /* STANDARDPRESSACTIONCOMPONENT_H_ */
