#include "KKDebugTouch.h"
#include "cocos2d.h"

USING_NS_CC;

KKDebugTouch* KKDebugTouch::instance = 0;

KKDebugTouch* KKDebugTouch::getInstance()
{
    if (instance == 0) {
        instance = new KKDebugTouch();
    }
    return instance;
}

KKDebugTouch::KKDebugTouch()
{
    CCLOG("Initializing KKDebugTouch");
    this->_lastStopper = NULL;
    this->_lastSwallower = NULL;
}

KKDebugTouch::~KKDebugTouch()
{
    CCLOG("Destroying KKDebugTouch");
}

void KKDebugTouch::setPZLRef(cocos2d::LayerColor* l)
{
    this->_pzlRef = l;
}

void KKDebugTouch::setLastStopper(cocos2d::EventListener* l)
{
    this->_lastStopper = l;
    l->retain();
}

void KKDebugTouch::clearLastStopper()
{
    if (this->_lastStopper)
        this->_lastStopper->release();
    this->_lastStopper = NULL;
}

void KKDebugTouch::setLastSwallower(cocos2d::EventListener* l, cocos2d::Touch*)
{
    if (this->_lastSwallower) {
        this->_lastSwallower->release();
    }
    l->retain();
    this->_lastSwallower = l;
}

void KKDebugTouch::addEvent(int set, cocos2d::EventTouch* e, const std::vector<Touch *> &e_touches)
{
    //const std::vector<Touch*>& e_touches = e->getTouches();
    //cocos2d::Vector <cocos2d::EventTouch*> *events = &this->_event_sets[set];
    cocos2d::Vector <cocos2d::Touch*> *touches = &this->_touch_sets[set];
    CCLOG("++ Adding event %x ",e);
    for (auto it : e_touches) {
        CCLOG("    + event %x touch %x id %i", e, it, it->getID());
        touches->pushBack(it);
    }
    CCLOG("++");
}

void KKDebugTouch::removeEvent(int set, cocos2d::EventTouch* e, const std::vector<Touch *> &e_touches)
{
    //cocos2d::Vector <cocos2d::EventTouch*> *events = &this->_event_sets[set];
    //const std::vector<Touch*>& e_touches = e->getTouches();
    cocos2d::Vector <cocos2d::Touch*> *touches = &this->_touch_sets[set];
    CCLOG("-- Removing event %x ",e);
    for (auto eit : e_touches) {
        for (auto it : *touches)
        {
            if (it == eit) {
                touches->eraseObject(it);
                CCLOG("    - event %x touch %x id %i", e, it, it->getID());
            }
        }
    }
    CCLOG("--");
}

void KKDebugTouch::procEvent(int set, cocos2d::EventTouch* e)
{
    // called from the end of the dispatcher
    // if a end / cancel event, checks that the touches were removed from the list
    // or reports an error.
    //cocos2d::Vector <cocos2d::EventTouch*> *events = &this->_event_sets[set];
    const std::vector<Touch*>& e_touches = e->getTouches();
    cocos2d::Vector <cocos2d::Touch*> *touches = &this->_touch_sets[set];
    EventTouch::EventCode eventCode = e->getEventCode();
    switch (eventCode)
    {
        case EventTouch::EventCode::ENDED:
        case EventTouch::EventCode::CANCELLED:    
            for (auto eit : e_touches) {
                for (auto it : *touches)
                {
                    if (it == eit) {
                        CCLOG("++++ EVENT TOUCH STILL IN SET SET %i : %x", set, e);
                    }
                }
            }
            break;
    }
}

