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
    AudioProcessor(BusesProperties().withOutput("Output", juce::AudioChannelSet::stereo(), true))
{
    mFormatManager.registerBasicFormats();
    for(int i=0;i<2;i++){
        volSmooth[i].setRamp(200, 200);
    }
}

StrightAudioProcessor::~StrightAudioProcessor(){
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
    fmCurve.setSamplerate(sampleRate);
    fmcList = fmcDefault;
    filterCurve.setSamplerate(sampleRate);
    fcList = fcDefault;

}

void StrightAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    ReferenceCountedBuffer::Ptr rcb(currentBuffer);

    for(const MidiMessageMetadata metadata: midiMessages){
        auto msg = metadata.getMessage();
        if(msg.isNoteOn()){
            //retrigger?
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
        copy.clear();
        hasCopy = false;
    }

    if(rcb==nullptr || !trigger){
        buffer.clear();
        return;
    }

    auto chans = buffer.getNumChannels();
    auto bchans = rcb->getAudioSampleBuffer()->getNumChannels();
    auto samps = buffer.getNumSamples();
    //auto* sample = rcb->getAudioSampleBuffer();
    
    if(!hasCopy){
        //kopie des samples machen damit man während playback den
        //buffer austauschen kann
        //noch besser wäre es, hotzuswappen
        //dann muss das playback einfach immer mit ptrs gemacht werden
        //oder so
        copy.makeCopyOf(*rcb->getAudioSampleBuffer());
        hasCopy = true;
    }
    
    int numStreams = 4;
    if(!hasStream){
        std::vector<float> startPhases {0., 0.75, 0.5, 0.25};
        int mod = bchans;
        //fake stereo
        if(bchans == 1)  bchans = 2;
        for(int c=0;c<bchans;c++){
            for(int i=0;i<numStreams;i++){
                stream.emplace_back(mSamplerate, &copy, c%mod
                                    , startPhases[i]);
            }
        }
        hasStream = true;
    }

    for(int i=0;i<chans;i++){
        auto *output = buffer.getWritePointer(i);
        
        for(int j=0;j<samps;j++){
            float phSamp = curvesPhasor[i](1.f/duration);
            if(change[i](phSamp) == -1){
                output[j] = 0;
                trigger = false;
            }
            
            float gcSamp = util::looky(phSamp, grainCurve.arr, 1024);
            float maGrainsize = util::modamt(grainsize/100.f, gcSamp, mGrainsize);
            maGrainsize = util::scale(maGrainsize, 0.f, 1.f, 1.f, 100.f, 4.f);
            float maPeak = util::modamt(peak, gcSamp, mPeak);
            float maPlayback = util::scale(playback, -4.f, 4.f, 0.f, 1.f);
            maPlayback = util::modamt(maPlayback, gcSamp, mPlayback);
            maPlayback = util::scale(maPlayback, 0.f, 1.f, -4.f, 4.f);
            float pos = util::looky(phSamp, scanCurve.arr, 1024);
            
            float c = 0;
            for(int k=0;k<numStreams;k++){
                int index = i*numStreams+k;
                c += stream[index](maGrainsize, maPeak, pos, maPlayback);
            }
            
            float vcSamp = util::looky(phSamp, volumeCurve.arr, 1024);
            vcSamp = volSlide[i](vcSamp, 0, 20);
            float maVolume = util::scale(volume, -70.f, 0.f, 0.f, 1.f);
            maVolume = util::modamt(maVolume, vcSamp, mVolume);
            maVolume = util::scale(maVolume, 0.f, 1.f, -70.f, 0.f, 0.5f);
            maVolume = juce::Decibels::decibelsToGain(maVolume);
            maVolume = volSmooth[i](maVolume);

            float mV = juce::Decibels::decibelsToGain(masterVolume);
            output[j] = c * mV * maVolume;
        }
    }
}

juce::AudioBuffer<float>& StrightAudioProcessor::getWaveForm(){
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
