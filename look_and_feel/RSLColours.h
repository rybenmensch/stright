//
//  RSLColours.h
//  stright
//
//  Created by Manolo Müller on 17.06.21.
//

#ifndef RSLColours_h
#define RSLColours_h
#include "JuceHeader.h"
//alles inline wegen duplicate symbol error (trotz header guard????)
namespace RSL{
inline juce::Colour black{0xFF232323};
inline juce::Colour white{0xFFFEFEFE};

inline std::vector<juce::Colour> curveCandycane{
    juce::Colour{0xFFCEE5E8},
    juce::Colour{0xFF8EBCFE},
    juce::Colour{0xFFB0E54E},
    juce::Colour{0xFFE0A7E5},
    juce::Colour{0xFFF9D975},
    juce::Colour{0xFFBB09C9},
    juce::Colour{0xFFE13E26},
    juce::Colour{0xFF0BBDD5}
};
inline juce::Colour curveGrid{0xFF595959};
inline juce::Colour numBoxSlider{0xFF8092C5};
//inline juce::Colour selectionBlue = juce::Colour::fromRGBA(0.506, 0.576, 0.776, 0.510);
//inline juce::Colour selectionBlue = juce::Colour::fromRGBA(0.506, 0.576, 0.776, 1);
inline juce::Colour selectionBlue{0x828193c6};

//auto extracted colours
inline juce::Colour c0 = juce::Colour::fromRGBA(0, 0, 0, 1.);
inline juce::Colour c1 = juce::Colour::fromRGBA(0.0, 0.0, 0.0, 0.99);
inline juce::Colour c2 = juce::Colour::fromRGBA(0.0, 0.0, 0.0, 1.0);
inline juce::Colour c3 = juce::Colour::fromRGBA(0.0, 0.533333, 0.168627, 1.0);
inline juce::Colour c4 = juce::Colour::fromRGBA(0.011765, 0.396078, 0.752941, 1.0);
inline juce::Colour c5 = juce::Colour::fromRGBA(0.029546, 0.773327, 0.821113, 1.0);
inline juce::Colour c6 = juce::Colour::fromRGBA(0.095481, 0.100396, 0.100293, 1.0);
inline juce::Colour c7 = juce::Colour::fromRGBA(0.113725, 0.580392, 0.737255, 1.0);
inline juce::Colour c8 = juce::Colour::fromRGBA(0.113725, 0.607843, 0.607843, 1.0);
inline juce::Colour c9 = juce::Colour::fromRGBA(0.137254901960784, 0.137254901960784, 0.137254901960784, 0.5);
inline juce::Colour c10 = juce::Colour::fromRGBA(0.137254901960784, 0.137254901960784, 0.137254901960784, 0.6);
inline juce::Colour c11 = juce::Colour::fromRGBA(0.137254901960784, 0.137254901960784, 0.137254901960784, 1.0);
inline juce::Colour c12 = juce::Colour::fromRGBA(0.163647, 0.174699, 0.17409, 1.0);
inline juce::Colour c13 = juce::Colour::fromRGBA(0.185512, 0.263736, 0.260626, 1.0);
inline juce::Colour c14 = juce::Colour::fromRGBA(0.199068, 0.062496, 0.060031, 0.9);
inline juce::Colour c15 = juce::Colour::fromRGBA(0.2, 0.2, 0.2, 0.0);
inline juce::Colour c16 = juce::Colour::fromRGBA(0.231373, 0.121569, 0.305882, 0.9);
inline juce::Colour c17 = juce::Colour::fromRGBA(0.235294117647059, 0.235294117647059, 0.235294117647059, 1.0);
inline juce::Colour c18 = juce::Colour::fromRGBA(0.239216, 0.254902, 0.278431, 1.0);
inline juce::Colour c19 = juce::Colour::fromRGBA(0.287863, 0.333333, 0.329398, 1.0);
inline juce::Colour c20 = juce::Colour::fromRGBA(0.290196, 0.309804, 0.301961, 0.45);
inline juce::Colour c21 = juce::Colour::fromRGBA(0.290196, 0.309804, 0.301961, 1.0);
inline juce::Colour c22 = juce::Colour::fromRGBA(0.294118, 0.313726, 0.337255, 1);
inline juce::Colour c23 = juce::Colour::fromRGBA(0.304029, 0.250694, 0.285628, 1.0);
inline juce::Colour c24 = juce::Colour::fromRGBA(0.309495, 0.299387, 0.299789, 1.0);
inline juce::Colour c25 = juce::Colour::fromRGBA(0.313725, 0.313725, 0.313725, 0.0);
inline juce::Colour c26 = juce::Colour::fromRGBA(0.313725490196078, 0.313725490196078, 0.313725490196078, 0.0);
inline juce::Colour c27 = juce::Colour::fromRGBA(0.317647, 0.654902, 0.976471, 1.0);
inline juce::Colour c28 = juce::Colour::fromRGBA(0.32549, 0.345098, 0.372549, 0.0);
inline juce::Colour c29 = juce::Colour::fromRGBA(0.32549, 0.345098, 0.372549, 1.0);
inline juce::Colour c30 = juce::Colour::fromRGBA(0.349019607843137, 0.349019607843137, 0.349019607843137, 1.0);
inline juce::Colour c31 = juce::Colour::fromRGBA(0.376471, 0.384314, 0.4, 0.0);
inline juce::Colour c32 = juce::Colour::fromRGBA(0.376471, 0.384314, 0.4, 1.0);
inline juce::Colour c33 = juce::Colour::fromRGBA(0.380392, 0.380392, 0.380392, 1.0);
inline juce::Colour c34 = juce::Colour::fromRGBA(0.396, 0.396, 0.396, 1.0);
inline juce::Colour c35 = juce::Colour::fromRGBA(0.396078431372549, 0.396078431372549, 0.396078431372549, 1.0);
inline juce::Colour c36 = juce::Colour::fromRGBA(0.439216, 0.74902, 0.254902, 0.898039);
inline juce::Colour c37 = juce::Colour::fromRGBA(0.454902, 0.462745, 0.482353, 0.0);
inline juce::Colour c38 = juce::Colour::fromRGBA(0.461105, 0.492646, 0.591878, 0.34);
inline juce::Colour c39 = juce::Colour::fromRGBA(0.461105, 0.492646, 0.591878, 1.0);
inline juce::Colour c40 = juce::Colour::fromRGBA(0.462745, 0.933333, 0.0, 1.0);
inline juce::Colour c41 = juce::Colour::fromRGBA(0.475135, 0.293895, 0.251069, 1.0);
inline juce::Colour c42 = juce::Colour::fromRGBA(0.498039, 0.498039, 0.498039, 0.5);
inline juce::Colour c43 = juce::Colour::fromRGBA(0.498039, 0.498039, 0.498039, 1.0);
inline juce::Colour c44 = juce::Colour::fromRGBA(0.501960784313725, 0.501960784313725, 0.501960784313725, 0.0);
inline juce::Colour c45 = juce::Colour::fromRGBA(0.501960784313725, 0.572549019607843, 0.772549019607843, 1.0);
inline juce::Colour c46 = juce::Colour::fromRGBA(0.505882352941176, 0.576470588235294, 0.776470588235294, 0.0);
inline juce::Colour c47 = juce::Colour::fromRGBA(0.505882352941176, 0.576470588235294, 0.776470588235294, 0.51);
inline juce::Colour c48 = juce::Colour::fromRGBA(0.50764, 0.065317, 0.112129, 1.0);
inline juce::Colour c49 = juce::Colour::fromRGBA(0.6, 0.0, 0.0, 1.0);
inline juce::Colour c50 = juce::Colour::fromRGBA(0.619608, 0.0, 0.360784, 1.0);
inline juce::Colour c51 = juce::Colour::fromRGBA(0.646639, 0.821777, 0.854593, 1.0);
inline juce::Colour c52 = juce::Colour::fromRGBA(0.647059, 0.647059, 0.647059, 0.0);
inline juce::Colour c53 = juce::Colour::fromRGBA(0.65, 0.597804, 0.185705, 1.0);
inline juce::Colour c54 = juce::Colour::fromRGBA(0.65, 0.65, 0.65, 0.99);
inline juce::Colour c55 = juce::Colour::fromRGBA(0.65, 0.65, 0.65, 1.0);
inline juce::Colour c56 = juce::Colour::fromRGBA(0.65098, 0.65098, 0.65098, 0.0);
inline juce::Colour c57 = juce::Colour::fromRGBA(0.65098, 0.666667, 0.662745, 1.0);
inline juce::Colour c58 = juce::Colour::fromRGBA(0.654902, 0.572549, 0.376471, 1.0);
inline juce::Colour c59 = juce::Colour::fromRGBA(0.701961, 0.415686, 0.886275, 1.0);
inline juce::Colour c60 = juce::Colour::fromRGBA(0.720698, 0.16723, 0.080014, 1.0);
inline juce::Colour c61 = juce::Colour::fromRGBA(0.764705882352941, 0.764705882352941, 0.764705882352941, 0.0);
inline juce::Colour c62 = juce::Colour::fromRGBA(0.764706, 0.592157, 0.101961, 1.0);
inline juce::Colour c63 = juce::Colour::fromRGBA(0.764706, 0.764706, 0.764706, 0.0);
inline juce::Colour c64 = juce::Colour::fromRGBA(0.784314, 0.145098, 0.023529, 1.0);
inline juce::Colour c65 = juce::Colour::fromRGBA(0.786675, 0.801885, 0.845022, 1.0);
inline juce::Colour c66 = juce::Colour::fromRGBA(0.803922, 0.898039, 0.909804, 0.29);
inline juce::Colour c67 = juce::Colour::fromRGBA(0.807843137254902, 0.898039215686275, 0.909803921568627, 0.0);
inline juce::Colour c68 = juce::Colour::fromRGBA(0.82517, 0.78181, 0.059545, 1.0);
inline juce::Colour c69 = juce::Colour::fromRGBA(0.839216, 0.839216, 0.839216, 1.0);
inline juce::Colour c70 = juce::Colour::fromRGBA(0.83978, 0.839941, 0.839753, 1.0);
inline juce::Colour c71 = juce::Colour::fromRGBA(0.862745, 0.870588, 0.878431, 1.0);
inline juce::Colour c72 = juce::Colour::fromRGBA(0.870588, 0.415686, 0.062745, 1.0);
inline juce::Colour c73 = juce::Colour::fromRGBA(0.871412, 0.955014, 0.629622, 1.0);
inline juce::Colour c74 = juce::Colour::fromRGBA(0.904179, 0.895477, 0.842975, 0.56);
inline juce::Colour c75 = juce::Colour::fromRGBA(0.941176, 0.690196, 0.196078, 1.0);
inline juce::Colour c76 = juce::Colour::fromRGBA(0.952941, 0.564706, 0.098039, 1.0);
inline juce::Colour c77 = juce::Colour::fromRGBA(0.984314, 0.819608, 0.05098, 1.0);
inline juce::Colour c78 = juce::Colour::fromRGBA(0.996078, 0.615686, 0.0, 1.0);
inline juce::Colour c79 = juce::Colour::fromRGBA(0.996078431372549, 0.996078431372549, 0.996078431372549, 0.0);
inline juce::Colour c80 = juce::Colour::fromRGBA(0.996078431372549, 0.996078431372549, 0.996078431372549, 0.39);
inline juce::Colour c81 = juce::Colour::fromRGBA(0.996078431372549, 0.996078431372549, 0.996078431372549, 0.5);
inline juce::Colour c82 = juce::Colour::fromRGBA(0.996078431372549, 0.996078431372549, 0.996078431372549, 1.0);
inline juce::Colour c83 = juce::Colour::fromRGBA(1, 1, 1, 0.5);
inline juce::Colour c84 = juce::Colour::fromRGBA(1, 1, 1, 1.);
inline juce::Colour c85 = juce::Colour::fromRGBA(1.0, 0.0, 0.0, 1.0);
inline juce::Colour c86 = juce::Colour::fromRGBA(1.0, 0.709803921568627, 0.196078431372549, 0.0);
inline juce::Colour c87 = juce::Colour::fromRGBA(1.0, 0.709803921568627, 0.196078431372549, 1.0);
inline juce::Colour c88 = juce::Colour::fromRGBA(1.0, 0.709804, 0.196078, 0.0);
inline juce::Colour c89 = juce::Colour::fromRGBA(1.0, 0.709804, 0.196078, 1.0);
inline juce::Colour c90 = juce::Colour::fromRGBA(1.0, 0.71, 0.196, 0.0);
inline juce::Colour c91 = juce::Colour::fromRGBA(1.0, 0.71, 0.196, 1.0);
inline juce::Colour c92 = juce::Colour::fromRGBA(1.0, 0.890196, 0.0, 1.0);
inline juce::Colour c93 = juce::Colour::fromRGBA(1.0, 0.890196, 0.090196, 1.0);
inline juce::Colour c94 = juce::Colour::fromRGBA(1.0, 1.0, 1.0, 0.0);
inline juce::Colour c95 = juce::Colour::fromRGBA(1.0, 1.0, 1.0, 0.200000002980232);
inline juce::Colour c96 = juce::Colour::fromRGBA(1.0, 1.0, 1.0, 0.5);
inline juce::Colour c97 = juce::Colour::fromRGBA(1.0, 1.0, 1.0, 1.0);

}; //namespace RSL

#endif /* RSLColours_h */
