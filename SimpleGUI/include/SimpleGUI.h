// MowaLibs
//
// Copyright (c) 2011, Marcin Ignac / marcinignac.com
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
//
// This code is intended to be used with the Cinder C++ library, http://libcinder.org
//
// Temptesta Seven font by Yusuke Kamiyamane http://p.yusukekamiyamane.com/fonts/
// "The fonts can be used free for any personal or commercial projects."

#pragma once

#include <vector>
#include "cinder/app/App.h"
#include "cinder/Text.h"
#include "cinder/gl/Texture.h"
#include "cinder/gl/TextureFont.h"

using namespace ci;
using namespace ci::app;

namespace mowa { namespace sgui {
	
//-----------------------------------------------------------------------------
	
class Control;
class FloatVarControl;
class DoubleVarControl;
class IntVarControl;
class BoolVarControl;
class EnumVarControl;
class ButtonControl;
class LabelControl;
class SeparatorControl;
class ColumnControl;
class PanelControl;
class TextureVarControl;
class ColorVarControl;

template<typename T, unsigned int _size>
class VectorVarControl;
typedef VectorVarControl<float,2>   Vec2fVarControl;
typedef VectorVarControl<float,3>   Vec3fVarControl;
typedef VectorVarControl<float,4>   Vec4fVarControl;

	
//-----------------------------------------------------------------------------

class SimpleGUI {
private:
	bool enabled;
	Vec2f	mousePos;
	std::vector<Control*> controls;
	Control* selectedControl;
	
	signals::connection	cnMouseDown;
	signals::connection	cnMouseUp;
	signals::connection cnMouseDrag;

	void	init(App* app);	
public:
	static ColorA darkColor;
	static ColorA lightColor;
	static ColorA bgColor;
	static ColorA textColor;
	static float spacing;
	static Vec2f padding;
	static Vec2f sliderSize;
	static Vec2f labelSize;
	static Vec2f separatorSize;
	static Font textFont;
    static ci::gl::TextureFontRef textureFont;
	
	enum {
		RGB,
		HSV
	};
public:
	SimpleGUI(App* app);
	~SimpleGUI();
	bool	isSelected() { return selectedControl != NULL; }
	Control* getSelectedControl() { return selectedControl; };
	std::vector<Control*>& getControls() { return controls; }	
	
	bool	onMouseDown(MouseEvent event);
	bool	onMouseUp(MouseEvent event);
	bool	onMouseDrag(MouseEvent event);
	
	void	draw();
	void	dump();
	void	save(std::string fileName = "");
	void	load(std::string fileName = "");
	
	bool	isEnabled();
	void	setEnabled(bool state);
	void	removeControl( Control* controlToRemove );


	FloatVarControl* 	addParam(const std::string& paramName, float* var, float min=0, float max=1, float defaultValue = 0);
    DoubleVarControl* 	addParam(const std::string& paramName, double* var, double min=0, double max=1, double defaultValue = 0);
	IntVarControl*		addParam(const std::string& paramName, int* var, int min=0, int max=1, int defaultValue = 0);
	BoolVarControl*		addParam(const std::string& paramName, bool* var, bool defaultValue = false, int groupId = -1);
    EnumVarControl*		addParam(const std::string& paramName, int* var, std::vector<std::string>& enumOptions);
	ColorVarControl*	addParam(const std::string& paramName, ColorA* var, ColorA const defaultValue = ColorA(0.0f, 1.0f, 1.0f, 1.0f), int colorModel = RGB);
    Vec2fVarControl*    addParam(const std::string& paramName, float* var, const Vec2f& min =Vec2f::zero(), const Vec2f& max =Vec2f(1.0f,1.0f));
    Vec3fVarControl*    addParam(const std::string& paramName, float* var, const Vec3f& min =Vec3f::zero(), const Vec3f& max =Vec3f(1.0f,1.0f,1.0f));
    Vec4fVarControl*    addParam(const std::string& paramName, float* var, const Vec4f& min =Vec4f::zero(), const Vec4f& max =Vec4f(1.0f,1.0f,1.0f,1.0f));
	TextureVarControl*	addParam(const std::string& paramName, gl::Texture* var, int scale = 1, bool flipVert =false);
	
	ButtonControl*		addButton(const std::string& buttonName);
	LabelControl*		addLabel(const std::string& labelName);	
	SeparatorControl*	addSeparator();	
	ColumnControl*		addColumn(int x = 0, int y = 0);	
	PanelControl*		addPanel();
	
	Control*			getControlByName(const std::string& name);
	
	static Vec2f		getStringSize(const std::string& str);		
	static Rectf		getScaledWidthRectf(Rectf rect, float scale);
};
	
//-----------------------------------------------------------------------------
	
	
class Control {
public:
	enum Type {
		FLOAT_VAR,
        DOUBLE_VAR,
		INT_VAR,
		BOOL_VAR,
        VECTOR_VAR,
		COLOR_VAR,
		TEXTURE_VAR,
		BUTTON,
		LABEL,
		SEPARATOR,
		COLUMN,
		PANEL
	};
	
	Vec2f	position;
	Rectf	activeArea;
	ColorA	bgColor;
	Type	type;
	std::string name;
    gl::Texture labelTexture;
	SimpleGUI* parentGui;
	
	Control(Type name, const std::string& label);	
	virtual ~Control() {};
	void setBackgroundColor(ColorA color);	

	void notifyUpdateListeners();
	virtual Vec2f draw(Vec2f pos) = 0;
	virtual std::string toString() { return ""; };
	virtual void fromString(std::string& strValue) {};
	virtual void onMouseDown(MouseEvent event) {};
	virtual void onMouseUp(MouseEvent event) {};
	virtual void onMouseDrag(MouseEvent event) {};
    virtual void updateLabel( const std::string& label );
};


class CallbackControl : public Control {
public:
    CallbackControl(Type name, const std::string& label) : Control(name,label) {}
    
	CallbackId		registerCallback( std::function<bool (void)> callback ) { return callbacks.registerCb( callback ); }
	template<typename T>
	CallbackId		registerCallback( T *obj, bool (T::*callback)(void) ) { return callbacks.registerCb( std::bind( std::mem_fun( callback ), obj ) ); }
	void			unregisterCallback( CallbackId id ) { callbacks.unregisterCb( id ); }
    
	bool triggerCallback();
    
private:
    CallbackMgr<bool (void)>		callbacks;

};
	
//-----------------------------------------------------------------------------
template<typename T>
class NumberVarControl : public CallbackControl {
public:	
	T* var;
	T min;
	T max;
public:
	NumberVarControl(Control::Type type, const std::string& name, T* var, T min=0, T max=1, T defaultValue = 0);
	float getNormalizedValue();
	void setNormalizedValue(const float value, const bool silent =false);
	Vec2f draw(Vec2f pos);
	std::string toString();
	void fromString(std::string& strValue);
	void onMouseDown(MouseEvent event);	
	void onMouseDrag(MouseEvent event);
};
    
class FloatVarControl : public NumberVarControl<float> {
public:
	FloatVarControl(const std::string& name, float* var, float min=0, float max=1, float defaultValue = 0)
    : NumberVarControl<float>( Control::FLOAT_VAR, name, var, min, max, defaultValue )
    {}
};
    
class DoubleVarControl : public NumberVarControl<double> {
public:
    DoubleVarControl(const std::string& name, double* var, double min=0, double max=1, double defaultValue = 0)
    : NumberVarControl<double>( Control::DOUBLE_VAR, name, var, min, max, defaultValue )
    {}
};

class IntVarControl : public NumberVarControl<int> {
public:
	IntVarControl(const std::string& name, int* var, int min=0, int max=1, int defaultValue = 0)
    : NumberVarControl<int>( Control::INT_VAR, name, var, min, max, defaultValue )
    {}
};

//-----------------------------------------------------------------------------

class BoolVarControl : public CallbackControl {
public:
	bool* var;
	int groupId;
public:
	BoolVarControl(const std::string& name, bool* var, bool defaultValue, int groupId);
	Vec2f draw(Vec2f pos);	
	std::string toString();	
	void fromString(std::string& strValue);
	void onMouseDown(MouseEvent event);
    
};

//-----------------------------------------------------------------------------
//
    
class EnumVarControl : public IntVarControl {
public:
    std::vector<std::string> enumOptions;
    std::vector<Rectf> elementAreas;
public:
    EnumVarControl(const std::string& name, int* var, std::vector<std::string>& options);
    Vec2f draw(Vec2f pos);
	void onMouseDown(MouseEvent event);
    void onMouseDrag(MouseEvent event);
};
    
//-----------------------------------------------------------------------------

template <typename T, unsigned int _size>
class VectorVarControl : public CallbackControl {
public:
    T* var;
    T max[_size];
    T min[_size];
    Rectf	elementArea[_size];
    int		activeTrack;
public:
    VectorVarControl(const std::string& name, T* var, T min[_size], T max[_size]);
    Vec2f draw(Vec2f pos);
    float getNormalizedValue(const int element);
	void setNormalizedValue(const int element, const float value, const bool silent =false);
    virtual std::string toString();
    virtual void fromString(std::string& strValue);    
    void onMouseDown(MouseEvent event);	
    void onMouseDrag(MouseEvent event);
};
    	
//-----------------------------------------------------------------------------

class ColorVarControl : public CallbackControl {
public:
	ColorA* var;
	Rectf	activeArea1;
	Rectf	activeArea2;
	Rectf	activeArea3;
	Rectf	activeArea4;	
	int		activeTrack;
	int		colorModel;
public:
	ColorVarControl(const std::string& name, ColorA* var, ColorA defaultValue, int colorModel);
	Vec2f draw(Vec2f pos);
	std::string toString();	//saved as "r g b a"
	void fromString(std::string& strValue); //expecting "r g b a"
	void onMouseDown(MouseEvent event);	
	void onMouseDrag(MouseEvent event);
    void setValueForElement(const int element, const float value, const bool silent =false);
    float getValueForElement(const int element);
};

	
//-----------------------------------------------------------------------------

class ButtonControl : public CallbackControl {
private:
	bool pressed;
public:
	ButtonControl(const std::string& name);
	Vec2f draw(Vec2f pos);
	void onMouseDown(MouseEvent event);
	void onMouseUp(MouseEvent event);
};

//-----------------------------------------------------------------------------
	
class LabelControl : public Control {
public:
	LabelControl(const std::string& name);
	void setText(const std::string& text);
	Vec2f draw(Vec2f pos);	
};
	
//-----------------------------------------------------------------------------
	
class SeparatorControl : public Control {
public:
	SeparatorControl();
	Vec2f draw(Vec2f pos);	
};

//-----------------------------------------------------------------------------

class ColumnControl : public Control {
public:
	int x;
	int y;
	ColumnControl(int x = 0, int y = 0);
	Vec2f draw(Vec2f pos);	
}; 
	
//-----------------------------------------------------------------------------
 
class PanelControl : public Control {
public:
	bool enabled;
	PanelControl();
	Vec2f draw(Vec2f pos);
};
	

//-----------------------------------------------------------------------------

class TextureVarControl : public CallbackControl {
public:
	gl::Texture* var;
	float scale;
	bool flipVert;	
	TextureVarControl(const std::string& name, gl::Texture* var, int scale, bool flipVert = false);
	Vec2f draw(Vec2f pos);	
    void resetTexture( gl::Texture* var );
    void onMouseDown(MouseEvent event);
};
		
//-----------------------------------------------------------------------------

} //namespace sgui
} //namespace vrg