//
//  curve.cpp
//  stright
//
//  Created by Manolo Müller on 09.06.21.
//

#include "curve.h"
#include <iostream>

curve::curve() : x_value(0.), x_ccinput(0), x_target(0.), x_deltaset(false), x_nleft(0), x_retarget(false), x_size(42), x_nsegs(0), x_pause(false)
{
    setSamplerate(44100);
}

curve::curve(float sr)
{
    setSamplerate(sr);
}

curve::~curve() { }

void curve::setSamplerate(float sr)
{
    x_ksr = sr*0.001;
}

//true means keep going, false means has stopped
//möglich hier eine laufende zeitliche anpassung der geschwindigkeit einzubauen?
//oder geht das nur in curve-coef?
//geht im moment eh nicht, da ein ganzer block angepasst wird
//möglichkeiten: anpassen, dass es sample für sample ist, oder gesamte curve in memory schreiben (wahrscheinlich cringe und langsam)
bool curve::operator()(float *f, int nsamps)
{
    float *outp = f;
    int nblock = nsamps;
    int nxfer = x_nleft;
    float curval = x_value;
    double vv = x_vv;
    double bb = x_bb;
    double mm = x_mm;
    float dy = x_dy;
    float y0 = x_y0;
    
    if (x_pause){
        while (nblock--)
            *outp++ = curval;
        return 1;
    }
    
    if(isnan(curval)){
        curval = x_value = 0;
    }
retarget:
    if (x_retarget){
        float target = segs_it->s_target;
        float delta = segs_it->s_target;
        int nhops = segs_it->s_nhops;
        bb = segs_it->s_bb;
        mm = segs_it->s_mm;
        if (segs_it->s_ccinput < 0)
            dy = x_value - target;
        else
            dy = target - x_value;
        x_nsegs--;
        advance(segs_it, 1);

        while(nhops <= 0){
            curval = x_value = target;
            if (x_nsegs){
                target = segs_it->s_target;
                delta = segs_it->s_delta;
                nhops = segs_it->s_nhops;
                bb = segs_it->s_bb;
                mm = segs_it->s_mm;
                if (segs_it->s_ccinput < 0)
                    dy = x_value - target;
                else
                    dy = target - x_value;
                x_nsegs--;
                advance(segs_it, 1);
            }else{
                while (nblock--)
                    *outp++ = curval;
                x_nleft = 0;
                x_retarget = 0;
                return 0;
            }
        }
        nxfer = x_nleft = nhops;
        x_vv = vv = bb;
        x_bb = bb;
        x_mm = mm;
        x_dy = dy;
        x_y0 = y0 = x_value;
        x_target = target;
        x_retarget = false;
    }
    
    int retval = 1;
    if(nxfer >= nblock){
        bool silly = ((x_nleft -= nblock) == 0);
        while (nblock--){
            *outp++ = curval = (vv - bb) * dy + y0;
            vv *= mm;
        }
        
        if(silly){
            if(x_nsegs)
                x_retarget = true;
            else
                retval = 0;
            x_value = x_target;
        }else{
            x_value = curval;
            x_vv = vv;
        }
    }else if (nxfer > 0){
        nblock -= nxfer;
        do{
            *outp++ = (vv - bb) * dy + y0;
            vv *= mm;

        }while(--nxfer);
        curval = x_value = x_target;
        if (x_nsegs){
            x_retarget = true;
            goto retarget;
        }else{
            while (nblock--)
                *outp++ = curval;
            x_nleft = 0;
            retval = 0;
        }
    }else{
        while(nblock--)
            *outp++ = curval;
    }
    return retval;
}

void curve::list(std::vector<std::vector<float>>& args)
{
    segs.clear();
    int nsegs = static_cast<int>(args.size());
    nsegs = util::min(nsegs, 42);
    x_nsegs = nsegs;
    for(auto &arg : args){
        segs.push_back(t_curveseg(arg, x_ksr));
    }
    
    x_deltaset = 0;
    x_target = segs[0].s_target;
    segs_it = segs.begin();
    x_retarget = true;
    x_pause = false;
}

void curve::list_block(std::vector<std::vector<float>>& args)
{
    float length = 0;
    for(auto &arg : args){
        length += arg[1];
    }
    length *= x_ksr;
    float lF = 1024./length;
    for(auto &arg : args){
        arg[1] *= lF;
    }
    list(args);
    operator()(arr, 1024);
}

void curve::stop()
{
    x_nleft = 0;
    x_nsegs = 0;
}

void curve::pause()
{
    x_pause = true;
}

void curve::resume()
{
    x_pause = false;
}
