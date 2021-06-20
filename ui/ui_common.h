//
//  ui_common.h
//  stright
//
//  Created by Manolo Müller on 19.06.21.
//

#ifndef ui_common_h
#define ui_common_h
#include "JuceHeader.h"
#include "../util.h"
#include "../look_and_feel/RSLColours.h"

class RangeSelector : public Component
{
public:
    RangeSelector(juce::Colour &c) : colour(c)
    {
        setRepaintsOnMouseActivity(true);
    }
    ~RangeSelector() {};
    void paint(juce::Graphics& g) override
    {
        height = getHeight();
        width = getWidth();
        g.fillAll(RSL::black);
        
        g.setColour(RSL::selectionBlue);
        auto drawSX = util::clamp(startX, 0.f, width-1);
        auto drawEX = util::clamp(endX-startX, 0.f, width-1);
        if(startX<=0.f)
            drawEX = util::clamp(endX, 0.f, width-1);
        drawEX = drawEX?drawEX:1.f;
        g.fillRect(drawSX, 0.f, drawEX, height);
    }
    
    void mouseMove(const MouseEvent &e) override
    {
        mouseEnter(e);
    }
    void mouseEnter(const MouseEvent &e) override
    {
        if(e.mods.isAltDown()){
            setMouseCursor(juce::MouseCursor::UpDownResizeCursor);
        }else if(e.mods.isCommandDown()){
            setMouseCursor(juce::MouseCursor::LeftRightResizeCursor);
        }else{
            setMouseCursor(juce::MouseCursor::NormalCursor);
        }
    }
    
    void mouseExit(const MouseEvent &e) override
    {
        setMouseCursor(juce::MouseCursor::NormalCursor);
    }
    void mouseUp(const MouseEvent &e) override
    {
        setMouseCursor(juce::MouseCursor::NormalCursor);

        auto [aX, aY] = Desktop::getInstance().getMousePosition();
        auto oX = aX - e.position.getX();
        auto oY = aY - e.position.getY();
        auto p = juce::Point(oX+mX, oY+mY);
        Desktop::getInstance().getMainMouseSource().setScreenPosition(p);
    }
    
    void mouseDown(const MouseEvent &e) override
    {
        auto x = e.position.getX();
        auto y = e.position.getY();
        mX = x;
        mY = y;
        
        //memoryX = x;
        if(e.mods.isShiftDown()){
            //find nearer of the startX and endX
            float dStartX = std::abs(startX - x);
            float dEndX = std::abs(endX - x);
            memoryX = (dEndX<dStartX)?startX:endX;
        }else if(e.mods.isAltDown()){
            previousY = y;
            setMouseCursor(juce::MouseCursor::NoCursor);
        }else if(e.mods.isCommandDown()){
            previousX = x;
            setMouseCursor(juce::MouseCursor::NoCursor);
        }
        else{
            endX = startX = memoryX = x;
        }
        onValueChange();
        repaint();
    }

    void mouseDrag(const MouseEvent &e) override
    {
        auto x = e.position.getX();
        auto y = e.position.getY();
        
        if(e.mods.isAltDown()){
            auto deltaY = y - previousY;
            //prevent cursor from moving when making field smaller and start and end are already the same.

            if(startX>=endX && deltaY>0.f){
                startX = endX;
                deltaY = 0;
            }
  
            startX += deltaY;
            endX -= deltaY;
            startX = util::clamp(startX, 0.f, width);
            endX = util::clamp(endX, 0.f, width);
            previousY = y;
        }else if(e.mods.isCommandDown()){
            auto deltaX = x - previousX;
            endX += deltaX;
            startX += deltaX;
            previousX = x;
        }else{ //kein modifier oder shift
            mX = x;
            mY = y;

            if(x<memoryX){
                startX = util::clamp(x, 0.f, width);
                endX = memoryX;
            }else{
                startX = memoryX;
                endX = util::clamp(x, 0.f, width);
            }
        }
        onValueChange();
        repaint();
    }
    
    void setStart(float f)
    {
        startX = util::clamp(f, 0.f, 1.f)*width;
    }
    
    void setEnd(float f)
    {
        endX = util::clamp(f, 0.f, 1.f)*width;
    }
    
    std::tuple<float, float> getValues()
    {
        return std::make_tuple(getStart(), getEnd());
    }
    std::function<void()> onValueChange;

private:
    float getStart()
    {
        return util::clamp(startX, 0.f, width)/width;
    }
    
    float getEnd()
    {
        return util::clamp(endX, 0.f, width)/width;
    }
    juce::Colour colour;

    float height    {0.};
    float width     {0.};
    
    float startX    {0.};
    float endX      {0.};
    
    float memoryX   {0.};
    float previousX {0.f};
    float previousY {0.};
    
    float sWidth    {0.};
    //mouse reset coordinates
    float mX        {0.};
    float mY        {0.};
};
#endif /* ui_common_h */
