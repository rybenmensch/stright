//
//  ReferenceCountedBuffer.hpp
//  stright
//
//  Created by Manolo Müller on 07.06.21.
//

#ifndef ReferenceCountedBuffer_hpp
#define ReferenceCountedBuffer_hpp
#include <JuceHeader.h>

class ReferenceCountedBuffer  : public juce::ReferenceCountedObject
{
public:
    typedef juce::ReferenceCountedObjectPtr<ReferenceCountedBuffer> Ptr;
    
    ReferenceCountedBuffer (const juce::String& nameToUse,
                            int numChannels,
                            int numSamples)
    : name (nameToUse),
    buffer (numChannels, numSamples)
    {
        DBG (juce::String ("Buffer named '") + name + "' constructed. numChannels = " + juce::String (numChannels) + ", numSamples = " + juce::String (numSamples));
    }
    
    ~ReferenceCountedBuffer()
    {
        DBG (juce::String ("Buffer named '") + name + "' destroyed");
    }

    juce::AudioBuffer<float>* getAudioSampleBuffer()
    {
        return &buffer;
    }
private:
    juce::String name;
    juce::AudioBuffer<float> buffer;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ReferenceCountedBuffer)
};

#endif /* ReferenceCountedBuffer_hpp */
