//
//  curve.hpp
//  stright
//
//  Created by Manolo Müller on 09.06.21.
//

#pragma once
#include <cmath>
#include <vector>
#include "util.h"

#define CURVE_C1 1e-20
#define CURVE_C2 1.2
#define CURVE_C3 0.41
#define CURVE_C4 0.91

typedef struct _curveseg
{
    float   s_target;
    float   s_delta;
    int     s_nhops;
    float   s_ccinput;
    double  s_bb;
    double  s_mm;
    float   s_sr;
    
    _curveseg(std::vector<float> arg, float sr) :
    s_target(arg[0]), s_delta(arg[1]), s_ccinput(arg[2]), s_sr(sr)
    {
        s_ccinput   = util::clamp(s_ccinput, -1.f, 1.f);
        s_nhops     = s_delta * s_sr + 0.5;
        s_nhops     = util::max(s_nhops, 0);
        curve_coefs();
    }
    
    void curve_coefs()
    {
        if (s_nhops > 0){
            double hh, ff, eff, gh;
            if (s_ccinput < 0){
                hh = pow(((CURVE_C1 - s_ccinput) * CURVE_C2), CURVE_C3) * CURVE_C4;
                ff = hh / (1. - hh);
                eff = exp(ff) - 1.;
                gh = (exp(ff * .5) - 1.) / eff;
                s_bb = gh * (gh / (1. - (gh + gh)));
                s_mm = 1. / (((exp(ff * (1. / (double)s_nhops)) - 1.) / (eff * s_bb)) + 1.);
                s_bb += 1.;
            }else{
                hh = pow(((s_ccinput + CURVE_C1) * CURVE_C2), CURVE_C3) * CURVE_C4;
                ff = hh / (1. - hh);
                eff = exp(ff) - 1.;
                gh = (exp(ff * .5) - 1.) / eff;
                s_bb = gh * (gh / (1. - (gh + gh)));
                s_mm = ((exp(ff * (1. / (double)s_nhops)) - 1.) / (eff * s_bb)) + 1.;
            }
        }else if (s_ccinput < 0){
            s_bb = 2.;
            s_mm = 1.;
        }else{
            s_bb = s_mm = 1.;
        }
    }
}t_curveseg;

class curve
{
public:
    curve();
    curve(float samplerate);
    ~curve();
    void setSamplerate(float sr);
    bool operator()(float *f, int nsamps);
    void list(std::vector<std::vector<float>>& args);
    void list_block(std::vector<std::vector<float>>& args);
    void stop();
    void pause();
    void resume();
    float arr[1024];
private:
    float       x_value;
    float       x_ccinput;
    float       x_target;
    float       x_delta;
    bool        x_deltaset;
    double      x_vv;
    double      x_bb;
    double      x_mm;
    float       x_y0;
    float       x_dy;
    float       x_ksr;
    int         x_nleft;
    bool        x_retarget;
    int         x_size;
    int         x_nsegs;
    bool        x_pause;
    std::vector<t_curveseg> segs;
    std::vector<t_curveseg>::iterator segs_it;
};
