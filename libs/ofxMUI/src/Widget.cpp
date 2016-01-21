// =============================================================================
//
// Copyright (c) 2009-2016 Christopher Baker <http://christopherbaker.net>
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//
// =============================================================================


#include "ofx/MUI/Widget.h"
#include "ofx/DOM/Exceptions.h"
#include "ofx/MUI/MUI.h"
#include "ofx/MUI/Styles.h"


namespace ofx {
namespace MUI {


Widget::Widget(float x, float y, float width, float height):
    Widget("", x, y, width, height)
{
}


Widget::Widget(const std::string& id, float x, float y, float width, float height):
    DOM::Element(id, x, y, width, height)
{
    addEventListener(pointerMove, &Widget::_onPointerEvent, false, std::numeric_limits<int>::min());
    addEventListener(pointerDown, &Widget::_onPointerEvent, false, std::numeric_limits<int>::min());
    addEventListener(pointerUp, &Widget::_onPointerEvent, false, std::numeric_limits<int>::min());

    addEventListener(pointerOver, &Widget::_onPointerEvent, false, std::numeric_limits<int>::min());
    addEventListener(pointerEnter, &Widget::_onPointerEvent, false, std::numeric_limits<int>::min());
    addEventListener(pointerOut, &Widget::_onPointerEvent, false, std::numeric_limits<int>::min());
    addEventListener(pointerLeave, &Widget::_onPointerEvent, false, std::numeric_limits<int>::min());

    addEventListener(gotPointerCapture, &Widget::_onPointerCaptureEvent, false, std::numeric_limits<int>::min());
    addEventListener(lostPointerCapture, &Widget::_onPointerCaptureEvent, false, std::numeric_limits<int>::min());

	setImplicitPointerCapture(true);
}


Widget::~Widget()
{
    removeEventListener(pointerMove, &Widget::_onPointerEvent, false, std::numeric_limits<int>::min());
    removeEventListener(pointerDown, &Widget::_onPointerEvent, false, std::numeric_limits<int>::min());
    removeEventListener(pointerUp, &Widget::_onPointerEvent, false, std::numeric_limits<int>::min());

    removeEventListener(pointerOver, &Widget::_onPointerEvent, false, std::numeric_limits<int>::min());
    removeEventListener(pointerEnter, &Widget::_onPointerEvent, false, std::numeric_limits<int>::min());
    removeEventListener(pointerOut, &Widget::_onPointerEvent, false, std::numeric_limits<int>::min());
    removeEventListener(pointerLeave, &Widget::_onPointerEvent, false, std::numeric_limits<int>::min());

    removeEventListener(gotPointerCapture, &Widget::_onPointerCaptureEvent, false, std::numeric_limits<int>::min());
    removeEventListener(lostPointerCapture, &Widget::_onPointerCaptureEvent, false, std::numeric_limits<int>::min());
}


void Widget::onDraw() const
{
	Styles::State state = Styles::STATE_NORMAL;

	if (isDragging() || isPointerDown())
	{
		state = Styles::STATE_DOWN;
	}
	else if (isPointerOver())
	{
		state = Styles::STATE_OVER;
	}

    ofFill();
    ofSetColor(getStyles()->getColor(Styles::ROLE_BACKGROUND, state));
    ofDrawRectangle(0, 0, getWidth(), getHeight());

    ofNoFill();
    ofSetColor(getStyles()->getColor(Styles::ROLE_BORDER, state));
    ofDrawRectangle(0, 0, getWidth(), getHeight());

	ofFill();
	ofSetColor(getStyles()->getColor(Styles::ROLE_FOREGROUND, state));
	ofDrawRectangle(10, 10, getWidth() - 20, getHeight() - 20);

	std::stringstream ss;

	ss << getId() << std::endl;

	if (!capturedPointers().empty())
	{
		ss << "CP: " << ofToString(capturedPointers()) << std::endl;
	}

	ofSetColor(getStyles()->getColor(Styles::ROLE_TEXT, state));
	ofDrawBitmapString(ss.str(), 2, 12);


}


bool Widget::isPointerOver() const
{
    return _isPointerOver;
}


bool Widget::isPointerDown() const
{
	return !capturedPointers().empty();
}


void Widget::setDropTarget(bool dropTarget)
{
    _isDropTarget = dropTarget;
}


bool Widget::isDropTarget() const
{
    return _isDropTarget;
}


void Widget::setDraggable(bool draggable)
{
	_isDraggable = draggable;
}


bool Widget::isDraggable() const
{
    return _isDraggable;
}


bool Widget::isDragging() const
{
    return _isDragging;
}


std::shared_ptr<Styles> Widget::getStyles() const
{
	if (_styles == nullptr)
	{
		const MUI* mui = dynamic_cast<const MUI*>(document());

		if (mui != nullptr)
		{
			_styles = mui->getDocumentStyles();
		}
		else
		{
			ofLogWarning("Widget::getStyles") << "No root document, using default styles.";
			_styles = std::make_shared<Styles>();
		}
	}

	return _styles;
}


void Widget::setStyles(std::shared_ptr<Styles> styles)
{
	_styles = styles;
}


std::unique_ptr<Layout> Widget::removeLayout()
{
    if (nullptr != _layout)
    {
        // Move the Layout.
        std::unique_ptr<Layout> detachedLayout = std::move(_layout);

        // Set the parent to nullptr.
        detachedLayout->_parent = nullptr;

        // Invalidate all cached child geometry.
        invalidateChildGeometry();

        // Return the detached child.
        // If the return value is ignored, it will be deleted.

        return detachedLayout;
    }
    else
    {
        // Return nullptr because we couldn't find anything.
        return nullptr;
    }
}


Layout* Widget::layout()
{
    // Returns nullptr if no object is owned.
    return _layout.get();
}


void Widget::invalidateChildGeometry() const
{
    DOM::Element::invalidateChildGeometry();

    if (nullptr != _layout)
    {
        _layout->doLayout();
    }
}


void Widget::_onPointerEvent(DOM::PointerUIEventArgs& e)
{
	if (e.type() == PointerEventArgs::POINTER_DOWN)
	{
	}
	else if (e.type() == PointerEventArgs::POINTER_MOVE)
	{
		if (_isDragging)
		{
			if (!capturedPointers().empty())
			{
				const DOM::CapturedPointer& pointer = *capturedPointers().begin();
				setPosition(screenToParent(pointer.position() - pointer.offset()));
			}
			else
			{
				ofLogError("Widget::_onPointerEvent") << "No captured pointers to drag with.";
			}

		}
	}
	else if (e.type() == PointerEventArgs::POINTER_OVER)
	{
		_isPointerOver = true;
	}
	else if (e.type() == PointerEventArgs::POINTER_OUT)
	{
		_isPointerOver = false;
	}
    else
    {
       // cout << "unhandled " << e.type() << endl;
    }
}


void Widget::_onPointerCaptureEvent(DOM::PointerCaptureUIEventArgs& e)
{
	if (e.type() == PointerEventArgs::GOT_POINTER_CAPTURE)
	{
		if (_moveToFrontOnCapture)
		{
			moveToFront();
		}

		_isDragging = _isDraggable;

	}
	else if (e.type() == PointerEventArgs::LOST_POINTER_CAPTURE)
	{
		_isDragging = false;
	}
}


std::vector<Widget*> Widget::getChildWidgets()
{
    std::vector<Widget*> results;

    auto iter = _children.begin();

    while (iter != _children.end())
    {
        Widget* pWidget = dynamic_cast<Widget*>(iter->get());

        if (pWidget)
        {
            results.push_back(pWidget);
        }

        ++iter;
    }

    return results;
}


} } // namespace ofx::MUI
