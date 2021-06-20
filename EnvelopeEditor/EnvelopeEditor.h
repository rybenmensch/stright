#pragma once
#include "JuceHeader.h"
#include "../util.h"
#include "../look_and_feel/RSLColours.h"
#include "../curve.h"

using namespace juce;

class EnvelopePoint : public Component
{
public:
    EnvelopePoint(const Point<float> &p) : x(p.getX()), y(p.getY()) {};
    EnvelopePoint()
    {
        setRepaintsOnMouseActivity(true);
    };
    ~EnvelopePoint() {};
    void paint(Graphics& g) override
    {
    }
    void mouseDown(const MouseEvent& e) override
    {
        //sollte sofort verschwinden und nicht warten
        //bis mouseUp
        if(e.mods.isShiftDown()){
            coordChanged = true;
            shouldDelete = true;
            shouldCalculate = true;
            repaint();
        }
    }
    void mouseDrag(const MouseEvent& e) override
    {
        x = e.position.getX() + getX();
        y = e.position.getY() + getY();
        coordChanged = true;
        repaint();
    }
    void mouseUp(const MouseEvent& e) override
    {
        coordChanged = false;
        shouldCalculate = true;
        repaint();
    }
    bool coordChanged {false};
    bool shouldDelete {false};
    bool shouldCalculate {true};
    float x {0};
    float y {0};
private:
};

class EnvelopePointContainer
{
public:
    using epPtr = std::unique_ptr<EnvelopePoint>;
    EnvelopePointContainer() {};
    ~EnvelopePointContainer() {};
    EnvelopePoint &insert(const juce::Point<float> &p)
    {
        points.emplace_back(std::make_unique<EnvelopePoint>(p));
        last = &*points.back(); //what the fuck?

        //starting to like lambdas!
        std::sort(points.begin(), points.end(), [] (epPtr &p1, epPtr &p2)
                  ->bool{return p1->x<p2->x;});
        return *(points.back());
    }
    EnvelopePoint &operator[](int i)
    {
        return *points[i];
    }
    void remove(int i)
    {
        points.erase(points.begin()+i);
    }
    
    std::vector<epPtr>::size_type
    size()
    {
        return points.size();
    }
    
    void calculateCurve(float width, float height, float hr)
    {
        float r = hr*2;
        curvePoints.clear();
        for(auto &p : points){
            float x = p->getX()/(width-hr) * 1000.f;
            float y = 1. - (p->getY()/(height-r));
            curvePoints.push_back({y, x, 0.});
            /*
            DBG(String(curvePoints.back()[0])+"\t"+
                String(curvePoints.back()[1])+"\t"+
                String(curvePoints.back()[2]));
            */
        }
    }
    std::vector<std::vector<float>> &getCurvePoints()
    {
        return curvePoints;
    }
    EnvelopePoint *last;
private:
    std::vector<epPtr> points;
    std::vector<std::vector<float>> curvePoints;
};

class EnvelopeEditor : public Component
{
public:
    EnvelopeEditor(juce::Colour &c);
    ~EnvelopeEditor();
    
    void paint(juce::Graphics&) override;
    void mouseDown(const MouseEvent&) override;
    void mouseUp(const MouseEvent&) override;
    void mouseDrag(const MouseEvent&) override;
    void childBoundsChanged(Component *child) override;
    void setVector(std::vector<std::vector<float>> *v);

    //void mouseDoubleClick(const MouseEvent&) override;
    //bool keyPressed(const KeyPress&) override;
    //void mouseMove(const MouseEvent&) override;
    //void mouseEnter(const MouseEvent&) override;
    //void mouseExit(const MouseEvent&) override;
    //bool newPoint {false};
    bool shouldCalculate {false};
    int grid {1};
private:
    //std::vector<std::unique_ptr<EnvelopePoint>> evp;
    EnvelopePointContainer pointsContainer;
    EnvelopePoint *lastPoint;
    juce::Colour colour;
    std::vector<std::vector<float>> *vecptr;
};
