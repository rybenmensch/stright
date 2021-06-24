/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "ReferenceCountedBuffer.h"
#include "util.h"
#include "curve.h"
#include "granulator.h"

//==============================================================================
/**
*/
class StrightAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    StrightAudioProcessor();
    ~StrightAudioProcessor() override;
    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;
    
    juce::AudioBuffer<float>& getWaveForm();
    void processMidi(juce::MidiBuffer& buffer);
    std::atomic<int>& getSampleCount() { return mSampleCount; }
    juce::String chosenPath {""};
        
    curve scanCurve;
    std::vector<std::vector<float>> scList;
    std::vector<std::vector<float>> scDefault{
        {0., 0., 0.},
        {1., 1000, 0.}
    };
    curve grainCurve;
    std::vector<std::vector<float>> gcList;
    std::vector<std::vector<float>> gcDefault{
        {0, 0, 0},
        {1, 1000, 0}
    };
    curve pitchCurve;
    std::vector<std::vector<float>> pcList;
    std::vector<std::vector<float>> pcDefault{
          {0, 0, 0},
          {1, 1000, 0}
    };
    curve volumeCurve;
    std::vector<std::vector<float>> vcList;
    std::vector<std::vector<float>> vcDefault{
        {1., 1., 0.},
        {1., 1000., 0.}
    };
    curve fmCurve;
    std::vector<std::vector<float>> fmcList;
    std::vector<std::vector<float>> fmcDefault{
        {1., 1., 0.},
        {1., 1000., 0.}
    };
    curve filterCurve;
    std::vector<std::vector<float>> fcList;
    std::vector<std::vector<float>> fcDefault{
        {1., 1., 0.},
        {1., 1000., 0.}
    };
    
    util::genlibphasor<float> curvesPhasor[2];
    util::change<float> change[2];
    
    std::atomic<bool> hasStream {false};
    std::vector<grainStream> stream;
        
    float duration {1.};
    float grainsize {4.};
    float peak {0.};
    float playback {1.};
    float volume {0.};
    util::slide<float> volSlide[2];
    util::rampsmooth volSmooth[2];
    float masterVolume {0.};
    
    float mGrainsize {0};
    float mPeak {0};
    float mPlayback {0};
    float mVolume {0};
    
    float mSamplerate {44100.};
    juce::AudioFormatManager mFormatManager;
    std::unique_ptr<juce::AudioFormatReaderSource> readerSource;
    juce::ReferenceCountedArray<ReferenceCountedBuffer> buffers;
    ReferenceCountedBuffer::Ptr currentBuffer { nullptr };
    std::atomic<bool> hasCopy {false};
    juce::AudioBuffer<float> copy;
private:

    std::atomic<bool> trigger {false};
    std::atomic<int> mSampleCount { 0 };
    
//==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (StrightAudioProcessor)
};
