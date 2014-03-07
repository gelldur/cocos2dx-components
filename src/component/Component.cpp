/*
 * Component.cpp
 *
 *  Created on: Dec 17, 2013
 *      Author: Dawid Drozd
 */

#include "component/Component.h"

namespace KoalaComponent
{

Component::Component() :
	m_pOwner ( nullptr )
{
}

Component::~Component()
{
	m_pOwner = nullptr;
}

void Component::initComponent ( ComponentManager& manager )
{
}

void Component::setOwner ( ComponentManager* pManager )
{
	m_pOwner = pManager;
	initComponent ( *pManager );
}

void Component::removeOwner()
{
	m_pOwner = nullptr;
}

} /* namespace KoalaComponent */

