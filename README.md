cocos2dx-components
===================

On <b>MIT<b> license

Components for cocos2dx

<b><h1>Hierarchy graph<b>

![alt tag](https://raw.github.com/gelldur/cocos2dx-components/master/Diagram.jpg)

Example use
===================

```cpp
ComponentNode<CCSprite>* pSprite = ComponentNode<CCSprite>::create();

pSprite->initWithFile ( R::Sprite::GUI_BUTTON_GREEN_9 );

pSprite->getComponentManager().addComponent ( TouchComponent::create() );
pSprite->getComponentManager().addComponent ( StandardPressActionComponent::create() );
pSprite->getComponentManager().addComponent (ClickComponent::create ( Utils::makeCallback ( this, &AchievementRow::onSkipClickedCallback ) ) );

//Or another way
TouchComponent* pTouch = TouchComponent::create();
ClickComponent* pClick = ClickComponent::create ( Utils::makeCallback ( this, &AchievementRow::onSkipClickedCallback ) );
StandardPressActionComponent* pPressAnimation = StandardPressActionComponent::create();


pTouch->getManager().addComponent( pClick );
pClick->getManager().addComponent( pPressAnimation );

pSprite->getComponentManager().addComponent ( pTouch );

/**
What is under hood
Now when TouchComponent recives touches he notify about them to NotificationCenter and ClickComponent decides is this pSprite (Owner) is being clicked. 
ClickComponent sends notification that we have click start so StandardPressActionComponent start animation of pressing button.
**/
```

You can see example components in /sample (snippets)
 - TouchComponent
 - ClickComponent
 - StandardPressActionComponent

More about callbacks you can read here: http://www.cocos2d-x.org/forums/6/topics/41358?r=43554#message-43554

How to create notifications
===================
Simply use macro

```cpp
MAKE_NOTIFICATION ( OnTouchBegan, CCTouch*, bool* );
MAKE_NOTIFICATION ( OnTouchEnded, CCTouch* );
MAKE_NOTIFICATION ( OnTouchMoved, CCTouch* );
MAKE_NOTIFICATION(OnPress);
MAKE_NOTIFICATION(OnUnPress);
```

Where first argument is name for notification. Automaticly macro will add something like "getNotification" as prefix so for example

getNotificationOnTouchBegan();
getNotificationOnPress();

rest of arguments are vargs that will be used for notifications. Types are really important here.

Look at Notifier.h for implementation.

