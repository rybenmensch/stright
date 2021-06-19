//
//  granulator.hpp
//  stright
//
//  Created by Manolo Müller on 09.06.21.
//

#pragma once
#include <JuceHeader.h>
#include "ReferenceCountedBuffer.h"
#include "util.h"
#include "curve.h"
#include "fastonebigheader.h"

class source
{
public:
    source(float sr, juce::AudioBuffer<float> *a, int channel) :
    sampleRate(sr),
    buffer(a->getReadPointer(channel)), nSamps(a->getNumSamples()) {}
    ~source() {};
    void setPosition(float pos)
    {
        phasor.reset(pos);
    }
    
    float operator()(float rate)
    {
        float freq = sampleRate/(float)nSamps * rate;
        float phase = phasor(freq, 1./sampleRate);
        return util::looky(phase, buffer, nSamps);
    }
    void reset()
    {
        phasor.reset();
    }
private:
    util::genlibphasor<float> phasor;
    float sampleRate;
    const float *buffer;
    int nSamps;
};


//freq, invSr
class window
{
public:
    window(float sr, float pOff) :
    slide(50, 50, 1.),
    invSr(1./sr)
    {
        phasor.reset(pOff);
    }
    ~window() {}
    float operator()(float freq, float peak, float isr)
    {
        float phase = phasor(freq, isr);
        reset = (change(phase) == -1)?true:false;
        peak = peak*100 + 1;
        peak = slide(peak, 50, 50);
        float angle = phase * juce::MathConstants<float>::pi;
        float lut = fast::fastersin(angle);
        lut = util::scale(lut, 0.f, 1.f, 0.f, 1.f, peak);
        return lut;
    }
    bool reset {true};
private:
    util::slide<float> slide;
    util::genlibphasor<float> phasor;
    util::change<float> change;
    float invSr;
};

class grainStream
{
public:
    grainStream
    (
        float sr,
        juce::AudioBuffer<float> *a, int channel,
        float pOff
    ) :
        sampleRate(sr), invSampleRate(1./sr),
        buffer(a->getReadPointer(channel)), bSize(a->getNumSamples()),
        phaseOffset(pOff),
        w(sr, pOff),
        s(sr, a, channel)
    {
    }
    void setSamplerate(float sr)
    {
        sampleRate      = sr;
        invSampleRate   = 1./sr;
    }
    float operator()(float wFreq, float wPeak,
                     float pPos, float pRate)
    {
        float window = w(wFreq, wPeak, invSampleRate);
        if(w.reset){
            s.setPosition(pPos);
            w.reset = false;
        }
        float sample = s(pRate);
        return window * sample;
    }
    /*
    void reset()
    {
        s.reset();
    }
     */
private:
    float sampleRate;
    float invSampleRate;
    const float *buffer;
    int bSize;
    float phaseOffset;
    util::change<float> change;
    window w;
    source s;
};
