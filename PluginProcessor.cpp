/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
using namespace juce;

//==============================================================================

StrightAudioProcessor::StrightAudioProcessor() :
    AudioProcessor(BusesProperties().withOutput("Output", juce::AudioChannelSet::stereo(), true)), Thread("Background Thread")
{
    mFormatManager.registerBasicFormats();
    startThread();
}

StrightAudioProcessor::~StrightAudioProcessor(){
    stopThread(4000);
}

//==============================================================================
const juce::String StrightAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool StrightAudioProcessor::acceptsMidi() const
{
    return true;
}

bool StrightAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool StrightAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double StrightAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int StrightAudioProcessor::getNumPrograms()
{
    return 1;
}

int StrightAudioProcessor::getCurrentProgram()
{
    return 0;
}

void StrightAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String StrightAudioProcessor::getProgramName (int index)
{
    return {};
}

void StrightAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void StrightAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool StrightAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void StrightAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    mSamplerate = sampleRate;
    float isr = 1./sampleRate;
    
    for(int i=0;i<2;i++){
        curvesPhasor[i].setInvSamplerate(isr);
    }
    
    if(hasStream){
        for(auto &arg : stream){
            arg.setSamplerate(sampleRate);
        }
    }
    
    scanCurve.setSamplerate(sampleRate);
    scList = scDefault;
    grainCurve.setSamplerate(sampleRate);
    gcList = gcDefault;
    pitchCurve.setSamplerate(sampleRate);
    pcList = pcDefault;
    volumeCurve.setSamplerate(sampleRate);
    vcList = vcDefault;
}

void StrightAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    ReferenceCountedBuffer::Ptr rcb(currentBuffer);

    for(const MidiMessageMetadata metadata: midiMessages){
        auto msg = metadata.getMessage();
        if(msg.isNoteOn()){
            trigger = true;
            scanCurve.list_block(scList);
            pitchCurve.list_block(pcList);
            grainCurve.list_block(gcList);
            volumeCurve.list_block(vcList);
            for(int i=0;i<2;i++){
                curvesPhasor[i].reset();
                change[i].reset();
            }
        }else if(msg.isNoteOff()){
        }
    }
    
    if(hasStream && !trigger){
        stream.clear();
        hasStream = false;
    }

    if(rcb==nullptr || !trigger){
        buffer.clear();
        return;
    }

    auto chans = buffer.getNumChannels();
    auto samps = buffer.getNumSamples();
    auto* sample = rcb->getAudioSampleBuffer();
    
    if(!hasStream){
        for(int i=0;i<chans;i++){
            stream.emplace_back(mSamplerate, sample, i, (float)i/2.);
        }
        hasStream = true;
    }

    for(int i=0;i<chans;i++){
        auto *output = buffer.getWritePointer(i);
        for(int j=0;j<samps;j++){
            
            float phSamp = curvesPhasor[i](1.f/duration);
            float cc = change[i](phSamp);
            //if(change[i](phSamp) == -1){
            if(cc == -1){
                output[j] = 0;
                trigger = false;
            }
            
            float gcSamp = util::looky(phSamp, grainCurve.arr, 1024);
            float maGrainsize = util::modamt(grainsize/100.f, gcSamp, mGrainsize/100.f);
            maGrainsize*= 100.f;
            float maPeak = util::modamt(peak, gcSamp, mPeak/100.f);
            float maPlayback = util::scale(playback, -4.f, 4.f, 0.f, 1.f);
            maPlayback = util::modamt(maPlayback, gcSamp, mPlayback/100.f);
            maPlayback = util::scale(maPlayback, 0.f, 1.f, -4.f, 4.f);
            
            float vcSamp = util::looky(phSamp, volumeCurve.arr, 1024);
            float maVolume = util::modamt(volume, vcSamp, mVolume/100.f);

            float pos = util::looky(phSamp, scanCurve.arr, 1024);
            
            float c = stream[i](maGrainsize, maPeak, pos, maPlayback);
            float mV = juce::Decibels::decibelsToGain(masterVolume);
            output[j] = c * mV * maVolume;
        }
    }
}

juce::AudioBuffer<float>& StrightAudioProcessor::getWaveForm(){
    //currently only works the second time dropping in a file
    if(currentBuffer != nullptr){
        auto sound = currentBuffer->getAudioSampleBuffer();
        if(sound){
            return *sound;
        }
    }
    static AudioBuffer<float> dummybuffer;
    return dummybuffer;
}

//==============================================================================
bool StrightAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* StrightAudioProcessor::createEditor()
{
    return new StrightAudioProcessorEditor (*this);
}

//==============================================================================
void StrightAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void StrightAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new StrightAudioProcessor();
}

//Thread stuff
void StrightAudioProcessor::run(){
    while(!threadShouldExit()){
        checkForBuffersToFree();
        checkForPathToOpen();
        wait(500);
    }
}

void StrightAudioProcessor::checkForBuffersToFree(){
    for(auto i=buffers.size();--i>=0;){
        ReferenceCountedBuffer::Ptr buffer (buffers.getUnchecked(i));
        
        if(buffer->getReferenceCount() == 2){
            buffers.remove(i);
        }
    }
}

void StrightAudioProcessor::checkForPathToOpen(){
    juce::String pathToOpen;
    pathToOpen.swapWith(chosenPath);
    
    if(pathToOpen.isNotEmpty()){
        juce::File file (pathToOpen);
        std::unique_ptr<juce::AudioFormatReader> reader(mFormatManager.createReaderFor(file));
        
        if(reader.get() != nullptr){
            if(reader->numChannels>2){
                DBG("oops");
            }else{
                ReferenceCountedBuffer::Ptr newBuffer = new ReferenceCountedBuffer(file.getFileName(),
                                                                                                 (int) reader->numChannels,
                                                                                                 (int) reader->lengthInSamples);
                              
                reader->read(newBuffer->getAudioSampleBuffer(), 0, (int)reader->lengthInSamples, 0, true, true);
                currentBuffer = newBuffer;
                buffers.add(newBuffer);
            }
        }
    }
}
