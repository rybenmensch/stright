//
//  WaveThumbnail.h
//  stright
//
//  Created by Manolo Müller on 07.06.21.
//

#pragma once

#include <JuceHeader.h>
#include "../PluginProcessor.h"
#include "../look_and_feel/RSLColours.h"

//buggy piece of shit
//fix sometime in se future
class WaveThumbnail : public juce::Component, public juce::FileDragAndDropTarget
{
public:
    WaveThumbnail (StrightAudioProcessor& p);
    ~WaveThumbnail();

    void paint (juce::Graphics&) override;
    void resized() override;
    
    bool isInterestedInFileDrag (const juce::StringArray& files) override;
    void fileDragEnter (const juce::StringArray& files, int x, int y) override;
    void fileDragExit (const juce::StringArray& files) override;
    void filesDropped (const juce::StringArray& files, int x, int y) override;
    
private:
    std::vector<float> mAudioPoints;
    bool mShouldBePainting { false };
    bool isInterested { false };
    
    juce::String mFileName { "" };
    
    StrightAudioProcessor& processor;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WaveThumbnail)
};

