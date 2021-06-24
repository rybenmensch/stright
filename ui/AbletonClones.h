
#pragma once
#include "JuceHeader.h"
#include "../util.h"
#include "../look_and_feel/RSLColours.h"

class ModamtNumbox : public Component
{
public:
    //polarity 0=uni, 1=bipolar
    ModamtNumbox(juce::Colour bgC,
                 juce::Colour fC,
                 juce::Colour bC,
                 juce::String u,
                 float minV, float maxV, float resetV,
                 int p) :
    onValueChanged(nullptr),
    bgColour(bgC), fillColour(fC), borderColour(bC),
    unit(u),
    min(minV), max(maxV), resetVal(resetV), val(resetV),
    polarity(p)
    {
        setRepaintsOnMouseActivity(true);
        setWantsKeyboardFocus(true);
        setMouseClickGrabsKeyboardFocus(true);
        incStep = (max - min)/128.f;
    }
    ModamtNumbox() : ModamtNumbox(RSL::black,
                                  RSL::selectionBlue,
                                  juce::Colours::transparentBlack,
                                  "%",
                                  -100., 100., 0., 1) {}
    ~ModamtNumbox() {}
    std::function<void()> onValueChanged;
    float getValue() { return val; }
private:
    void paint(juce::Graphics& g) override
    {
        /*
         x = borderFocus     //wird mit focus sichtbar
         o = border         //kann unsichtbar sein
         _ = drawZone/fillColour
         xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
         xoooooooooooooooooooooooooooooooooooooox
         xo____________________________________ox
         xo____________________________________ox
         xo____________________________________ox
         xoooooooooooooooooooooooooooooooooooooox
         xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
         ^^wow huere schön!
         */
        width = getWidth();
        height = getHeight();
        float offset = 1.f;
        juce::Rectangle<float> borderFocus {width, height};
        
        if(hasFocus){
            g.setColour(RSL::white);
            g.drawRect(borderFocus);
        }
        
        auto border = borderFocus.reduced(offset);
        g.setColour(borderColour);
        g.drawRect(border);

        auto drawZone = border.reduced(offset);
        g.setColour(bgColour);
        g.fillRect(drawZone);
        
        if(!input){
            g.setColour(RSL::selectionBlue);
            g.setColour(fillColour);
            auto dWidth = util::scale(val, min, max, 0.f, drawZone.getWidth());
			
            if(polarity == 0){
                auto drawBar = drawZone.withWidth(dWidth);
                g.fillRect(drawBar);
            }else if (polarity == 1){
				auto mVal = (min+max)/2.f;
                auto mCoord = drawZone.getWidth()/2.f;
                if(val>=mVal){
					auto dWidth = util::scale(val, mVal, max, 0.f, drawZone.getWidth())/2.f;
					auto drawBar = drawZone.withX(mCoord).withWidth(dWidth);
                    g.fillRect(drawBar);
					//g.fillRect(mCoord, drawZone.getY(), dWidth, drawZone.getHeight());
                }else{
                }
            }
        }
        
        g.setColour(RSL::white);
        g.setFont(9.5f);
        char buffer[100];
        //maybe juce::String has formatting options?
        if(input){
            sprintf(buffer,"%s", inputStr.c_str());
        }else{
            float ipart;
            if(std::modf(val, &ipart))
               sprintf(buffer ,"%.1f %%", val);
            else
			   sprintf(buffer, "%.0f %%", ipart);
            
        }
        g.drawFittedText(juce::String(buffer), getLocalBounds(), juce::Justification::centred, 1);
    }
    
    void modifyVal(float mult, bool isPrecise)
    {
        float inc = incStep;
        inc /= isPrecise?4.:1.;
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
        Desktop::getInstance().getMainMouseSource().setScreenPosition(p.toFloat());
    }
    
    void mouseDrag(const MouseEvent& e) override
    {
        auto y = e.position.getY();
        auto deltaY = y - previousY;
        auto isPrecise = e.mods.isShiftDown();
        if(deltaY){
            modifyVal(deltaY*-1., isPrecise);
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
            inputStr.clear();
        }

        //0-9 , - , . , backspace
        if((k>='-' && k<='9' && k!= '/') || k==juce::KeyPress::backspaceKey){
            input = true;
            if(k==juce::KeyPress::backspaceKey&&inputStr.length())
                inputStr.pop_back();
            else
                inputStr+=k;

            val = std::atof(inputStr.c_str());
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
    juce::Colour borderColour;
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
    std::string inputStr;
    int polarity;
};

class LiveDial
{
public:
    LiveDial() {};
    ~LiveDial() {};
private:
};
