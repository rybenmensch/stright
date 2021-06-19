//
//  curve.cpp
//  stright
//
//  Created by Manolo Müller on 09.06.21.
//

#include "curve.h"

curve::curve() : x_value(0.), x_target(0.), x_deltaset(false), x_ksr(44100*0.001), x_nleft(0), x_retarget(false), x_size(42), x_nsegs(0), x_pause(false), x_curseg(nullptr), x_segs(x_segini), CURVE_C1 (1e-20), CURVE_C2 (1.2), CURVE_C3 (0.41), CURVE_C4 (0.91)
{
    curve_factor(0.);
    //TODO put in what is done in curve_dsp
    //"make it more c++"
    //cut out unnecessary methods
}

curve::curve(float samplerate) : x_ksr(samplerate*0.001)
{
    
}

curve::~curve()
{
    /*
    if(x_segs != nullptr){
        delete x_segs;
    }
     */
}

void curve::curve_coefs(int nhops, double crv, double *bbp, double *mmp)
{
    if (nhops > 0){
        double hh, ff, eff, gh;
        if (crv < 0){
            if (crv < -1.)
                crv = -1.;
            hh = pow(((CURVE_C1 - crv) * CURVE_C2), CURVE_C3)
                * CURVE_C4;
            ff = hh / (1. - hh);
            eff = exp(ff) - 1.;
            gh = (exp(ff * .5) - 1.) / eff;
            *bbp = gh * (gh / (1. - (gh + gh)));
            *mmp = 1. / (((exp(ff * (1. / (double)nhops)) - 1.) /
                        (eff * *bbp)) + 1.);
            *bbp += 1.;
        }else{
            if (crv > 1.)
                crv = 1.;
            hh = pow(((crv + CURVE_C1) * CURVE_C2), CURVE_C3)
                * CURVE_C4;
            ff = hh / (1. - hh);
            eff = exp(ff) - 1.;
            gh = (exp(ff * .5) - 1.) / eff;
            *bbp = gh * (gh / (1. - (gh + gh)));
            *mmp = ((exp(ff * (1. / (double)nhops)) - 1.) /
                    (eff * *bbp)) + 1.;
        }
    }else if (crv < 0){
        *bbp = 2.;
        *mmp = 1.;
    }
    else{
        *bbp = *mmp = 1.;
    }
}

void curve::curve_cc(t_curveseg *segp, float f)
{
    int nhops = segp->s_delta * x_ksr + 0.5;
    segp->s_ccinput = f;
    segp->s_nhops = (nhops > 0 ? nhops : 0);
    curve_coefs(segp->s_nhops, (double)f, &segp->s_bb, &segp->s_mm);
}

//1 means keep going, 0 means has stopped
int curve::operator()(float *f, int nsamps)
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
    
    /*
    if (PD_BIGORSMALL(curval))  //LATER rethink
        curval = x_value = 0;
    */
retarget:
    if (x_retarget){
        float target = x_curseg->s_target;
        //float delta = x_curseg->s_delta;
        int nhops = x_curseg->s_nhops;
        bb = x_curseg->s_bb;
        mm = x_curseg->s_mm;
        if (x_curseg->s_ccinput < 0)
            dy = x_value - target;
        else
            dy = target - x_value;
        x_nsegs--;
        x_curseg++;
        while (nhops <= 0){
            curval = x_value = target;
            if (x_nsegs){
                target = x_curseg->s_target;
                //delta = x_curseg->s_delta;
                nhops = x_curseg->s_nhops;
                bb = x_curseg->s_bb;
                mm = x_curseg->s_mm;
                if (x_curseg->s_ccinput < 0)
                    dy = x_value - target;
                else
                    dy = target - x_value;
                x_nsegs--;
                x_curseg++;
            }else{
                while (nblock--)
                    *outp++ = curval;
                x_nleft = 0;
                //clock_delay(x_clock, 0);
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
        x_retarget = 0;
    }
    
    int retval = 1;
    if (nxfer >= nblock){
        int silly = ((x_nleft -= nblock) == 0);  /* LATER rethink */
        while (nblock--){
            *outp++ = curval = (vv - bb) * dy + y0;
            vv *= mm;
        }
        
        if (silly){
            if (x_nsegs){
                x_retarget = 1;
            }else{
                retval = 0;
                //clock_delay(x_clock, 0);
            }
            x_value = x_target;
        }else{
            x_value = curval;
            x_vv = vv;
        }
    }else if (nxfer > 0){
        //retval = 1;
        nblock -= nxfer;
        do{
            *outp++ = (vv - bb) * dy + y0;
            vv *= mm;

        }while (--nxfer);
        curval = x_value = x_target;
        if (x_nsegs){
            x_retarget = 1;
            goto retarget;
        }else{
            while (nblock--)
                *outp++ = curval;
            x_nleft = 0;
            retval = 0;
            //clock_delay(x_clock, 0);
        }
    }else{
        while (nblock--)
            *outp++ = curval;
    }
    
    return retval;
}

float curve::operator()()
{
    return 0;
}

void curve::curve_float(float f)
{
    if(x_deltaset){
        x_deltaset = 0;
        x_target = f;
        x_nsegs = 1;
        x_curseg = x_segs;
        x_curseg->s_target = f;
        x_curseg->s_delta = x_delta;
        curve_cc(x_curseg, x_ccinput);
        x_retarget = true;
    }else{
        x_value = x_target = f;
        x_nsegs = 0;
        x_curseg = 0;
        x_nleft = 0;
        x_retarget = false;
    }
    x_pause = 0;
}

void curve::curve_ft1(float f)
{
    x_delta = f;
    x_deltaset = (f > 0);
}

void curve::curve_list(std::vector<std::vector<float>>& args)
{
    segs.clear();
    int nsegs = static_cast<int>(args.size());
    nsegs = util::max(nsegs, 42);
    
    x_nsegs = nsegs;
    t_curveseg *segp = x_segs;
    
    for(auto arg : args){
        //segp->s_target = ar
    }
}

void curve::curve_list(std::vector<float>& args)
{
    //rewrite as c++
    int natoms = static_cast<int>(args.size());
    int odd = natoms % 3;
    int nsegs = natoms / 3;
    
    if(odd)
        nsegs++;
    if(nsegs > 42){
        nsegs = 42;
        odd = 0;
    }
    x_nsegs = nsegs;
    t_curveseg *segp = x_segs;
    if(odd)
        nsegs--;
    
    int a_c = 0;
    while(nsegs--){
        segp->s_target = args[a_c++];
        segp->s_delta = args[a_c++];
        curve_cc(segp, args[a_c++]);
        segp++;
    }
    //sketchy
    if(odd){
        segp->s_target = args[a_c++];
        if(odd > 1)
            segp->s_delta = args[1];
        else
            segp->s_delta = 0;
        curve_cc(segp, x_ccinput);
    }
    x_deltaset = 0;
    x_target = x_segs->s_target;
    x_curseg = x_segs;
    x_retarget = true;
    x_pause = false;
}

void curve::curve_factor(float f)
{
    x_ccinput = util::clamp(f, -1.f, 1.f);
}

void curve::curve_stop()
{
    x_nsegs = x_nleft = 0;
}

void curve::curve_pause()
{
    x_pause = true;
}

void curve::curve_resume()
{
    x_pause = false;
}
