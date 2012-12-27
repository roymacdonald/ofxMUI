/*==============================================================================
 
 Copyright (c) 2009-2012 Christopher Baker <http://christopherbaker.net>
 
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 
 ==============================================================================*/

#include "ofxMuiObject.h"

//--------------------------------------------------------------
ofxMuiObject::ofxMuiObject(/*const string& _name, */float _x, float _y, float _width, float _height, bool _enabled) :
    ofxMuiBox(_x,_y,_width,_height), 
    ofxMuiEnabler(_enabled), 
ofxMuiKeyMapper(){//,
//    name(_name) {
        init();
}

//--------------------------------------------------------------
ofxMuiObject::ofxMuiObject(/*const string& _name, */ float _x, float _y, bool _enabled) : 
    ofxMuiBox(_x,_y), 
    ofxMuiEnabler(_enabled), 
ofxMuiKeyMapper(){//,
//    name(_name) {
        init();
}
//--------------------------------------------------------------
ofxMuiObject::ofxMuiObject(/*const string& _name, */bool _enabled) : 
    ofxMuiBox(), 
    ofxMuiEnabler(_enabled), 
ofxMuiKeyMapper() {//,
//    name(_name) {
        init();
}

//--------------------------------------------------------------
ofxMuiObject::~ofxMuiObject() {
    // clear and delete any child objects
    for(int i = 0; i < childObjects.size(); i++) delete childObjects[i];
    childObjects.clear();
}

//--------------------------------------------------------------
void ofxMuiObject::init() {
    defaults = &ofxMuiDefaults::getInstance(); // get a link to the singleton
    
    //tooltip             = name;
    //bTooltipEnabled      = false;
    
	parent              = NULL;

	_isTouchDown        = false;
	_isTouchOver        = false;
	
	// drag'n'drop
	_isDragMoveable		= false;
	_isDragging			= false;
    
    _isHitDragable   = true;
    _isHitDragOrigin = false;

    
    _isSelectable       = false;
    _isSelectDragable   = false;

    
	_isDropSender		= false; // can we drop this thing on something
	_isDropReceiver		= false; // can we receive a dropped object
	
    _isFileDropReceiver  = false;
    
    // set vars from static members
    cBox                = defaults->cBox;
    cBorder             = defaults->cBorder;
    cPadding            = defaults->cPadding;
    cContent            = defaults->cContent;
    cHit                = defaults->cHit;
    
    cText               = defaults->cText;

    cActiveAreaForeground = defaults->cActiveAreaForeground;
    cActiveAreaBackground = defaults->cActiveAreaBackground;
    cActiveAreaFrame      = defaults->cActiveAreaFrame;
    
    cShadow             = defaults->cShadow;

    doShadow            = defaults->doShadow;
    shadowOffset        = defaults->shadowOffset;
    
    alphaScale          = defaults->alphaScale;
    
    keyboardOnPress     = defaults->keyboardOnPress;
    
    objectType          = "BASE_OBJECT";
    
    orientation         = OF_ORIENTATION_DEFAULT;
    
    isActiveTimer       = 0.0f;
    isActiveTimerRate   = 0.05f;
            
}


//--------------------------------------------------------------

void ofxMuiObject::_setup(ofEventArgs &e) {
	if(!isEnabled()) return;
    
    // propagate setup
    if(propagateSetupEvents()) {
        for(int i = 0; i < childObjects.size(); i++) {
            ofxMuiPushObject(childObjects[i]);
            childObjects[i]->_setup(e);
            ofxMuiPopObject();
        }
    }
    
    // do local setup
	if(isSetupHandler()) {
        setup();
    }
}


//--------------------------------------------------------------

void ofxMuiObject::_update(ofEventArgs &e) {
    if(!isEnabled()) return;
	
    
    for(int i = 0; i < childObjects.size(); i++) {
        // update child (this will resize its frame if needed)
        ofxMuiPushObject(childObjects[i]);
        
        childObjects[i]->_update(e);
        if(childObjects[i]->getParentNeedsLayoutUpdate()) {
            needsLayout = true;
        }
        
        ofxMuiPopObject();
    }
	
    // TODO: do layout changes always happen? 
    // resize the local frames, propagate layout requests
    if(needsLayout) {
        isLayoutSetupComplete = true;
        doLayout(); // will set to false
    }
    
    if(isUpdateHandler()) {
        // do the local update
        if (isTouchOver() || isTouchDown() || isDragging()) {
            isActiveTimer += isActiveTimerRate;
        } else {
            isActiveTimer -= isActiveTimerRate;
        }
        
        isActiveTimer = ofClamp(isActiveTimer,0,1);
        
        update();
    }

}

//--------------------------------------------------------------
void ofxMuiObject::_draw(ofEventArgs &e) {
    
    
    //ofxMuiDebugString(this, "DRAW-> " + this->getName() + " isHidden()=" + ofToString(isHidden()));
    
	if(!isEnabled() || isHidden()) return;

    ofPushStyle();
    ofPushMatrix();
    {
        ofTranslate(getPosition());
        
        if(isDrawHandler()) {

            ////////////////////////////////////////////////////////
            // DRAW THE BOX / MARGIN ///////////////////////////////
            ////////////////////////////////////////////////////////
            
            drawBox();

            ////////////////////////////////////////////////////////
            // DRAW THE BORDER BOX /////////////////////////////////
            ////////////////////////////////////////////////////////
            
            ofPushMatrix();
            {
                ofTranslate(getBorderBoxOffset());
                drawShadowBox();
                drawBorder();
            }
            ofPopMatrix();
            
            ////////////////////////////////////////////////////////
            // DRAW THE PADDING BOX ////////////////////////////////
            ////////////////////////////////////////////////////////

            ofPushMatrix();
            {
                ofTranslate(getPaddingBoxOffset());
                drawPaddingBox();
            }
            ofPopMatrix(); 
        
            ////////////////////////////////////////////////////////
            // DRAW THE CONTENT BOX ////////////////////////////////
            ////////////////////////////////////////////////////////
            
            ofPushMatrix();
            {
                ofTranslate(getContentBoxOffset());
                drawContentBox();
            }
            ofPopMatrix();
        }
        
        if(propagateDrawEvents()) {
            ofPushMatrix();
            {
                ofTranslate(getContentBoxOffset());
                
                ////////////////////////////////////////////////////////
                // SET UP CONTENT BOX CLIPPING IF REQUESTED ////////////
                ////////////////////////////////////////////////////////
                if(defaults->enableContentBoxClipping) {
                    ofVec2f sp = getContentBoxScreenPosition();
                    glEnable(GL_SCISSOR_TEST);
                    glScissor(sp.x-2, sp.y-2, getContentBoxWidth()+3,getContentBoxHeight()+3);
                }
                ////////////////////////////////////////////////////////
                // DRAW THE CONTENT ITSELF /////////////////////////////
                ////////////////////////////////////////////////////////
                draw(); ////////////////////////////////////////////////
                ////////////////////////////////////////////////////////
                // SET UP CONTENT BOX CLIPPING IF REQUESTED ////////////
                ////////////////////////////////////////////////////////
                if(defaults->enableContentBoxClipping) {
                    glDisable(GL_SCISSOR_TEST);
                }
                ////////////////////////////////////////////////////////
                
                for(int i = 0; i < childObjects.size(); i++) {
                    ofxMuiPushObject(childObjects[i]);
                    childObjects[i]->_draw(e);
                    ofxMuiPopObject();
                }
                
            }
        }
        
        ofPopMatrix();
    }
    ofPopMatrix();
    ofPopStyle();

}

//--------------------------------------------------------------
void ofxMuiObject::_exit(ofEventArgs &e) {
    // always propagated.  always executed
    
    for(int i = 0; i < childObjects.size(); i++) {
        ofxMuiPushObject(childObjects[i]);
        childObjects[i]->_exit(e);
        ofxMuiPopObject();
    }

	exit();
}

//--------------------------------------------------------------
void ofxMuiObject::_windowResized(ofResizeEventArgs &e) {
    // always propagated
    if(propagateWindowResizedEvents()) {
        for(int i = 0; i < childObjects.size(); i++) { 
            ofxMuiPushObject(childObjects[i]);
            childObjects[i]->_windowResized(e);
            ofxMuiPopObject();
        }
    }
        
    if(isWindowResizedHandler()) {
        windowResized(e);
    }
}

//--------------------------------------------------------------
ofxMuiObject*  ofxMuiObject::_fileDragEvent(ofDragInfo &e, ofxMuiObject* handled) {
//    vector <string> files;
//    ofPoint position;

	ofxMuiObject* newHandler = NULL;
    
    // always propagated ... (?)
    if(propagateFileDragEvents()) {
        for(int i = 0; i < childObjects.size(); i++) {
            ofxMuiPushObject(childObjects[i]);
            newHandler = childObjects[i]->_fileDragEvent(e, handled);
            ofxMuiPopObject();
            if(!handled && newHandler) {
                handled = newHandler;
            }
        }
    }
    
    if(isFileDragHandler() && canHandleIt(handled) && hitTest(e.position)) {
        fileDragEvent(e);
        handled = this;
    } 
    
    return handled;
}

//--------------------------------------------------------------
void ofxMuiObject::_gotMessage(ofMessage &e) {
    // always propagated (?)
    
    if(propagateMessageEvents()) {
        for(int i = 0; i < childObjects.size(); i++) {
            ofxMuiPushObject(childObjects[i]);
            childObjects[i]->_gotMessage(e);
            ofxMuiPopObject();
        }
    }
    
    gotMessage(e);
}

//--------------------------------------------------------------

ofxMuiObject* ofxMuiObject::_keyPressed(ofKeyEventArgs &e, ofxMuiObject* handled) {
	if(!isEnabled() || !keyboardOnPress || isHidden()) return NULL;
    
	ofxMuiObject* newHandler = NULL;
	
    saveKeyboardEvent(e);

    if(propagateKeyboardEvents()) {
        for(int i = (childObjects.size()-1); i >= 0; i--) {
            ofxMuiPushObject(childObjects[i]);
            newHandler = childObjects[i]->_keyPressed(e, handled);
            ofxMuiPopObject();
            // currently an event can only be handled by one item
            if(!handled && newHandler) {
                handled = newHandler;
            }
        }
    }
    
    if(isKeyboardHandler() && canHandleIt(handled) && executeKeyMappedFunc(key)) {
        onKeyPressed(key); // just an alert (should be earlier?)
        handled = this;
    }

	
	return handled;
}

//--------------------------------------------------------------

ofxMuiObject* ofxMuiObject::_keyReleased(ofKeyEventArgs &e, ofxMuiObject* handled) {
	if(!isEnabled() || keyboardOnPress || isHidden()) return NULL;
    
	ofxMuiObject* newHandler = NULL;
	
    saveKeyboardEvent(e);
    
    if(propagateKeyboardEvents()) {
        for(int i = (childObjects.size()-1); i >= 0; i--) {
            ofxMuiPushObject(childObjects[i]);
            newHandler = childObjects[i]->_keyReleased(e, handled);
            ofxMuiPopObject();
            // currently an event can only be handled by one item
            if(!handled && newHandler) {
                handled = newHandler;
            }
        }
    }
	
    if(isKeyboardHandler() && canHandleIt(handled) && executeKeyMappedFunc(key)) {
        onKeyReleased(key);
        handled = this;
    }
	
	return handled;
}

//--------------------------------------------------------------

ofxMuiObject* ofxMuiObject::_mouseMoved(ofMouseEventArgs &e, ofxMuiObject* handled) {
    
	if(!isEnabled() || isHidden()) return NULL;

    saveMouseEvent(e);
    
    if(propagateTouchEvents()) {
        ofxMuiObject* newHandler = NULL;
        for(int i = (childObjects.size()-1); i >= 0; i--) {
            ofxMuiPushObject(childObjects[i]);
            newHandler = childObjects[i]->_mouseMoved(e, handled);
            ofxMuiPopObject();
            // currently an event can only be handled by one item
            if(!handled && newHandler) {
                handled = newHandler;
            }
        }
    }
	
    bool didHit = hitTest(mousePosition); // if mouse is over the object

	if(isTouchHandler() && canHandleIt(handled) && didHit) {

		// if the mouse wasn't over it already
		if(!isTouchOver()) {
			onHoverIn();
			_isTouchOver = true;				// update flag
		} else {
			// we were already on it!
			onHoverOn();	// and trigger 
		}

		handled = this; // this is the handler now!
        
	} else if(isTouchOver()) {		// if mouse is not over the object, but the flag is true (From previous frame)
		_isTouchOver = false;		// update flag
		onHoverOut();			// call
	} else {
		// unaffected
	}
	
	return handled;
}

//--------------------------------------------------------------

ofxMuiObject* ofxMuiObject::_mouseDragged(ofMouseEventArgs &e, ofxMuiObject* handled) {
	
    cout << "_____DRAGGING_____ isDragging()=" << isDragging() << endl;
	if(!isEnabled() || isHidden()) return NULL;
	
	saveMouseEvent(e);

    if(propagateTouchEvents()) {
        ofxMuiObject* newHandler = NULL;
        for(int i = (childObjects.size()-1); i >= 0; i--) {
            ofxMuiPushObject(childObjects[i]);
            newHandler = childObjects[i]->_mouseDragged(e, handled);
            ofxMuiPopObject();
            // currently an event can only be handled by one item
            if(!handled && newHandler) {
                handled = newHandler;
            }
        }
    }

	bool didHit = hitTest(mousePosition);
	
    cout << "_____DIDHIT=" << didHit << "_____" << endl;

    if(isTouchHandler() && (isDragging() || (canHandleIt(handled) && didHit && isTouchDown()))) {
        cout << "1" << endl;

        if(!isDragging()) {
            cout << "1a" << endl;

            _isDragging = true;
            dragStartPosition = mousePosition;
            
//            if (isHitDragable() && insideHitBox(screenToHitBox(mousePosition))) {
            if (isHitDragable() && hitTest(mousePosition)) {
                cout << "1b" << endl;

                cout << "hit drag origin = true" << endl;;
                _isHitDragOrigin = true;
            } else {
                cout << "1c" << endl;
                cout << "hit drag origin = false" << endl;;
                _isHitDragOrigin = false;
            }
            
            onDragStart();
        } else {
            cout << "XXXX" << endl;
            totalDragDelta = mousePosition - dragStartPosition;
            onDrag();
        }
        
        cout << "2 isDragMoveable=" << isDragMoveable() << endl;
        cout << "2 isHitDragable=" << isHitDragable() << endl;
        cout << "2 isHitDragOrigin=" << isHitDragOrigin() << endl;

        if(isDragMoveable() && isHitDragOrigin() && isHitDragable()) {
            
            cout << "2a" << endl;

            ofVec2f pos = getPosition();
            ofVec2f maxB = getBox().getMax();
            ofVec2f minB = getBox().getMin();
            
            ofVec2f inWPos;
            if(hasParent()) {
                inWPos = parent->screenToHitBox(mousePosition);
            } else {
                inWPos = screenToHitBox(mousePosition);
            }
            
            cout << "YYY" << endl;
            
            //cout << "2b" << endl;

            
          //  (inWPos.x > minB.x && deltaMousePosition.x > 0)
          //  ||
          //  (inWPos.x < (maxB.x + hitBox.width) && deltaMousePosition.x < 0)) {
            
            
            //if((inWPos.x > minB.x && deltaMousePosition.x > 0) ||
            //   (inWPos.x < maxB.x && deltaMousePosition.x < 0)) {
//                getBox().translateX(deltaMousePosition.x);
            //}
  
            ofRectangle& boxRef = getBoxRef();
            boxRef.translate(deltaMousePosition);
            

            onDragMoving();
        } 
            
        if(didHit) {
            onDragOver();
        } else {
            onDragOutside();
        }
        
		handled = this;
	
	} else {
                
		// ALREADY HANDLED OR NOT DRAGGING OR NOT OVER
		if(didHit) {
			if(isDropReceiverForObject(handled)) {
                onObjectDragOver(handled);
            }
		} else {
			if(isDropReceiverForObject(handled)) {
                onObjectDragOutside(handled);
            }
		}		
	}
		
	return handled;
}

//--------------------------------------------------------------

ofxMuiObject* ofxMuiObject::_mousePressed(ofMouseEventArgs &e, ofxMuiObject* handled, int numClicks) {

	if(!isEnabled() || isHidden()) return NULL;
	
	saveMouseEvent(e);

    if(propagateTouchEvents()) {
        ofxMuiObject* newHandler = NULL;
        for(int i = (childObjects.size()-1); i >= 0; i--) {
            ofxMuiPushObject(childObjects[i]);
            newHandler = childObjects[i]->_mousePressed(e, handled, numClicks);
            ofxMuiPopObject();
            // currently an event can only be handled by one item
            if(!handled && newHandler) {
                handled = newHandler;
            }
        }
    }
	
    
    bool didHit = hitTest(mousePosition);
    
	if(isTouchHandler() && canHandleIt(handled) && didHit) {
		if(!isTouchDown()) {				
			_isTouchDown = true;
			onPress();
            if(numClicks == 2 && pMousePosition == mousePosition) {
                onDoublePress();
            }
		}
		handled = this;					// set this as handled
	} else {							// if mouse is not over
		_isTouchDown = false;
		onPressOutside(); // someone else was pressed
	}

	return handled;
}

//--------------------------------------------------------------
ofxMuiObject* ofxMuiObject::_mouseReleased(ofMouseEventArgs &e, ofxMuiObject* handled) {

	if(!isEnabled() || isHidden()) return NULL;

	saveMouseEvent(e);
    
    if(propagateTouchEvents()) {
        ofxMuiObject* newHandler = NULL;
        for(int i = (childObjects.size()-1); i >= 0; i--) {
            ofxMuiPushObject(childObjects[i]);
            newHandler = childObjects[i]->_mouseReleased(e, handled);
            ofxMuiPopObject();
            // currently an event can only be handled by one item
            if(!handled && newHandler) {
                handled = newHandler;
            }
        }
    }	
    
	// hit test
	bool didHit = hitTest(mousePosition);
	
	if(isTouchHandler() && (isDragging() || didHit)) {
		// this is either the object being dragged or an object being dropped upon
		
		if(isDragging()) {
			if(didHit) {
				onRelease();
			} else {
				onReleaseOutside();
			}
			
			handled = this;
            
		} else if(didHit && isDropReceiverForObject(handled)) {
			// are we a drop receiver? and is there valid drag object?
			onObjectDropReceived(handled); // note that we got it
			handled->onObjectDropSent(this); // note to the object that it was sent
		} else {
			// drop failed
			onRelease();
		}

	} else {
		onReleaseOutside();
	}
		
	
	// in any case, the mouse is no longer down
    _isDragging = false;
	_isTouchDown = false;
    _isTouchOver = isTouchHandler() && didHit;
	
	return handled;
}

//--------------------------------------------------------------
void ofxMuiObject::cancelDrag() {
	if(isDragging()) {
		_isDragging = false;
		onDragCancelled();
	}
}

//--------------------------------------------------------------
void ofxMuiObject::setIsDragMoveable(bool _val) {
    _isDragMoveable = _val;
}

//--------------------------------------------------------------
bool ofxMuiObject::isDragMoveable() const {
    return _isDragMoveable;
}

//--------------------------------------------------------------
bool ofxMuiObject::isDragging() const {
    return _isDragging;
}

//--------------------------------------------------------------
bool ofxMuiObject::isHitDragable() const {
    return _isHitDragable;
}

//--------------------------------------------------------------
bool ofxMuiObject::isHitDragOrigin() const {
    return _isHitDragOrigin;
}

//--------------------------------------------------------------
void ofxMuiObject::setIsFileDropReceiver(bool _val) {
    _isFileDropReceiver = _val;
}

//--------------------------------------------------------------
bool ofxMuiObject::isFileDropReceiver() const {
    return _isFileDropReceiver;
}

//--------------------------------------------------------------
void ofxMuiObject::setIsDropSender(bool _val) {
    _isDropSender = _val;
}

//--------------------------------------------------------------
bool ofxMuiObject::isDropSender() const {
    return _isDropSender;
}

//--------------------------------------------------------------
void ofxMuiObject::setIsDropReceiver(bool _val) {
    _isDropReceiver = _val;
}

//--------------------------------------------------------------
bool ofxMuiObject::isDropReceiver() const {
    return _isDropReceiver;
}

//--------------------------------------------------------------
bool ofxMuiObject::isTouchDown() const {
    return _isTouchDown;
}

//--------------------------------------------------------------
bool ofxMuiObject::isTouchOver() const {
    return _isTouchOver;
}



//--------------------------------------------------------------
/*
void ofxMuiObject::releaseDrag() {
	
	if(isMouseDragging()) {
		_isMouseDragging = false;
		onDragReleased(x, y);
		preDragPosition = NULL; // get rid of the reference
	}
}
 */

//--------------------------------------------------------------

void ofxMuiObject::doSetEnabled(bool _enabled) {
	// tell all of the children (if any)
	for(int i = (childObjects.size()-1); i >= 0; i--) {
        ofxMuiPushObject(childObjects[i]);
		childObjects[i]->setEnabled(_enabled);
        ofxMuiPopObject();
	}
}



//--------------------------------------------------------------
int ofxMuiObject::saveObjectData() {
	/*
	int id	= props->mXml.addTag("OBJECT");
	
	props->mXml.setValue("OBJECT:ENABLED", isEnabled, id);
	props->mXml.setValue("OBJECT:TYPE", getTagName(), id);
	props->mXml.setValue("OBJECT:NAME", name, id);

	props->mXml.setValue("OBJECT:LEFT", mObjX, id);
	props->mXml.setValue("OBJECT:TOP", mObjY, id);
	//	props->mXml.setValue("OBJECT:WIDTH", mObjWidth, id);
	//	props->mXml.setValue("OBJECT:HEIGHT", mObjHeight, id);
	
	//	props->mXml.setValue("OBJECT:CTRX", mCtrX, id);
	//	props->mXml.setValue("OBJECT:CTRY", mCtrY, id);
	
	props->mXml.setValue("OBJECT:WIDTH", mCtrWidth, id);
	props->mXml.setValue("OBJECT:HEIGHT", mCtrHeight, id);
	
	props->mXml.setValue("OBJECT:MODE", mMode, id);
	props->mXml.setValue("OBJECT:DISPLAY", mDisplay, id);
	props->mXml.setValue("OBJECT:STEPS", mSteps, id);
	
	return id;
	 */
	return -1;
}


//--------------------------------------------------------------

// this can be overridden to provide more sophisticated drag/drop filtering
bool ofxMuiObject::isDropReceiverForObject(ofxMuiObject* dragObject) {
	return dragObject && isDropReceiver() && dragObject->isDropSender();
}

//--------------------------------------------------------------
void ofxMuiObject::saveMouseEvent(ofMouseEventArgs &e) {
    pMousePosition.x = mousePosition.x;
    pMousePosition.y = mousePosition.y;
	pMouseButton = mouseButton;

    mousePosition.x = e.x;
    mousePosition.y = e.y;
    mouseButton = e.button;
    
    deltaMousePosition = mousePosition - pMousePosition;
}

//--------------------------------------------------------------
void ofxMuiObject::saveKeyboardEvent(ofKeyEventArgs &e) {
    pKey = key;
	key = e.key;
}

//--------------------------------------------------------------
bool ofxMuiObject::hasParent() const {
	return parent != NULL;
}


//--------------------------------------------------------------
ofVec2f ofxMuiObject::getBoxScreenPosition() {
	if(hasParent()) {
        // we always measure position from the parent's CONTENT box
		return parent->getBoxScreenPosition() + parent->getContentBoxOffset() +  getPosition();
	} else {
        // get the position w/ respect to the window
		return getPosition();
	}
}

//--------------------------------------------------------------
ofVec2f ofxMuiObject::getContentBoxScreenPosition() {
    return getBoxScreenPosition() + getContentBoxOffset();
}


//--------------------------------------------------------------
string ofxMuiObject::getDebugString() {
    ostringstream out;
	
//out << name << " ";
    out << "mX/mY: " << mousePosition << endl;
    out << "dX/dY: " << deltaMousePosition << endl;
    out << "mouseOver: " << isTouchOver() << " mouseDown: " << isTouchDown() << " isDragging: " << isDragging() << endl;
    
    if(isDragging()) {
        out << dragStartPosition << endl;
        out << totalDragDelta << endl;
    }

	return out.str();
}


////--------------------------------------------------------------
//void ofxMuiObject::setName(string _name) {
//    name = _name;
//}
//
////--------------------------------------------------------------
//string ofxMuiObject::getName() {
//    return name;
//}

//--------------------------------------------------------------
string ofxMuiObject::getObjectType() {
    return objectType;
}

//--------------------------------------------------------------
void ofxMuiObject::setObjectType(string _objectType) {
    objectType = _objectType;
}

//--------------------------------------------------------------
void ofxMuiObject::setOrientation(ofOrientation _orientation) {
    orientation = _orientation;
    requestBoxLayout();
}

//--------------------------------------------------------------
ofOrientation ofxMuiObject::getOrientation() {
    return orientation;
}

//--------------------------------------------------------------
bool ofxMuiObject::isOrientationHorizontal() {
    switch(orientation) {
        case OF_ORIENTATION_UNKNOWN: // we'll just assume
        case OF_ORIENTATION_DEFAULT:	
        case OF_ORIENTATION_180:
            return true;
        case OF_ORIENTATION_90_RIGHT:
        case OF_ORIENTATION_90_LEFT:
            return false;
    }
}






////--------------------------------------------------------------
//string ofxMuiObject::getTooltip() const {
//    return tooltip;
//}
//
////--------------------------------------------------------------
//void ofxMuiObject::setTooltip(const string& _tooltip) {
//    tooltip = _tooltip;
//}
//
////--------------------------------------------------------------
//bool ofxMuiObject::isTooltipEnabled() const {
//    return bTooltipEnabled;
//}
//
////--------------------------------------------------------------
//void ofxMuiObject::setTooltipEnabled(bool _enabled) {
//    bTooltipEnabled = _enabled;
//}

//--------------------------------------------------------------
ofxMuiObject* ofxMuiObject::getParent() {
    return parent;
}

//--------------------------------------------------------------
void ofxMuiObject::setParent(ofxMuiObject* _parent) {
    parent = _parent;
}


//--------------------------------------------------------------
void ofxMuiObject::addChild(ofxMuiObject* child) {
    if(child == NULL) {
        ofLogWarning("ofxMuiObject") << "addChild() : child is NULL.";
        return;
    }
    
    if(child->hasParent()) {
        ofLogWarning("ofxMuiObject") << "addChild() : child already has parent.  Failing.";
        return;
    }

    child->setParent(this);
    childObjects.push_back(child);
    requestBoxLayout();
}

//--------------------------------------------------------------
bool ofxMuiObject::isChild(ofxMuiObject* child) {
    
    // just double check
    if(this == child) return false;
    
    // TODO: this will short circuit, but perhaps there is a better structure
    // to hold all of these than a giant recursive tree ...
    for(int i = 0; i < childObjects.size(); i++) {
        // is this a child?  or is it a child of the child?
        if(child == childObjects[i] || childObjects[i]->isChild(child)) {
            return true;
        }        
    }
    
    return false;
}


//--------------------------------------------------------------
bool ofxMuiObject::hasChildren() {
    return !childObjects.empty();
}

//--------------------------------------------------------------
bool ofxMuiObject::removeChild(ofxMuiObject* child) {
    
    // iterator to vector element:
    childObjectsIter = find (childObjects.begin(), childObjects.end(), child);

    // TODO: should delete memory?
    // XXXXX DELETE MEMORY?
    if(childObjectsIter != childObjects.end()) {
        childObjects.erase(childObjectsIter);
        return true;
    } else {
        return false;
    }	
}

//--------------------------------------------------------------
void ofxMuiObject::drawBox() {
    ofColor boxColor = cBox.get(isTouchOver(),isTouchDown(),isEnabled(),alphaScale);
    if(boxColor.a > 0) {
        ofFill();
        ofSetColor(boxColor);
        ofxRect(0,0,getWidth(),getHeight());
    }
}

//--------------------------------------------------------------
void ofxMuiObject::drawBorder() {
    ofColor borderColor = cBorder.get(isTouchOver(),isTouchDown(),isEnabled(),alphaScale);
    if(borderColor.a > 0) {
        ofFill();
        ofSetColor(borderColor);
        ofxFrame(0,0,getBorderBoxWidth(),getBorderBoxHeight(),getBoxProperties().border);
    }
}

//--------------------------------------------------------------
void ofxMuiObject::drawPaddingBox() {
    ofColor paddingColor = cPadding.get(isTouchOver(),isTouchDown(),isEnabled(),alphaScale);
    if(paddingColor.a > 0) {
        ofFill();
        ofSetColor(paddingColor);
        ofxRect(0,0,getPaddingBoxWidth(),getPaddingBoxHeight());
    }
}

//--------------------------------------------------------------
void ofxMuiObject::drawContentBox() {
    ofColor contentColor = cContent.get(isTouchOver(),isTouchDown(),isEnabled(),alphaScale);
    if(contentColor.a > 0) {
        ofFill();
        ofSetColor(contentColor);
        ofxRect(0,0,getContentBoxWidth(),getContentBoxHeight());
    }
}

//--------------------------------------------------------------
void ofxMuiObject::drawShadowBox() {
    // DRAW THE SHADOW if alpha is set above 0
    if(doShadow) {
        ofColor shadowColor = cShadow.get(isTouchOver(),isTouchDown(),isEnabled(),alphaScale);
        if(shadowColor.a > 0) {
            ofPushMatrix();
            ofTranslate(shadowOffset);
            ofFill();
            ofSetColor(shadowColor);
            ofxRect(getBorderBox());
            ofPopMatrix();
        }
    }
}

//-------------------------------------------------------------
void ofxMuiObject::draw() {
    ofColor hitColor = cHit.get(isTouchOver(),isTouchDown(),isEnabled(),alphaScale);
    if(hitColor.a > 0) {
        ofFill();
        ofSetColor(hitColor);
        ofxRect(0,0,getHitBoxWidth(),getHitBoxHeight());
    }
}

//--------------------------------------------------------------
float ofxMuiObject::getAlphaScale() const {
    return alphaScale;
}
//--------------------------------------------------------------
void ofxMuiObject::setAlphaScale(float _alphaScale) {
    alphaScale = _alphaScale;
}

//--------------------------------------------------------------
bool ofxMuiObject::getDoShadow() const {
    return doShadow;
}
//--------------------------------------------------------------
void ofxMuiObject::setDoShadow(bool _doShadow) {
    doShadow = _doShadow;
}

//--------------------------------------------------------------
void ofxMuiObject::setKeyboardOnPress(bool _keyboardOnPress) {
    keyboardOnPress = _keyboardOnPress;
}

//--------------------------------------------------------------
bool ofxMuiObject::getKeyboardOnPress() {
    return keyboardOnPress;
}

//--------------------------------------------------------------
bool ofxMuiObject::isShiftPressed() {
    // PR rejected b/c of glut find new solution
    return false;//ofGetModifierPressed(OF_KEY_SHIFT);
}

//--------------------------------------------------------------
bool ofxMuiObject::isCtrlPressed() {
    // PR rejected b/c of glut find new solution
    return false;//ofGetModifierPressed(OF_KEY_CTRL);
}

//--------------------------------------------------------------
bool ofxMuiObject::isAltPressed() {
    // PR rejected b/c of glut find new solution
    return false;//ofGetModifierPressed(OF_KEY_ALT);
}

//--------------------------------------------------------------
float ofxMuiObject::getActiveTimer() {
    return isActiveTimer;
}

//--------------------------------------------------------------
bool ofxMuiObject::canHandleIt(ofxMuiObject* handled) {
    // are we the handler already?  or has it been handled yet?
    return handled == this || !handled;
}


