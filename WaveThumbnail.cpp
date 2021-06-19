//
//  WaveThumbnail.cpp
//  stright
//
//  Created by Manolo Müller on 07.06.21.
//

#include "WaveThumbnail.h"


//==============================================================================
WaveThumbnail::WaveThumbnail (StrightAudioProcessor& p) : processor (p)
{
}

WaveThumbnail::~WaveThumbnail()
{
}

void WaveThumbnail::paint (juce::Graphics& g)
{
    float width     = (float)getWidth();
    float height    = (float)getHeight();
    g.fillAll(RSL::black);
    
    if(isInterested){
        g.setColour(RSL::white);
    }else{
        g.setColour(RSL::curveGrid);
    }
    
    for(int i=0;i<2;i++){
        float gX = i*width;
        g.drawLine(gX, 0., gX, height);
        float gY = i*height;
        g.drawLine(0, gY, width, gY);
    }
    
    auto waveform = processor.getWaveForm();
    
    if (waveform.getNumSamples() > 0)
    {
        juce::Path p;
        mAudioPoints.clear();
        
        auto ratio = waveform.getNumSamples() / getWidth();
        auto buffer = waveform.getReadPointer(0);
        
        //scale audio file to window on x axis
        for (int sample = 0; sample < waveform.getNumSamples(); sample+=ratio)
        {
            mAudioPoints.push_back(buffer[sample]);
        }
        
        g.setColour (juce::Colours::white);
        p.startNewSubPath (0, getHeight() / 2);
        
        //scale on y axis
        for (int sample = 0; sample < mAudioPoints.size(); ++sample)
        {
            auto point = juce::jmap<float> (mAudioPoints[sample], -1.0f, 1.0f, getHeight(), 0);
            p.lineTo (sample, point);
        }
        
        g.strokePath(p, juce::PathStrokeType (2));
    }
    else
    {
        g.setColour (juce::Colours::white);
        g.setFont (40.0f);
        g.drawFittedText ("DROPZONE", getLocalBounds(), juce::Justification::centred, 1);
    }
}

void WaveThumbnail::resized()
{
}

bool WaveThumbnail::isInterestedInFileDrag (const juce::StringArray& files)
{
    for (auto file : files){
        if (file.contains (".wav") || file.contains (".mp3") || file.contains (".aif"))
            isInterested = true;
        else
            isInterested = false;
    }
    return isInterested;
}

void WaveThumbnail::fileDragEnter (const juce::StringArray& files, int x, int y)
{
    if(isInterested){
        repaint();
    }
}

void WaveThumbnail::fileDragExit (const juce::StringArray& files)
{
    isInterested = false;
    repaint();
}


void WaveThumbnail::filesDropped (const juce::StringArray& files, int x, int y)
{
    for (auto file : files){
        if (isInterestedInFileDrag (file)){
            auto myFile = std::make_unique<juce::File>(file);
            auto path = myFile->getFullPathName();
            processor.chosenPath.swapWith(path);
            processor.doNotify();
            isInterested = false;
        }
    }
    repaint();
}
