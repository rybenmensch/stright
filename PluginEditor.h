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

//==============================================================================
/**
*/
class StrightAudioProcessorEditor : public juce::AudioProcessorEditor,
private juce::Slider::Listener, private juce::Thread
{
public:
    StrightAudioProcessorEditor (StrightAudioProcessor&);
    ~StrightAudioProcessorEditor() override;

    //==============================================================================
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
    
    void run() override;
    void checkForBuffersToFree();
    void checkForPathToOpen();
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (StrightAudioProcessorEditor)
};
