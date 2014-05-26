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

Performance test
===================

Performance tests are disabled in unit_test/NotifierTest.cpp

On Nexus4 (RELEASE)
===================

https://docs.google.com/spreadsheets/d/1EJjdqgEs1rPL4_vh0yUtk88SvsUu5LJyYgcNXynV9Ck/edit?usp=sharing

Final result
```
name	calls	avg ns	min ns	max ns	total ns				
Notifier_1kk_notify_to_1_listener	1000000	0	0	2014345	1025393005				
CCNotifier_1kk_notify_to_1_listener	1000000	0	0	122112009	4568869178	N	N	60.62119895	4.455724933 <--- 4x boost!!!
Notifier_100k_notify_to_1k_listeners	100000	11386	0	2929956	2709415380				
CCNotifier_100k_notify_to_1k_listeners	100000	122081	91561	4822219	14286342287	10.72202705	N	1.645833248	5.272850517 <--- 5x boost!!!
Notifier_100k_notify_to_1k_listener_multiple_notifications	100000	6	0	3509843	1233206157				
CCNotifier_100k_notify_to_1k_listener_multiple_notifications	100000	121682	61041	6683962	12740454771	20280.33333	N	1.904347858	10.33116377 <--- 10x boost!!!
``` 

