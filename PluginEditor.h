/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "./look_and_feel/RSLColours.h"
//#include "look_and_feel/default_look_and_feel.h"
#include "WaveThumbnail.h"
#include "./EnvelopeEditor/EnvelopeEditor.h"
#include "util.h"


//==============================================================================
/**
*/
class StrightAudioProcessorEditor : public juce::AudioProcessorEditor,
private juce::Slider::Listener
{
public:
    StrightAudioProcessorEditor (StrightAudioProcessor&);
    ~StrightAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    void sliderValueChanged(juce::Slider *slider) override;
    StrightAudioProcessor& audioProcessor;
    //DefaultLookAndFeel defaultLookAndFeel;
    juce::AudioTransportSource transportSource;
    WaveThumbnail mWaveThumbnail;
    EnvelopeEditor scanCurve;
    EnvelopeEditor grainCurve;
    EnvelopeEditor pitchCurve;
    EnvelopeEditor volumeCurve;
    
    juce::Slider sDuration;
    juce::Slider sGrainsize;
    juce::Slider sPeak;
    juce::Slider sPlayback;
    juce::Slider sVolume;
    juce::Slider sMasterVolume;
    
    juce::Slider sModGrainsize;
    juce::Slider sModPeak;
    juce::Slider sModPlayback;
    juce::Slider sModVolume;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (StrightAudioProcessorEditor)
};
