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
	, m_pManager ( nullptr )
{
}

Component::~Component()
{
	m_pOwner = nullptr;
	CC_SAFE_DELETE ( m_pManager );
}

void Component::initComponent ( ComponentManager& manager )
{
}

void Component::setOwner ( ComponentManager* pManager )
{
	m_pOwner = pManager;
	initComponent ( *pManager );
}

} /* namespace KoalaComponent */

