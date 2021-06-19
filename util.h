//
//  util.h
//  stright
//
//  Created by Manolo Müller on 07.06.21.
//

#pragma once
#ifndef UTIL_H
#define UTIL_H
#include "JuceHeader.h"
#include <vector>
#include <iostream>

namespace util{
template <typename T>
T clamp(T a, T lo, T hi)
{
    return (a > lo) ? ((a < hi) ? a : hi) : lo;
}

template <typename T>
T max(T a, T lo)
{
    return (a<lo)?lo:a;
}

template <typename T>
T min(T a, T hi)
{
    return (a<hi)?a:hi;
}

template <typename T>
T scale(T in, T il, T ih, T ol, T oh, T p)
{
    T output = 0;
    p = util::max(p, T{0});
    T check = (in - il) / (ih - il);
    
    if(check == 0){
        output = ol;
    }else if(check > 0){
        output = (ol + (oh - ol) * pow((in - il) / (ih - il), p));
    }else{
        output = (ol + (oh - ol) * - (pow(((-in + il) / (ih - il)), p)));
    }
    return output;
}

template <typename T>
T scale(T in, T il, T ih, T ol, T oh) { return util::scale(in, il, ih, ol, oh, T{1}); }

template <typename T>
T wrap(T a, T lo, T hi)
{
    if(a >= lo && a <= hi)
        return a;
    else if(a<lo)
        return (a - lo)+hi;
    else
        return (a - hi) + lo;
}

//from genlib
template <typename T>
T wrap2(T v, T lo1, T hi1)
{
    T lo;
    T hi;
    if(lo1 == hi1)
        return lo1;
    if (lo1 > hi1) {
        hi = lo1;
        lo = hi1;
    } else {
        lo = lo1;
        hi = hi1;
    }
    const T range = hi - lo;
    if (v >= lo && v < hi)
        return v;
    if (range <= 0.000000001)
        return lo;    // no point...
    const long numWraps = long((v-lo)/range) - (v < lo);
    return v - range * double(numWraps);
}

template <typename T>
T kink(T in, T slope)
{
    slope = max(slope, 0);
    if(slope){
        T breakpoint = 0.5/slope;
        if(in <= breakpoint)
            return in * slope;
        else
            return 0.5 / (1-breakpoint) * (in-breakpoint) + 0.5;
    }else{
        return 0;
    }
}

template <typename T>
T linear_interp(T index, T samp1, T samp2)
{
    return samp1 + index * (samp2 - samp1);
}

inline float lpeek(const float *buf, long buffer_size, float index){
    long index_trunc = std::floor(index);
    float index_fract = (index - index_trunc);
    long index_trunc_2 = (index_trunc+1);
    bool index_ignore = ((index_trunc >= buffer_size) || (index_trunc<0));
    bool index_ignore_2 = ((index_trunc_2 >= buffer_size) || (index_trunc_2<0));

    float read = (index_ignore)?0:buf[index_trunc];
    float read_2 = (index_ignore_2)?0:buf[index_trunc_2];

    float readinterp = linear_interp(index_fract, read, read_2);
    return readinterp;
}

inline float peek(juce::AudioBuffer<float> *buffer, float phase, int channel)
{
    auto *a = buffer->getReadPointer(channel % buffer->getNumChannels());
    int nSamps = buffer->getNumSamples();
    
    phase *= nSamps;
    nSamps--;
    return util::lpeek(a, nSamps, phase);;
}

inline float looky(float phase, const float* buffer, int nSamps)
{
    float iPhase = (phase * (float)nSamps) - 1;
    return util::lpeek(buffer, nSamps, iPhase);
}
 
//all this T{} needed?
template <typename T>
T modamt(T value, T modulation, T modamt)
{
    value = util::clamp(value, T{0}, T{1});
    modulation = util::clamp(modulation, T{0}, T{1});
    modamt = util::clamp(modamt, T{-100}, T{100});
    modamt /= T{100};
    T modamt_pos = util::clamp(modamt, T{0}, T{1});
    T val_inv = T{1} - value;
    modamt_pos *= val_inv;
    modamt_pos *= modulation;

    T modamt_neg = util::clamp(modamt, T{-1}, T{0}) * T{-1};
    modamt_neg *= modulation;
    modamt_neg *= value;
    modamt_neg = value - modamt_neg;

    return modamt_neg + modamt_pos;
}

template <typename T>
class slide
{
public:
    slide(int slideUp, int slideDown, T val)
    {
         setSlide(slideUp, slideDown);
         reset(val);
    }
    slide(int slideUp, int slideDown) :
    slide(slideUp, slideDown, 0.) {}

    slide() : slide(1, 1, 0.) {}
    
    T operator()(T f, int slideUp, int slideDown)
    {
        float output = 0.;
        if(f >= mLast){
            if(slideUp > 1.)
                output = mLast + ((f - mLast) / slideUp);
            else
                output = mLast = f;
        }else if(f < mLast){
            if(slideDown > 1.)
                output = mLast + ((f - mLast) / slideDown);
            else
                output = mLast = f;
        }
        if(output == mLast && output != f)
            output = f;
        mLast = output;
        return output;
    }
    
    T operator()(T f)
    {
        float output = 0.;
        if(f >= mLast){
            if(mSlideUp > 1.)
                output = mLast + ((f - mLast) / (T)mSlideUp);
            else
                output = mLast = f;
        }else if(f < mLast){
            if(mSlideDown > 1.)
                output = mLast + ((f - mLast) / (T)mSlideDown);
            else
                output = mLast = f;
        }
        if(output == mLast && output != f)
            output = f;
        mLast = output;
        return output;
    }

    void reset(T v=0.) { mLast = v;}
    void slideUp(int factor) { mSlideUp = max(factor, 1); }
    void slideDown(int factor) { mSlideDown = max(factor, 1); }
    void setSlide(int sU, int sD) { slideUp(sU); slideUp(sD); }
private:
    int mSlideUp;
    int mSlideDown;
    T mLast;
};

class rampsmooth
{
public:
    rampsmooth(int rampUp, int rampDown)
    {
        setRamp(rampUp, rampDown);
        reset();
    }

    rampsmooth() : rampsmooth(1, 1) {}

    void reset()
    {
        mChange = 1;
        mLast   = 0.;
        mTarget = 0.;
        mIncr   = 0.;
        mNLeft  = 0;
    }

    float operator()(float f)
    {
        float output = 0;
        if(f != mTarget || mChange){
            mTarget = f;
            mChange = false;
            if(f > mLast){
                if(mRampUp > 1){
                    mIncr = (f - mLast) * mUpCoef;
                    mNLeft = mRampUp;
                    output =  (mLast += mIncr);
                    return output;
                }
            }else if(f<mLast){
                if(mRampDown > 1){
                    mIncr = (f - mLast) * mDownCoef;
                    mNLeft = mRampDown;
                    output =  (mLast += mIncr);
                    return output;
                }
            }
            mIncr = 0;
            mNLeft = 0;
            mLast = f;
            return mLast;
        }else if(mNLeft > 0){
            output = (mLast += mIncr);
            if(--mNLeft == 1){
                mIncr = 0.;
                mLast = mTarget;
            }
        }else{
            output = mTarget;
        }

        return output;
    }

    void rampUp(int factor)
    {
        double upCoef;
        if(factor>1){
            mRampUp = factor;
            upCoef = 1. / (float)factor;
        }else{
            mRampUp = 0;
            upCoef = 0.;
        }

        if(upCoef != mUpCoef){
            mUpCoef = upCoef;
            mChange = true;
        }
    }

    void rampDown(int factor)
    {
        double downCoef;
        if(factor>1){
            mRampDown = factor;
            downCoef = 1. / (float)factor;
        }else{
            mRampDown = 0;
            downCoef = 0.;
        }

        if(downCoef != mDownCoef){
            mDownCoef = downCoef;
            mChange = true;
        }
    }

    void setRamp(int rU, int rD) { rampUp(rU); rampDown(rD); }
private:
    int mRampUp;
    int mRampDown;
    double mUpCoef;
    double mDownCoef;
    float mLast;
    float mTarget;
    float mIncr;
    int mNLeft;
    bool mChange;
};

template <typename T>
class genlibphasor
{
public:
    genlibphasor() { reset(); }
    void reset(T v=0.) { phase = v; }
    void setSamplerate(T f) { mInvSamplerate = 1./f; }
    void setInvSamplerate(T f) { mInvSamplerate = f; }
    inline T operator()(T freq, T invsamplerate)
    {
        const T pincr = freq * invsamplerate;
        phase = util::wrap2(phase + pincr, T{0}, T{1});
        //phase = util::wrap(phase + pincr, 0., 1.);
        return phase;
    }
    
    inline T operator()(T freq) { return operator()(freq, mInvSamplerate); }
    
private:
    T phase;
    T mInvSamplerate;
};

template <typename T>
class delta
{
public:
    delta() { reset(); }
    void reset(T v=0.) { mLast = v; }
    T operator()(T f)
    {
        T outval = f - mLast;
        mLast = f;
        return outval;
    }
private:
    T mLast;
};

template <typename T>
class change
{
public:
    change() { reset(); }
    void reset(T v=0.) { mDelta.reset(v); }
    T operator()(T f)
    {
        T d = mDelta(f);
        if(d > 0.)
            return 1;
        else if(d < 0.)
            return -1;
        else
            return 1;
    }
private:
    delta<T> mDelta;
};

/*
class phasor
{
public:
    phasor(float f, float p)
    {
        setSamplerate(44100);
        setFrequency(f);
        setIncr();
        p = util::clamp(p, 0.f, 1.f);
        setPhase(p);
        mRunningPhase = 0;
    }
    
    phasor(float f) : phasor(f, 0) {}
    phasor() : phasor(1., 0.) {}
    void setFrequency(float f) { mFrequency = f; setIncr(); }
    void setPhase(float p) { mPhase = p; }
    void reset() { mRunningPhase = mPhase; }
    void set(float f) { mRunningPhase = f; }
    void setSamplerate(float sr) { mSamplerate = sr; setIncr(); }
    
    float operator()(float freq, float invsr)
    {
        float outval = mRunningPhase = util::wrap(mRunningPhase, 0.f, 1.f);
        mRunningPhase += freq * invsr;
        return util::wrap(outval + mPhase, 0.f, 1.f);
    }
    
    float operator()()
    {
        float outval;
        outval = mRunningPhase = util::wrap(mRunningPhase, 0.f, 1.f);
        mRunningPhase += mIncr;
        return util::wrap(outval + mPhase, 0.f, 1.f);
    }

    void operator()(float* f, int samps)
    {
        for(int i = 0;i<samps;i++){
            f[i] = operator()();
        }
    }
private:
    void setIncr() { mInvSr = 1/mSamplerate; mIncr = mInvSr * mFrequency; }
    float mFrequency;
    float mPhase;
    float mRunningPhase;
    float mSamplerate;
    float mInvSr;
    float mIncr;
};

//invsr sollte gegeben werden können
class phasor2
{
public:
    phasor2(float phase) : mPhase(phase) {}
    phasor2() : phasor2(0) {}
    float operator()(float freq, float invsr)
    {
        float outval = mRunningPhase = util::wrap(mRunningPhase, 0.f, 1.f);
        mRunningPhase += freq * invsr;
        return util::wrap(outval + mPhase, 0.f, 1.f);
        return 0;
    }
    void operator()(float freq, float invsr, float *f, int samps)
    {
        while(samps--){
            *f++ = operator()(freq, invsr);
        }
    }
    void set(float p) { mRunningPhase = p; }
    void reset() { set(0); }
    void setSamplerate(double sr) { mInvSr = 1./sr; };
private:
    float mRunningPhase;
    float mPhase;
    float mInvSr;
};

class scale
{
public:
    scale(float minIn, float maxIn, float minOut, float maxOut, float expo, bool classic) :
    mMinIn(minIn), mMaxIn(maxIn), mMinOut(minOut), mMaxOut(maxOut), mExpo(expo), mClassic(classic) {}

    scale(float minIn, float maxIn, float minOut, float maxOut) :
    scale(minIn, maxIn, minOut, maxOut, 1, false) {}

    scale() : scale(0, 127, 0, 1, 1, false) {}

    float operator()(float in, float il, float ih, float ol, float oh, float p)
    {
        float output = 0;
        if (mClassic != 0){
            p = p <= 1 ? 1 : p;
            if (p == 1) output = ((in - il) / (ih - il) == 0) ? ol
                : (((in - il) / (ih - il)) > 0) ? (ol + (oh - ol) * pow((in - il) / (ih - il), p))
                    : (ol + (oh - ol) * -(pow(((-in + il) / (ih - il)), p)));
            else{
                output = ol + (oh - ol) * ((oh - ol) * exp((il - ih) * log(p)) * exp(in * log(p)));
                if (oh - ol <= 0)
                    output = output * -1;
            }
        }
        else{
            p = p <= 0 ? 0 : p;
            output = ((in - il) / (ih - il) == 0) ? ol :
                (((in - il) / (ih - il)) > 0) ?
                (ol + (oh - ol) * pow((in - il) / (ih - il), p)) :
                (ol + (oh - ol) * -(pow(((-in + il) / (ih - il)), p)));
        }
        return output;
    }
    
    float operator()(float in) { return operator()(in, mMinIn, mMaxIn, mMinOut, mMaxOut, mExpo); }

    void setClassic(bool c) { mClassic = c; }
private:
    float mMinIn;
    float mMaxIn;
    float mMinOut;
    float mMaxOut;
    float mExpo;
    bool mClassic;
};
*/
}
//namespace util
#endif //UTIL_H
