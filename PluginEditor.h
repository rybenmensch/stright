/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "./look_and_feel/RSLColours.h"
#include "look_and_feel/default_look_and_feel.h"
#include "./EnvelopeEditor/EnvelopeEditor.h"
#include "util.h"
#include "ui/ui_all.h"

#define ADD_VISIBLE_LAMBDA(arg)\
addAndMakeVisible(arg);\
arg.onValueChanged = [this](){\
    testCallback(&arg);\
};

//==============================================================================
/**
*/
class StrightAudioProcessorEditor : public juce::AudioProcessorEditor,
private juce::Slider::Listener, private juce::Thread
{
public:
    StrightAudioProcessorEditor (StrightAudioProcessor&);
    ~StrightAudioProcessorEditor() override;
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    void sliderValueChanged(juce::Slider *slider) override;
    void testCallback(juce::Component *c);
    StrightAudioProcessor& audioProcessor;
    //DefaultLookAndFeel defaultLookAndFeel;
    juce::AudioTransportSource transportSource;
    WaveThumbnail mWaveThumbnail;
    RangeSelector waveSelector;
    EnvelopeEditor scanCurve;
    EnvelopeEditor grainCurve;
    EnvelopeEditor pitchCurve;
    EnvelopeEditor volumeCurve;
    EnvelopeEditor fmCurve;
    EnvelopeEditor filterCurve;
    
    juce::Slider sDuration;
    juce::Slider sGrainsize;
    juce::Slider sPeak;
    juce::Slider sPlayback;
    juce::Slider sVolume;
    juce::Slider sMasterVolume;
    
    ModamtNumbox sModGrainsize;
    ModamtNumbox sModPeak;
    ModamtNumbox sModPlayback;
    ModamtNumbox sModVolume;
    ModamtNumbox sModVelocity;
    
    ModamtNumbox sStart;
    ModamtNumbox sEnd;
    ModamtNumbox sKey;
    
    RSLHeader generalHeader;
    RSLHeader scanHeader;
    RSLHeader grainHeader;
    RSLHeader pitchHeader;
    RSLHeader volumeHeader;
    RSLHeader fmHeader;
    RSLHeader filterHeader;
    
    std::vector<std::unique_ptr<RSLModLine>> ml;
    std::vector<juce::Rectangle<int>> mlPos = {
        /*
        {21, 526, 8, 9},
        {21, 550, 8, 9},
        {181, 526, 8, 9},
        {181, 550, 8, 9},
        {249, 526, 8, 9},
        {249, 550, 8, 9},
        {291, 526, 8, 9},
        {291, 550, 8, 9},
        */
        {249, 278, 8, 9},
        {249, 302, 8, 9},
        {291, 278, 8, 9},
        {291, 302, 8, 9},
        {332, 278, 8, 9},
        {332, 302, 8, 9},
        {705, 278, 8, 9},
        {705, 302, 8, 9},
        {357, 335, 23, 15}
    };
    
    void run() override;
    void checkForBuffersToFree();
    void checkForPathToOpen();
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (StrightAudioProcessorEditor)
};
