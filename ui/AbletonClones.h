
#pragma once
#include "JuceHeader.h"
#include "../util.h"
#include "../look_and_feel/RSLColours.h"

//border colour
class ModamtNumbox : public Component
{
public:
    ModamtNumbox() : ModamtNumbox(RSL::black, RSL::selectionBlue,
                                  "%",
                                  -100., 100., 0.) {}
    ModamtNumbox(juce::Colour bgC, juce::Colour fC,
                 juce::String u,
                 float minV, float maxV, float resetV) :
    onValueChanged(nullptr),
    bgColour(bgC), fillColour(fC),
    unit(u),
    min(minV), max(maxV), resetVal(resetV), val(resetV)
    {
        setRepaintsOnMouseActivity(true);
        setWantsKeyboardFocus(true);
        setMouseClickGrabsKeyboardFocus(true);
        incStep = (max - min)/128.f;

    }
    ~ModamtNumbox() {}
    std::function<void()> onValueChanged;
    float getValue()
    {
        return val;
    }
private:
    void paint(juce::Graphics& g) override
    {
        width = getWidth();
        height = getHeight();
        auto middle = width/2.f;
        float off = 1.f;
        if(hasFocus){
            g.setColour(RSL::white);
            g.fillRect(0.f, 0.f, width, height);
            g.setColour(RSL::black);
            g.fillRect(off, off, width-2*off, height-2*off);
        }else{
            g.fillAll(RSL::black);
        }
        
        if(!input){
            g.setColour(RSL::selectionBlue);
            auto sVal = util::scale(val, min, max, 2*off, width-2*off);
            if(val>0.f){
                g.fillRect(middle, 2*off, sVal-middle, height-4*off);
            }else if(val<0.f){
                g.fillRect(sVal, 2*off, middle-sVal, height-4*off);
            }
        }

        g.setColour(RSL::white);
        g.setFont(9.5f);
        char buffer[100];
        if(input){
            sprintf(buffer,"%s", str.c_str());
        }else
            sprintf(buffer ,"%.1f %%", val);
        g.drawFittedText(juce::String(buffer), getLocalBounds(), juce::Justification::centred, 1);
    }
    
    void modifyVal(float mult, bool isPrecise)
    {
        float inc = incStep;
        inc /= isPrecise?2.:1.;
        inc *= mult;
        val += inc;
        val = util::clamp(val, min, max);
        if(onValueChanged)
            onValueChanged();
    }
    
    void mouseDown(const MouseEvent & e) override
    {
        //set cursors etc
        auto x = e.position.getX();
        auto y = e.position.getY();
        previousY = y;
        
        setMouseCursor(juce::MouseCursor::NoCursor);
        mX = x;
        mY = y;
    }
    
    void mouseUp(const MouseEvent& e) override
    {
        setMouseCursor(juce::MouseCursor::NormalCursor);
        auto [aX, aY] = Desktop::getInstance().getMousePosition();
        auto oX = aX - e.position.getX();
        auto oY = aY - e.position.getY();
        auto p = juce::Point(oX+mX, oY+mY);
        Desktop::getInstance().getMainMouseSource().setScreenPosition(p);
    }
    
    void mouseDrag(const MouseEvent& e) override
    {
        //schauen wieviel delta und dann skalieren?
        auto y = e.position.getY();
        auto deltaY = y - previousY;
        auto isPrecise = e.mods.isShiftDown();
        if(deltaY){
            auto sign = deltaY<0?1.:-1.;
            modifyVal(sign, isPrecise);
        }
        repaint();
        previousY = y;
    }
    
    void mouseDoubleClick(const MouseEvent& e) override
    {
        val = resetVal;
        repaint();
        if(onValueChanged)
            onValueChanged();
    }
    
    bool keyPressed(const juce::KeyPress& key) override
    {
        auto k = key.getKeyCode();
        auto isPrecise = key.getModifiers().isShiftDown();
        
        if(k == juce::KeyPress::leftKey|| k == juce::KeyPress::downKey)
            modifyVal(-1., isPrecise);
        else if(k == juce::KeyPress::rightKey|| k == juce::KeyPress::upKey)
            modifyVal(1., isPrecise);
        
        if(!input){
            str.clear();
        }

        //0-9 , - , . , backspace
        if((k>='-' && k<='9' && k!= '/') || k==juce::KeyPress::backspaceKey){
            input = true;
            if(k==juce::KeyPress::backspaceKey&& str.length())
                str.pop_back();
            else
                str+=k;

            val = std::atof(str.c_str());
        }
        
        if(k == juce::KeyPress::returnKey){
            input = false;
            val = util::clamp(val, min, max);;

        }
        repaint();
        return true;
    }
    
    void focusGained(FocusChangeType cause) override
    {
        hasFocus = true;
        repaint();
    }
    
    void focusLost(FocusChangeType cause) override
    {
        hasFocus = false;
        input = false;
        val = util::clamp(val, min, max);;
        repaint();
    }

    float incStep {0.};
    float width {0.};
    float height {0.};
    juce::Colour bgColour;
    juce::Colour fillColour;
    juce::String unit;
    float min;
    float max;
    float resetVal;
    float val       {0.};
    float previousY {0.};
    float mX        {0.};
    float mY        {0.};
    bool hasFocus {false};
    bool input {false};
    std::string str;
};
