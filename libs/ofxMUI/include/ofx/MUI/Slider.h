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


#pragma once
	

#include "ofx/MUI/Styles.h"
#include "ofx/MUI/Types.h"
#include "ofx/MUI/Utils.h"


namespace ofx {
namespace MUI {


/// \brief Slider
///
/// \tparam Type the data type represented by the Slider.
template<typename Type>
class Slider: public Widget
{
public:
    /// \brief Create a Slider with the given parameters.
    /// \param id The Widget's id string.
    /// \param x x-position in parent coordinates.
    /// \param y y-position in parent coordinates.
    /// \param width The width (x-dimension) of Widget.
    /// \param height The height (y-dimension) of Widget.
    /// \param orientation The orientation of the Slider.
    /// \param mode The DragMode of the Slider.
    Slider(const std::string& id = "",
           float x = 0,
           float y = 0,
           float width = DEFAULT_WIDTH,
           float height = DEFAULT_HEIGHT,
           Orientation orientation = Orientation::DEFAULT,
           DragMode mode = DragMode::ABSOLUTE);

    /// \brief Destroy the Slider.
    virtual ~Slider();

    virtual void onDraw() const override;

    /// \returns the current DragMode.
    DragMode getDragMode() const;

    /// \brief Set the current DragMode.
    ///
    /// DragMode affects how a slider interprets drags.
    ///
    /// \param mode The DragMode to set.
    void setDragMode(DragMode mode);

    /// \returns the current orientation.
    Orientation getOrientation() const;

    /// \brief Set the Orientation of the slider.
    /// \param the Desired Orientation.
    void setOrientation(Orientation orientation);

    /// \brief Determine if the slider direction inverted.
    ///
    /// By default the Slider direction is mapped from 0 (left) to 1 (right)
    /// in Orientation::LANDSCAPE mode and from 0 (bottom) to 1 (top) in
    /// Orientation::PORTRAIT mode.  Inverted Sliders map in the opposite way.
    ///
    /// \returns true if the Slider is inverted.
    bool isInverted() const;

    /// \brief Invert the Slider direction.
    ///
    /// By default the Slider direction is mapped from 0 (left) to 1 (right)
    /// in Orientation::LANDSCAPE mode and from 0 (bottom) to 1 (top) in
    /// Orientation::PORTRAIT mode.  Inverted Sliders map in the opposite way.
    ///
    /// \param inverted true if the Slider direction should be inverted.
    void setInverted(bool inverted);

    /// \brief Pointer event callback.
    /// \param evt The event data.
    void onPointerEvent(DOM::PointerUIEventArgs& e);

    /// \brief Pointer event callback.
    /// \param evt The event data.
    void onPointerCaptureEvent(DOM::PointerCaptureUIEventArgs& e);

    /// \brief Bind this adapter to an existing parameter.
    /// \param parameter The parameter to bind to.
    /// void makeReferenceTo(ofParameter<Type>& parameter);

    /// \brief Add listeners to this onValueChanged event.
    /// \tparam ListenerClass The class type of the listener.
    /// \tparam ListenerMethod The name of the listener method.
    /// \param listener A pointer to the listener instance.
    /// \param method A pointer to the listener method.
    /// \param prioirty The order priority of this listener.
    template<class ListenerClass, typename ListenerMethod>
    void addListener(ListenerClass* listener, ListenerMethod method, int priority = OF_EVENT_ORDER_AFTER_APP)
    {
        ofAddListener(onValueChanged, listener, method, priority);
    }

    /// \brief Remove listeners to this onValueChanged event.
    /// \tparam ListenerClass The class type of the listener.
    /// \tparam ListenerMethod The name of the listener method.
    /// \param listener A pointer to the listener instance.
    /// \param method A pointer to the listener method.
    /// \param prioirty The order priority of this listener.
    template<class ListenerClass, typename ListenerMethod>
    void removeListener(ListenerClass* listener, ListenerMethod method, int priority = OF_EVENT_ORDER_AFTER_APP)
    {
        ofRemoveListener(onValueChanged, listener, method, priority);
    }

    /// \brief A callback for the parameter change.
    ofEvent<Type> onValueChanged;

    /// \brief The assignment operator.
    /// \param v Value to assign.
    /// \returns the assigned value.
    Type operator = (Type v);

    /// \brief Dereference operator.
    operator const Type& ();

    void setMin(const Type& min);
    Type getMin() const;

    void setMax(const Type& max);
    Type getMax() const;

    enum
    {
        DEFAULT_WIDTH = 40,
        DEFAULT_HEIGHT = 100
    };

protected:
    /// \brief Get the effective orientation.
    /// \returns only Orientation::LANDSCAPE or Orientation::HORIZONTAL.
    Orientation _getEffectiveOrientation() const;

    /// \brief Get the active axis index.
    /// \returns 0 for X or 1 for Y.
    std::size_t _getActiveAxisIndex() const;

    /// \brief A callback for any resize events.
    /// \param value The DOM::ResizeEvent arguments.
    void _onResize(DOM::ResizeEventArgs&);

    /// \brief A callback for the ParameterWidget's value.
    /// \param value The the updated value.
    void _onValueChanged(const void* sender, Type& value);

    /// \brief The Slider orientation.
    Orientation _orientation = Orientation::HORIZONTAL;

    /// \brief The drag mode.
    DragMode _dragMode = DragMode::RELATIVE;

    /// \brief The "primary" pointer id.
    ///
    /// This is the first pointer that was captured when dragging.
    std::size_t _primaryPointerId = 0;

    /// \brief The start value of the value itself.
    ///
    /// Used when computing offsets in Mode::RELATIVE.
    Type _valueOffset = 0;

    /// \brief Is the Slider direction inverted.
    bool _isInverted = false;

    /// \brief The parameter to watch.
    ofParameter<Type> _value;

private:
    /// \brief False when the _effectiveOrientation needs to be recalculated.
    mutable bool _effectiveOrientationInvalid = true;

    /// \brief The effective orientation used when the orientation mode is AUTO.
    mutable Orientation _effectiveOrientation;

};


template<typename Type>
Slider<Type>::Slider(const std::string& id,
                     float x,
                     float y,
                     float width,
                     float height,
                     Orientation orientation,
                     DragMode dragMode):
    Widget(id, x, y, width, height),
    _value(id, 0, 0, 1),
    _orientation(orientation),
    _dragMode(dragMode),
    _effectiveOrientation(_getEffectiveOrientation())
{
    _value.addListener(this,
                       &Slider<Type>::_onValueChanged,
                       std::numeric_limits<Type>::lowest());

    addEventListener(pointerDown, &Slider<Type>::onPointerEvent);
    addEventListener(pointerMove, &Slider<Type>::onPointerEvent);
    addEventListener(gotPointerCapture, &Slider<Type>::onPointerCaptureEvent);
    addEventListener(lostPointerCapture, &Slider<Type>::onPointerCaptureEvent);

    ofAddListener(Widget::resize, this, &Slider<Type>::_onResize, std::numeric_limits<int>::min());

	setImplicitPointerCapture(true);
}


template<typename Type>
Slider<Type>::~Slider()
{
    // Remove the listener from the local or bound parameters.
    _value.removeListener(this,
                          &Slider<Type>::_onValueChanged,
                          std::numeric_limits<Type>::lowest());

    removeEventListener(pointerDown, &Slider<Type>::onPointerEvent);
    removeEventListener(pointerMove, &Slider<Type>::onPointerEvent);
    removeEventListener(gotPointerCapture, &Slider<Type>::onPointerCaptureEvent);
    removeEventListener(lostPointerCapture, &Slider<Type>::onPointerCaptureEvent);

    ofRemoveListener(resize, this, &Slider<Type>::_onResize, std::numeric_limits<int>::min());
}


template<typename Type>
void Slider<Type>::onPointerEvent(DOM::PointerUIEventArgs& e)
{
    if (e.pointer().id() == _primaryPointerId)
    {
        if (PointerEventArgs::POINTER_DOWN == e.type() ||
            PointerEventArgs::POINTER_MOVE == e.type())
        {
            // Convert to integer to avoid warnings w/ Point.
            std::size_t axisIndex = _getActiveAxisIndex();

            // Local position.
            float position = screenToLocal(e.pointer().point())[static_cast<int>(axisIndex)];

            Type valueMin = _value.getMin();
            Type valueMax = _value.getMax();

            if (_isInverted)
            {
                std::swap(valueMin, valueMax);
            }

            float sliderMin = 0;
            float sliderMax = getSize()[static_cast<int>(axisIndex)];

            if (Orientation::VERTICAL == _getEffectiveOrientation())
            {
                std::swap(sliderMin, sliderMax);
            }

            Type _lerpedValue = Math::lerp(position,
                                           sliderMin,
                                           sliderMax,
                                           valueMin,
                                           valueMax);

            if (DragMode::RELATIVE == _dragMode)
            {
                if (PointerEventArgs::POINTER_DOWN == e.type())
                {
                    // Find the difference between the current value
                    // and the value underneath the pointer.  This is
                    // the value offset.
                    _valueOffset = (_value - _lerpedValue);
                    return;
                }
                else
                {
                    // Add the value offset to the calculated value based
                    // on the poiter position.
                    _lerpedValue += _valueOffset;
                }
            }

            _value = Math::clamp(_lerpedValue, valueMin, valueMax);

        }
    }
}


template<typename Type>
void Slider<Type>::onPointerCaptureEvent(DOM::PointerCaptureUIEventArgs& e)
{
    if (PointerEventArgs::GOT_POINTER_CAPTURE == e.type() &&
        _primaryPointerId == 0)
    {
        _primaryPointerId = e.id();
    }
    else if (PointerEventArgs::LOST_POINTER_CAPTURE == e.type() &&
             _primaryPointerId == e.id())
    {
        _primaryPointerId = 0;
    }
}


template<typename Type>
void Slider<Type>::onDraw() const
{
    ofFill();

    std::shared_ptr<Styles> styles = getStyles();

    if (isPointerDown())
    {
        ofSetColor(styles->getColor(Styles::ROLE_BACKGROUND, Styles::STATE_DOWN));
    }
    else if (isPointerOver())
    {
        ofSetColor(styles->getColor(Styles::ROLE_BACKGROUND, Styles::STATE_OVER));
    }
    else
    {
		ofSetColor(styles->getColor(Styles::ROLE_BACKGROUND, Styles::STATE_NORMAL));
    }

    ofDrawRectangle(0, 0, getWidth(), getHeight());

    ofNoFill();

	ofSetColor(styles->getColor(Styles::ROLE_BORDER, Styles::STATE_NORMAL));
    ofDrawRectangle(0, 0, getWidth(), getHeight());

    Orientation orientation = _orientation;

    if (Orientation::DEFAULT == orientation)
    {
        orientation = (getWidth() > getHeight()) ? Orientation::HORIZONTAL : Orientation::VERTICAL;
    }

    ofFill();

    if (isPointerDown())
    {
        ofSetColor(styles->getColor(Styles::ROLE_FOREGROUND, Styles::STATE_DOWN));
    }
    else if (isPointerOver())
    {
        ofSetColor(styles->getColor(Styles::ROLE_FOREGROUND, Styles::STATE_OVER));
    }
    else
    {
        ofSetColor(styles->getColor(Styles::ROLE_FOREGROUND, Styles::STATE_NORMAL));
    }

    Type min = _value.getMin();
    Type max = _value.getMax();

    if (_isInverted)
    {
        std::swap(min, max);
    }

    if (Orientation::HORIZONTAL == orientation)
    {
        float x = Math::lerp(_value, min, max, 0, getWidth(), true);
        ofDrawRectangle(0, 0, x, getHeight());
    }
    else
    {
        float y = Math::lerp(_value, min, max, getHeight(), 0, true);
        ofDrawRectangle(0, y, getWidth(), getHeight() - y);
    }
}


template<typename Type>
DragMode Slider<Type>::getDragMode() const
{
    return _dragMode;
}


template<typename Type>
void Slider<Type>::setDragMode(DragMode dragMode)
{
    _dragMode = dragMode;
}


template<typename Type>
Orientation Slider<Type>::getOrientation() const
{
    return _orientation;
}


template<typename Type>
void Slider<Type>::setOrientation(Orientation orientation)
{
    if (orientation == Orientation::DEFAULT &&
       _orientation != Orientation::DEFAULT)
    {
        _effectiveOrientationInvalid = true;
    }

    _orientation = orientation;
}


template<typename Type>
bool Slider<Type>::isInverted() const
{
    return _isInverted;
}


template<typename Type>
void Slider<Type>::setInverted(bool inverted)
{
    _isInverted = inverted;
}


template<typename Type>
Orientation Slider<Type>::_getEffectiveOrientation() const
{
    if (Orientation::DEFAULT == _orientation)
    {
        if (_effectiveOrientationInvalid)
        {
            _effectiveOrientation = getWidth() > getHeight() ? Orientation::HORIZONTAL : Orientation::VERTICAL;
            _effectiveOrientationInvalid = false;
        }

        return _effectiveOrientation;
    }
    else
    {
        return _orientation;
    }
}


template<typename Type>
std::size_t Slider<Type>::_getActiveAxisIndex() const
{
    return (_getEffectiveOrientation() == Orientation::HORIZONTAL) ? 0 : 1;
}


template<typename Type>
void Slider<Type>::_onResize(DOM::ResizeEventArgs&)
{
    _effectiveOrientationInvalid = true;
}


template<typename Type>
void Slider<Type>::_onValueChanged(const void* sender, Type& value)
{
    // We forward the event changes as sent by the slider.
    ofNotifyEvent(onValueChanged, value, this);
}


//template<typename Type>
//void Slider<Type>::makeReferenceTo(ofParameter<Type>& parameter)
//{
//    // 1. Remove the reference to the internal parameter.
//    _value.removeListener(this,
//                              &Slider<Type>::_onValueChanged,
//                              std::numeric_limits<Type>::lowest());
//    // 2. Make the internal parameter a refefence to the external parameter.
//    _value.makeReferenceTo(parameter);
//    // 3. Make this instance a listener to the external parameter.
//    _value.addListener(this,
//                           &Slider<Type>::_onValueChanged,
//                           std::numeric_limits<Type>::lowest());
//}


template<typename Type>
Type Slider<Type>::operator=(Type v)
{
    _value = v;
    return v;
}


template<typename Type>
Slider<Type>::operator const Type& ()
{
    return _value;
}


template<typename Type>
void Slider<Type>::setMin(const Type& min)
{
    _value.setMin(min);
}


template<typename Type>
Type Slider<Type>::getMin() const
{
    return _value.getMin();
}


template<typename Type>
void Slider<Type>::setMax(const Type& max)
{
    _value.setMax(max);
}


template<typename Type>
Type Slider<Type>::getMax() const
{
    return _value.getMax();
}


typedef Slider<int> IntSlider;
typedef Slider<float> FloatSlider;
typedef Slider<double> DoubleSlider;


} } // ofx::MUI
