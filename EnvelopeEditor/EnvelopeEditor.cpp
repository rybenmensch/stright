//
//  EnvelopeEditor.cpp
//  stright
//
//  Created by Manolo Müller on 15.06.21.
//

#include "EnvelopeEditor.h"

EnvelopeEditor::EnvelopeEditor(juce::Colour &c) : colour(c)
{
    //setWantsKeyboardFocus(true);
    //setMouseClickGrabsKeyboardFocus(true);
    setRepaintsOnMouseActivity(true);
    setInterceptsMouseClicks(true, true);
}

EnvelopeEditor::~EnvelopeEditor()
{

}

void EnvelopeEditor::paint(juce::Graphics& g)
{
    float width     = (float)getWidth();
    float height    = (float)getHeight();
    g.fillAll(RSL::black);
    
    g.setColour(RSL::curveGrid);
    
    g.drawLine(0, 0, width, 0);
    g.drawLine(0, height, width, height);
    //GRID
    grid = util::max(grid, 1);
    for(int i=0;i<=grid;i++){
        float gX = i*(width/(grid));
        g.drawLine(gX, 0., gX, height);
    }
    
    auto pAmt = pointsContainer.size();
    
    float r     = 10;
    float hr    = r/2;
    
    for(int i=0;i<pAmt;i++){
        auto &p = pointsContainer[i];
        if(p.shouldCalculate){
            pointsContainer.calculateCurve(getWidth(), getHeight(), hr);
            *vecptr = pointsContainer.getCurvePoints();
            p.shouldCalculate = false;
        }
        
        if(p.shouldDelete){
            removeChildComponent(&p);
            pointsContainer.remove(i);
            pAmt = pointsContainer.size();
            continue;
        }
        
        if(p.coordChanged){
            float x = util::clamp(p.x-hr, 0.f, width -  r);
            float y = util::clamp(p.y-hr, 0.f, height - r);
            if(pAmt>1){
                if(i==0){
                    x = util::clamp(x, 0.f,
                                    pointsContainer[i+1].x-hr);
                }else if(i==pAmt-1){
                    x = util::clamp(x, pointsContainer[i-1].x-hr,
                                    width -r);
                }else{
                    x = util::clamp(x,
                                    pointsContainer[i-1].x-hr,
                                    pointsContainer[i+1].x-hr);
                }
            }
            p.setBounds(x, y, 25, 25);
            p.x = x+hr;
            p.y = y+hr;
        }
        
        g.setColour(colour);
        g.fillEllipse(p.getX(), p.getY(), r, r);
    }

    if(pAmt>1){
        for(int i=0;i<pAmt-1;i++){
        
            auto &cP = pointsContainer[i];
            auto &nP = pointsContainer[i+1];
            g.drawLine(cP.getX()+hr, cP.getY()+hr,
                       nP.getX()+hr, nP.getY()+hr);
        }
    }
}

void EnvelopeEditor::mouseDown(const MouseEvent& event)
{
    pointsContainer.insert(event.position);
    lastPoint = pointsContainer.last;
    addAndMakeVisible(lastPoint);
    lastPoint->coordChanged = true;
    repaint();
}

void EnvelopeEditor::mouseUp(const MouseEvent& event)
{
    if(lastPoint->coordChanged){
        lastPoint->coordChanged = false;
        lastPoint->shouldCalculate = true;
        shouldCalculate = true;
    }
}

void EnvelopeEditor::mouseDrag(const MouseEvent& event)
{
    if(lastPoint->coordChanged){
        lastPoint->x = event.position.getX();
        lastPoint->y = event.position.getY();
        repaint();
    }
}

void EnvelopeEditor::childBoundsChanged(Component *child)
{
    //shouldCalculate = true;
    repaint();
}

void EnvelopeEditor::setVector(std::vector<std::vector<float>> *v)
{
    vecptr = v;
}
