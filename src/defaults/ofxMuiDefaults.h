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

#pragma once

#include "ofMain.h"
#include "ofxMuiFont.h"
#include "ofxMuiConstants.h"
#include "ofxMuiColor.h"
#include "ofxMuiIcon.h"
#include "ofxMuiBoxProperties.h"


// The goal of this class is to provide an easy way for people to 
// set their own skins / default values for the gui.  It uses a standard
// Singleton pattern, which seems to be controversial ... but it
// seems to make the most sense for settings like this.

//--------------------------------------------------------------
static bool layoutDebug = true; // useful for viewing layout debugging
//--------------------------------------------------------------



class ofxMuiDefaults {
public:
    static ofxMuiDefaults& getInstance()
    {
        static ofxMuiDefaults instance; // Guaranteed to be destroyed.
        // Instantiated on first use.
        return instance;
    }

    
    bool loadDefaultsFromXML(string file) {
        // defaults can be loaded here statically!
        // ... someday
        return true;
    }
    
    //--------------------------------------------------------------
    // STATIC SHARED VARS FOR ofxMuiOBJECT /////////////////////////
    //--------------------------------------------------------------
    //      These variables are initialized in the .cpp file.
    //      They can be set during ofxMui initialization and all 
    //      subsequent objects will use them as defaults.
    //      See below for actual descriptions.
    
    bool enableContentBoxClipping; // should use GLSCISSORS to clip content to the content box 
    
    ofxMuiBoxProperties boxProperties;
    
    // COLORS
    ofxMuiColor cBox, cBorder, cPadding, cContent, cHit;
    
    ofxMuiColor cText;
    
    ofxMuiColor cActiveAreaForeground;
    ofxMuiColor cActiveAreaBackground;
    ofxMuiColor cActiveAreaFrame;
    
    ofxMuiColor cShadow;
    
    // SHADOWS
    bool        doShadow;
    ofVec2f     shadowOffset;
    
    // ALPHA
    float       alphaScale;

    // FONTS
    
    ofxMuiFont* getFont(ofxMuiSize fontSize);
    ofxMuiFont* getXXSmallFont();
    ofxMuiFont* getXSmallFont();
    ofxMuiFont* getSmallFont();
    ofxMuiFont* getMediumFont();
    ofxMuiFont* getLargeFont();
    ofxMuiFont* getXLargeFont();
    ofxMuiFont* getXXLargeFont();
    
    // SHARED
    ofxMuiBoxProperties sharedLabelBoxProperties;
    ofxMuiBoxProperties sharedLabelCleanBoxProperties;
    
    ofxMuiBoxProperties valueLabelBoxProperties;
    
    //--------------------------------------------------------------
    // MOUSE/KEYBOARD //////////////////////////////////////////////
    //--------------------------------------------------------------
    
    // are we looking for keyboard up or keyboard down?
    bool keyboardOnPress;
    
    char keyboardIncrease;
    char keyboardDecrease;
    char keyboardToggle;
    char keyboardMinMax;
    char keyboardEnableDisable;
    
    float incrementFloat;
    float incrementInt;
    
    int   displayPrecision;
    
    //--------------------------------------------------------------
    // XXXXXXXXXXXXXX //////////////////////////////////////////////
    //--------------------------------------------------------------
    
    // WINDOWS
    ofxMuiBoxProperties windowBoxProperties;
    ofxMuiIconName windowBoxMinMaxIcon;
    ofxMuiIconName windowBoxEnableDisableIcon;
    
    // HITS
    ofxMuiBoxProperties hitBoxProperties;
    
    // PANELS
    ofxMuiBoxProperties panelBoxProperties;
    
    // LABEL
    ofxMuiBoxProperties labelBoxProperties;
    
    // TOOLTIP
    ofxMuiBoxProperties tooltipBoxProperties;
    ofVec2f             tooltipOffset;
    int                 tooltipWaitDelay;
    int                 tooltipFadeDelay;

    // BUTTON
    ofxMuiBoxProperties buttonBoxProperties;
    int                 buttonWidth;  // TODO: obsolete?
    int                 buttonHeight; // TODO: obsolete?
    bool                buttonRoundFrame;
    
    // SLIDER
    ofxMuiBoxProperties sliderBoxProperties;
    ofxMuiBoxProperties sliderLabelBoxProperties;
    ofOrientation   sliderOrientation;
    bool                sliderUseHandle;
    int                 sliderHandleSize;
    int                 sliderWidth;
    int                 sliderHeight;
    
    bool                sliderEnableRange;
    int                 sliderRangeHandleSize;

    
    // COLOR PICKER
    ofxMuiBoxProperties colorPickerBoxProperties;
    int                 colorPickerWidth;
    int                 colorPickerHeight;
    int                 colorPickerPreviewHeight;
    int                 colorPickerEyeDropperZoom;
    int                 colorPickerEyeDropperWidth;
    int                 colorPickerEyeDropperHeight;
    
    // KNOB
    ofxMuiBoxProperties knobBoxProperties;
    int                 knobWidth;
    int                 knobHeight;

    // KNOB
    ofxMuiBoxProperties numberBoxBoxProperties;
    int                 numberBoxWidth;
    int                 numberBoxHeight;

    int                 dataHistoryLength;
    
    //--------------------------------------------------------------
    // SHARED IMAGES ///////////////////////////////////////////////
    //--------------------------------------------------------------

    void loadCustomIcon(ofxMuiIconName icon, string fileOn, string fileOff) {
        ofxMuiIcon* ico = getIcon(icon);
        
        if(ico != NULL) {
            ico->getIcon(true)->loadImage(fileOn);
            ico->getIcon(false)->loadImage(fileOff);
        } else {
            icons[icon] = ofxMuiIcon(fileOn,fileOff);
            icons[icon].getIcon(true)->loadImage(fileOn);
            icons[icon].getIcon(false)->loadImage(fileOff);
        }
    }
    
    ofxMuiIcon* getIcon(ofxMuiIconName icon) {
        iconsIterator = icons.find(icon);

        if(iconsIterator != icons.end()) {
            return &(*iconsIterator).second;
        } else {
            return NULL;
        }
    }
    
    // FONTS
    ofxMuiFont xxSmallFont;
    ofxMuiFont xSmallFont;
    ofxMuiFont smallFont;
    ofxMuiFont mediumFont;
    ofxMuiFont largeFont;
    ofxMuiFont xLargeFont;
    ofxMuiFont xxLargeFont;
    
    
private:
    
    ofxMuiDefaults();
    // Dont forget to declare these two. You want to make sure they
    // are unaccessable otherwise you may accidently get copies of
    // your singelton appearing.
    ofxMuiDefaults(ofxMuiDefaults const&); // Don't Implement
    void operator=(ofxMuiDefaults const&); // Don't implement
    
    void initWindow();
    void initKeyboard();
    void initShared();
    void initButton();
    void initPanel();
    void initLabel();
    void initTooltip();
    void initSlider();
    void initKnob();
    void initNumberBox();
    
    void initColorPicker();

    void initFonts();
    void initIcons();
    
    //--------------------------------------------------------------
    // STATIC SHARED ASSETS ////////////////////////////////////////
    //--------------------------------------------------------------
    map<ofxMuiIconName, ofxMuiIcon> icons;
    map<ofxMuiIconName, ofxMuiIcon>::iterator iconsIterator;
    
    string baseDir;
    string iconsDir;
    string fontsDir;
    
};