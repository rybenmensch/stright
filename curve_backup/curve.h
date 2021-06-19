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

typedef struct _curveseg
{
    float   s_target;
    float   s_delta;
    int     s_nhops;
    float   s_ccinput;
    double  s_bb;
    double  s_mm;
    /*
    _curveseg(float target, float delta, float nhops)
    {
        
    }
     */
}t_curveseg;



class curve //needs samplerate
{
public:
    curve();
    curve(float samplerate);
    ~curve();
    void curve_coefs(int nhops, double crv, double *bbp, double *mmp);
    void curve_cc(t_curveseg *segp, float f);
    int operator()(float *f, int nsamps);
    float operator()();
    void curve_float(float f);
    void curve_ft1(float f);
    void curve_list(std::vector<std::vector<float>>& args);
    void curve_list(std::vector<float>& args);
    void curve_factor(float f);
    void curve_stop();
    void curve_pause();
    void curve_resume();
        
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
    t_curveseg  *x_curseg;
    t_curveseg  *x_segs;
    t_curveseg  x_segini[42];
    std::vector<t_curveseg> segs;
    
    double CURVE_C1 {1e-20};
    double CURVE_C2 {1.2};
    double CURVE_C3 {0.41};
    double CURVE_C4 {0.91};
};
