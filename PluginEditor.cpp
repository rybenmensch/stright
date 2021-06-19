/*
 ==============================================================================
 
 This file contains the basic framework code for a JUCE plugin editor.
 
 ==============================================================================
 */

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
StrightAudioProcessorEditor::StrightAudioProcessorEditor (StrightAudioProcessor& p)
    : AudioProcessorEditor (&p),
    Thread("Background Thread"),
    audioProcessor (p),
    mWaveThumbnail(p),
    scanCurve(RSL::curveCandycane[0]),
    grainCurve(RSL::curveCandycane[1]),
    pitchCurve(RSL::curveCandycane[2]),
    volumeCurve(RSL::curveCandycane[3])
{
    startThread();

    //setLookAndFeel(&defaultLookAndFeel);
    addAndMakeVisible(mWaveThumbnail);
    scanCurve.setVector(&(audioProcessor.scList));
    addAndMakeVisible(scanCurve);
    grainCurve.setVector(&(audioProcessor.gcList));
    addAndMakeVisible(grainCurve);
    pitchCurve.setVector(&(audioProcessor.pcList));
    addAndMakeVisible(pitchCurve);
    volumeCurve.setVector(&(audioProcessor.vcList));
    addAndMakeVisible(volumeCurve);
    auto boxpos = juce::Slider::TextEntryBoxPosition::NoTextBox;
    auto style = juce::Slider::RotaryVerticalDrag;
    
    sDuration.setSliderStyle(style);
    sDuration.setTextBoxStyle(boxpos, true, 0, 0); sDuration.setTextBoxIsEditable(false);
    sDuration.setRange(0.1, 10., 0.01); sDuration.setValue(1.);
    sDuration.setPopupDisplayEnabled(true, true, this);
    sDuration.setTextValueSuffix(" Duration");
    sDuration.addListener(this);
    addAndMakeVisible(sDuration);
    
    sGrainsize.setSliderStyle(style);
    sGrainsize.setTextBoxStyle(boxpos, true, 0, 0); sGrainsize.setTextBoxIsEditable(false);
    sGrainsize.setRange(1, 100., 0.1); sGrainsize.setValue(4.);
    sGrainsize.setPopupDisplayEnabled(true, true, this);
    sGrainsize.setTextValueSuffix(" Grainsize");
    sGrainsize.addListener(this);
    addAndMakeVisible(sGrainsize);
    
    sPeak.setSliderStyle(style);
    sPeak.setTextBoxStyle(boxpos, true, 0, 0); sPeak.setTextBoxIsEditable(false);
    sPeak.setRange(0., 1., 0.01); sPeak.setValue(0.);
    sPeak.setPopupDisplayEnabled(true, true, this);
    sPeak.setTextValueSuffix(" Peak");
    sPeak.addListener(this);
    addAndMakeVisible(sPeak);
    
    sPlayback.setSliderStyle(style);
    sPlayback.setTextBoxStyle(boxpos, true, 0, 0); sPlayback.setTextBoxIsEditable(false);
    sPlayback.setRange(-4., 4., 0.1); sPlayback.setValue(1.);
    sPlayback.setPopupDisplayEnabled(true, true, this);
    sPlayback.setTextValueSuffix(" Playback");
    sPlayback.addListener(this);
    addAndMakeVisible(sPlayback);
    
    sVolume.setSliderStyle(style);
    sVolume.setTextBoxStyle(boxpos, true, 0, 0); sVolume.setTextBoxIsEditable(false);
    sVolume.setRange(-70, 0., 0.1); sVolume.setValue(0.);
    sVolume.setPopupDisplayEnabled(true, true, this);
    sVolume.setTextValueSuffix(" Volume");
    sVolume.addListener(this);
    addAndMakeVisible(sVolume);
    
    sMasterVolume.setSliderStyle(style);
    sMasterVolume.setTextBoxStyle(boxpos, true, 0, 0); sMasterVolume.setTextBoxIsEditable(false);
    sMasterVolume.setRange(-70, 6., 0.1); sMasterVolume.setValue(0.);
    sMasterVolume.setPopupDisplayEnabled(true, true, this);
    sMasterVolume.setTextValueSuffix(" Master Volume");
    sMasterVolume.addListener(this);
    addAndMakeVisible(sMasterVolume);
    
    sModGrainsize.setSliderStyle(juce::Slider::LinearBar);
    sModGrainsize.setTextBoxStyle(boxpos, true, 0, 0); sModGrainsize.setTextBoxIsEditable(false);
    sModGrainsize.setRange(-100, 100, 1);sModGrainsize.setValue(0);
    sModGrainsize.setPopupDisplayEnabled(true, true, this);
    sModGrainsize.setTextValueSuffix(" Grainsize Mod");
    sModGrainsize.addListener(this);
    addAndMakeVisible(sModGrainsize);
    
    sModPeak.setSliderStyle(juce::Slider::LinearBar);
    sModPeak.setTextBoxStyle(boxpos, true, 0, 0); sModPeak.setTextBoxIsEditable(false);
    sModPeak.setRange(-100, 100, 1);sModPeak.setValue(0);
    sModPeak.setPopupDisplayEnabled(true, true, this);
    sModPeak.setTextValueSuffix(" Peak Mod");
    sModPeak.addListener(this);
    addAndMakeVisible(sModPeak);
    
    sModPlayback.setSliderStyle(juce::Slider::LinearBar);
    sModPlayback.setTextBoxStyle(boxpos, true, 0, 0); sModPlayback.setTextBoxIsEditable(false);
    sModPlayback.setRange(-100, 100, 1);sModPlayback.setValue(0);
    sModPlayback.setPopupDisplayEnabled(true, true, this);
    sModPlayback.setTextValueSuffix(" Playback Mod");
    sModPlayback.addListener(this);
    addAndMakeVisible(sModPlayback);
    
    sModVolume.setSliderStyle(juce::Slider::LinearBar);
    sModVolume.setTextBoxStyle(boxpos, true, 0, 0); sModVolume.setTextBoxIsEditable(false);
    sModVolume.setRange(-100, 100, 1);sModVolume.setValue(0);
    sModVolume.setPopupDisplayEnabled(true, true, this);
    sModVolume.setTextValueSuffix(" Volume Mod");
    sModVolume.addListener(this);
    addAndMakeVisible(sModVolume);
    
    
    setSize (920, 620);
}

StrightAudioProcessorEditor::~StrightAudioProcessorEditor()
{
    setLookAndFeel(nullptr);
    stopThread(4000);
}

//==============================================================================
void StrightAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll(RSL::black);
    g.setColour(RSL::white);
    g.setFont(11.5f);
    //class machen, die automatisch anhand grösse des
    //texts die linie zeichnet
    g.drawText("SCAN", 5, 130, 39, 20, juce::Justification::left);
    g.drawText("GRAIN", 233, 130, 61, 20., juce::Justification::left);
    g.drawText("PITCH", 461, 130, 147, 20., juce::Justification::left);
    g.drawText("VOLUME", 689, 130, 147, 20, juce::Justification::left);
    
    g.setColour(juce::Colour::fromFloatRGBA(0.137, 0.137, 0.13, 0.600));
    g.fillRect(461, 130, 219, 234.5);
}

void StrightAudioProcessorEditor::resized()
{
    mWaveThumbnail.setBounds(233., 5., 675., 115.);
    scanCurve.setBounds(5, 163, 200, 118);
    grainCurve.setBounds(233, 163, 200, 118);
    pitchCurve.setBounds(461, 163, 200, 118);
    volumeCurve.setBounds(689, 163, 200, 118);
    
    sDuration.setBounds(5., 316., 41., 48.);
    sGrainsize.setBounds(233., 316., 40., 48.);
    sPeak.setBounds(275., 316., 40., 48.);
    sPlayback.setBounds(316, 316., 40., 48.);
    sVolume.setBounds(689., 316., 40., 48.);
    sMasterVolume.setBounds(849., 316., 40., 48.);
    
    sModGrainsize.setBounds(233., 287., 41., 15.);
    sModPeak.setBounds(275., 287., 41., 15.);
    sModPlayback.setBounds(316., 287., 41., 15.);
    sModVolume.setBounds(689., 287., 41., 15.);
}

void StrightAudioProcessorEditor::sliderValueChanged(juce::Slider* slider)
{
    if(slider == &sDuration){
        audioProcessor.duration = sDuration.getValue();
    }else if(slider == &sGrainsize){
        audioProcessor.grainsize = sGrainsize.getValue();
    }else if(slider == &sPeak){
        audioProcessor.peak = sPeak.getValue();
    }else if(slider == &sPlayback){
        audioProcessor.playback = sPlayback.getValue();
    }else if(slider == &sVolume){
        audioProcessor.volume = sVolume.getValue();
    }else if(slider == &sMasterVolume){
        audioProcessor.masterVolume = sMasterVolume.getValue();
    }else if(slider == &sModGrainsize){
        audioProcessor.mGrainsize = sModGrainsize.getValue();
    }else if(slider == &sModPeak){
        audioProcessor.mPeak = sModPeak.getValue();
    }else if(slider == &sModPlayback){
        audioProcessor.mPlayback = sModPlayback.getValue();
    }else if(slider == &sModVolume){
        audioProcessor.mVolume = sModVolume.getValue();
    }
}

//Thread stuff
void StrightAudioProcessorEditor::run(){
    while(!threadShouldExit()){
        checkForBuffersToFree();
        checkForPathToOpen();
        wait(500);
    }
}

void StrightAudioProcessorEditor::checkForBuffersToFree(){
    for(auto i=audioProcessor.buffers.size();--i>=0;){
        ReferenceCountedBuffer::Ptr buffer (audioProcessor.buffers.getUnchecked(i));
        
        if(buffer->getReferenceCount() == 2){
            audioProcessor.buffers.remove(i);
        }
    }
}

void StrightAudioProcessorEditor::checkForPathToOpen(){
    juce::String pathToOpen;
    pathToOpen.swapWith(audioProcessor.chosenPath);
    
    if(pathToOpen.isNotEmpty()){
        juce::File file (pathToOpen);
        std::unique_ptr<juce::AudioFormatReader> reader(audioProcessor.mFormatManager.createReaderFor(file));
        
        if(reader.get() != nullptr){
            if(reader->numChannels>2){
                DBG("oops");
            }else{
                ReferenceCountedBuffer::Ptr newBuffer = new ReferenceCountedBuffer(file.getFileName(),
                                                                                                 (int) reader->numChannels,
                                                                                                 (int) reader->lengthInSamples);
                              
                reader->read(newBuffer->getAudioSampleBuffer(), 0, (int)reader->lengthInSamples, 0, true, true);
                audioProcessor.currentBuffer = newBuffer;
                audioProcessor.buffers.add(newBuffer);
            }
        }
    }
}
