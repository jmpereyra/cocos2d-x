#ifndef KKDebugTouch_h
#define KKDebugTouch_h

#include "base/CCVector.h"
#include "2d/CCLayer.h"
#include "base/CCEventTouch.h"
#include "base/CCEventListenerTouch.h"

class KKDebugTouch
{
 public:
    static KKDebugTouch* getInstance();

    void addEvent(int set, cocos2d::EventTouch* e, const std::vector<cocos2d::Touch *> &e_touches);
    void removeEvent(int set, cocos2d::EventTouch* e, const std::vector<cocos2d::Touch *> &e_touches);
    void procEvent(int set, cocos2d::EventTouch* e);
    void setPZLRef(cocos2d::LayerColor* l);
    void setLastStopper(cocos2d::EventListener* l);
    void clearLastStopper();
    void setLastSwallower(cocos2d::EventListener* l, cocos2d::Touch*);
    
 private:
    static KKDebugTouch* instance;
    KKDebugTouch();
    ~KKDebugTouch();

    //cocos2d::Vector<cocos2d::EventTouch*> _event_sets[5];
    cocos2d::Vector<cocos2d::EventTouch*> _events_sets[5];
    cocos2d::Vector<cocos2d::Touch*>      _touch_sets[5];
    cocos2d::LayerColor*                  _pzlRef;
    cocos2d::EventListener*               _lastStopper;
    cocos2d::EventListener*               _lastSwallower;
};
#endif
