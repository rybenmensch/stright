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
    waveSelector(RSL::selectionBlue),
    scanCurve(RSL::curveCandycane[0]),
    grainCurve(RSL::curveCandycane[1]),
    pitchCurve(RSL::curveCandycane[2]),
    volumeCurve(RSL::curveCandycane[3]),
    fmCurve(RSL::curveCandycane[4]),
    filterCurve(RSL::curveCandycane[5]),
    sModVelocity(RSL::black, RSL::selectionBlue, RSL::white, "%", 0.f, 100.f, 50.f, 0)
{
    startThread();
    juce::Font labelfont {11.5, juce::Font::bold};
    generalHeader.set("GENERAL", RSL::white, labelfont); addAndMakeVisible(generalHeader);
    scanHeader.set("SCAN", RSL::white, labelfont); addAndMakeVisible(scanHeader);
    grainHeader.set("GRAIN", RSL::white, labelfont); addAndMakeVisible(grainHeader);
    pitchHeader.set("PITCH", RSL::white, labelfont); addAndMakeVisible(pitchHeader);
    volumeHeader.set("VOLUME", RSL::white, labelfont); addAndMakeVisible(volumeHeader);
    /*
    fmHeader.set("FM", RSL::white, labelfont); addAndMakeVisible(fmHeader);
    filterHeader.set("FILTER", RSL::white, labelfont); addAndMakeVisible(filterHeader);
    */
     
    scanCurve.setVector(&(audioProcessor.scList)); addAndMakeVisible(scanCurve);
    grainCurve.setVector(&(audioProcessor.gcList)); addAndMakeVisible(grainCurve);
    pitchCurve.setVector(&(audioProcessor.pcList)); addAndMakeVisible(pitchCurve);
    volumeCurve.setVector(&(audioProcessor.vcList)); addAndMakeVisible(volumeCurve);
    /*
    fmCurve.setVector(&(audioProcessor.fmcList)); addAndMakeVisible(fmCurve);
    filterCurve.setVector(&(audioProcessor.fcList)); addAndMakeVisible(filterCurve);
    */
    for(int i=0;i<mlPos.size();i++){
        ml.emplace_back(std::make_unique<RSLModLine>());
        ml[i]->rect = &(mlPos[i]);
        addAndMakeVisible(*ml[i]);
    }
    
    //setLookAndFeel(&defaultLookAndFeel);
    //mWavethumbnail sollte auch ein lambda haben
    addAndMakeVisible(mWaveThumbnail);
    //ADD_VISIBLE_LAMBDA(waveSelector)
    ADD_VISIBLE_LAMBDA(sModGrainsize);
    ADD_VISIBLE_LAMBDA(sModPeak);
    ADD_VISIBLE_LAMBDA(sModPlayback);
    ADD_VISIBLE_LAMBDA(sModVolume);
    ADD_VISIBLE_LAMBDA(sModVelocity);
    
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
    g.setColour(juce::Colour::fromFloatRGBA(0.137, 0.137, 0.13, 0.600));
    g.fillRect(461, 130, 219, 234.5);
}

void StrightAudioProcessorEditor::resized()
{
    generalHeader.setBounds(5, 5, 200, 20);
    scanHeader.setBounds(5, 130, 200, 20);
    grainHeader.setBounds(233, 130, 200, 20);
    pitchHeader.setBounds(461, 130, 200, 20);
    volumeHeader.setBounds(689, 130, 200, 20);
    fmHeader.setBounds(5, 387, 200, 20);
    filterHeader.setBounds(233, 387, 200, 20);
    
    mWaveThumbnail.setBounds(233., 5., 675., 115.);
    waveSelector.setBounds(233., 5., 675., 115.);
    scanCurve.setBounds(5, 163, 200, 118);
    grainCurve.setBounds(233, 163, 200, 118);
    pitchCurve.setBounds(461, 163, 200, 118);
    volumeCurve.setBounds(689, 163, 200, 118);
    /*
    fmCurve.setBounds(5., 411., 200., 118.);
    filterCurve.setBounds(233., 411., 200., 118.);
    */
    for(auto &l : ml){
        l->setBounds(*(l->rect));
    }
    
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
    sModVelocity.setBounds(741., 340., 41., 15.);
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
    }
}

void StrightAudioProcessorEditor::testCallback(juce::Component *c)
{
    if(c==&waveSelector){
        //auto [start, end] = waveSelector.getValues();
    }else if(c == &sModGrainsize){
        audioProcessor.mGrainsize = sModGrainsize.getValue();
    }else if(c == &sModPeak){
        audioProcessor.mPeak = sModPeak.getValue();
    }else if(c == &sModPlayback){
        audioProcessor.mPlayback = sModPlayback.getValue();
    }else if(c == &sModVolume){
        audioProcessor.mVolume = sModVolume.getValue();
    }else if(c== &sModVelocity){
        //
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
