/*
 * Author: Harry van Haaren 2013
 *         harryhaaren@gmail.com
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */


#ifndef OPENAV_DSP_REVERB_H
#define OPENAV_DSP_REVERB_H

#include <math.h>
#include <cmath>
#include <cstdlib>

template <int N> inline float faustpower(float x)
{
	return powf(x,N);
}
template <int N> inline double faustpower(double x)
{
	return pow(x,N);
}
template <int N> inline int faustpower(int x)
{
	return faustpower<N/2>(x) * faustpower<N-N/2>(x);
}
template <> 	 inline int faustpower<0>(int x)
{
	return 1;
}
template <> 	 inline int faustpower<1>(int x)
{
	return x;
}

class Reverb // : Effect
{
public:
	Reverb(int sr)
	{
		init( sr );
	}

	int getNumInputs()
	{
		return 2;
	}
	int getNumOutputs()
	{
		return 2;
	}

	void damping(float d)
	{
		if( d > 1.0 ) d = 1.0f;
		if( d < 0.0 ) d = 0.0f;

		_damping = (1-d) * 18500 + 1500.f;
	}

	void rt60(float rt)
	{
		if( rt > 1.0 ) rt = 1.0f;
		if( rt < 0.0 ) rt = 0.0f;

		_rt60 = 1 + rt * 5;
	}

	void dryWet(float dw)
	{
		if( dw > 1.0 ) dw = 1.0f;
		if( dw < 0.0 ) dw = 0.0f;
		_dryWet = dw;
	}

	void process (int count, float** input, float** output)
	{
		float 	fSlow0 = _rt60;
		float 	fSlow1 = expf((fConst2 / fSlow0));
		float 	fSlow2 = faustpower<2>(fSlow1);
		float 	fSlow3 = (1.0f - fSlow2);
		float 	fSlow4 = cosf((fConst3 * _damping));
		float 	fSlow5 = (1.0f - (fSlow4 * fSlow2));
		float 	fSlow6 = sqrtf(max(0.f, ((faustpower<2>(fSlow5) / faustpower<2>(fSlow3)) - 1.0f)));
		float 	fSlow7 = (fSlow5 / fSlow3);
		float 	fSlow8 = (fSlow7 - fSlow6);
		float 	fSlow9 = (fSlow0 - 0.5f);
		float 	fSlow10 = ((expf((fConst2 / fSlow9)) / fSlow1) - 1);
		float 	fSlow11 = (fSlow1 * ((1.0f + fSlow6) - fSlow7));
		float 	fSlow12 = expf((fConst12 / fSlow0));
		float 	fSlow13 = faustpower<2>(fSlow12);
		float 	fSlow14 = (1.0f - fSlow13);
		float 	fSlow15 = (1.0f - (fSlow4 * fSlow13));
		float 	fSlow16 = sqrtf(max(0.f, ((faustpower<2>(fSlow15) / faustpower<2>(fSlow14)) - 1.0f)));
		float 	fSlow17 = (fSlow15 / fSlow14);
		float 	fSlow18 = (fSlow17 - fSlow16);
		float 	fSlow19 = ((expf((fConst12 / fSlow9)) / fSlow12) - 1);
		float 	fSlow20 = (fSlow12 * ((1.0f + fSlow16) - fSlow17));
		float 	fSlow21 = expf((fConst17 / fSlow0));
		float 	fSlow22 = faustpower<2>(fSlow21);
		float 	fSlow23 = (1.0f - fSlow22);
		float 	fSlow24 = (1.0f - (fSlow4 * fSlow22));
		float 	fSlow25 = sqrtf(max(0.f, ((faustpower<2>(fSlow24) / faustpower<2>(fSlow23)) - 1.0f)));
		float 	fSlow26 = (fSlow24 / fSlow23);
		float 	fSlow27 = (fSlow26 - fSlow25);
		float 	fSlow28 = ((expf((fConst17 / fSlow9)) / fSlow21) - 1);
		float 	fSlow29 = (fSlow21 * ((1.0f + fSlow25) - fSlow26));
		float 	fSlow30 = expf((fConst22 / fSlow0));
		float 	fSlow31 = faustpower<2>(fSlow30);
		float 	fSlow32 = (1.0f - fSlow31);
		float 	fSlow33 = (1.0f - (fSlow4 * fSlow31));
		float 	fSlow34 = sqrtf(max(0.f, ((faustpower<2>(fSlow33) / faustpower<2>(fSlow32)) - 1.0f)));
		float 	fSlow35 = (fSlow33 / fSlow32);
		float 	fSlow36 = (fSlow35 - fSlow34);
		float 	fSlow37 = ((expf((fConst22 / fSlow9)) / fSlow30) - 1);
		float 	fSlow38 = (fSlow30 * ((1.0f + fSlow34) - fSlow35));
		float 	fSlow39 = expf((fConst27 / fSlow0));
		float 	fSlow40 = faustpower<2>(fSlow39);
		float 	fSlow41 = (1.0f - fSlow40);
		float 	fSlow42 = (1.0f - (fSlow4 * fSlow40));
		float 	fSlow43 = sqrtf(max(0.f, ((faustpower<2>(fSlow42) / faustpower<2>(fSlow41)) - 1.0f)));
		float 	fSlow44 = (fSlow42 / fSlow41);
		float 	fSlow45 = (fSlow44 - fSlow43);
		float 	fSlow46 = ((expf((fConst27 / fSlow9)) / fSlow39) - 1);
		float 	fSlow47 = (fSlow39 * ((1.0f + fSlow43) - fSlow44));
		float 	fSlow48 = expf((fConst32 / fSlow0));
		float 	fSlow49 = faustpower<2>(fSlow48);
		float 	fSlow50 = (1.0f - fSlow49);
		float 	fSlow51 = (1.0f - (fSlow4 * fSlow49));
		float 	fSlow52 = sqrtf(max(0.f, ((faustpower<2>(fSlow51) / faustpower<2>(fSlow50)) - 1.0f)));
		float 	fSlow53 = (fSlow51 / fSlow50);
		float 	fSlow54 = (fSlow53 - fSlow52);
		float 	fSlow55 = ((expf((fConst32 / fSlow9)) / fSlow48) - 1);
		float 	fSlow56 = (fSlow48 * ((1.0f + fSlow52) - fSlow53));
		float 	fSlow57 = expf((fConst37 / fSlow0));
		float 	fSlow58 = faustpower<2>(fSlow57);
		float 	fSlow59 = (1.0f - fSlow58);
		float 	fSlow60 = (1.0f - (fSlow4 * fSlow58));
		float 	fSlow61 = sqrtf(max(0.f, ((faustpower<2>(fSlow60) / faustpower<2>(fSlow59)) - 1.0f)));
		float 	fSlow62 = (fSlow60 / fSlow59);
		float 	fSlow63 = (fSlow62 - fSlow61);
		float 	fSlow64 = ((expf((fConst37 / fSlow9)) / fSlow57) - 1);
		float 	fSlow65 = (fSlow57 * ((1.0f + fSlow61) - fSlow62));
		float 	fSlow66 = expf((fConst42 / fSlow0));
		float 	fSlow67 = faustpower<2>(fSlow66);
		float 	fSlow68 = (1.0f - fSlow67);
		float 	fSlow69 = (1.0f - (fSlow67 * fSlow4));
		float 	fSlow70 = sqrtf(max(0.f, ((faustpower<2>(fSlow69) / faustpower<2>(fSlow68)) - 1.0f)));
		float 	fSlow71 = (fSlow69 / fSlow68);
		float 	fSlow72 = (fSlow71 - fSlow70);
		float 	fSlow73 = ((expf((fConst42 / fSlow9)) / fSlow66) - 1);
		float 	fSlow74 = (fSlow66 * ((1.0f + fSlow70) - fSlow71));
		float* input0 = input[0];
		float* input1 = input[1];
		float* output0 = output[0];
		float* output1 = output[1];
		for (int i=0; i<count; i++) {
			fRec11[0] = ((fConst7 * (fRec4[1] + fRec4[2])) + (fConst6 * fRec11[1]));
			fRec10[0] = ((fSlow11 * (fRec4[1] + (fSlow10 * fRec11[0]))) + (fSlow8 * fRec10[1]));
			fVec0[IOTA&8191] = (1e-20f + (0.35355339059327373f * fRec10[0]));
			float fTemp0 = (0.3f * (float)input0[i]);
			float fTemp1 = ((fTemp0 + fVec0[(IOTA-iConst9)&8191]) - (0.6f * fRec8[1]));
			fVec1[IOTA&2047] = fTemp1;
			fRec8[0] = fVec1[(IOTA-iConst10)&2047];
			float 	fRec9 = (0.6f * fVec1[IOTA&2047]);
			fRec15[0] = ((fConst7 * (fRec0[1] + fRec0[2])) + (fConst6 * fRec15[1]));
			fRec14[0] = ((fSlow20 * (fRec0[1] + (fSlow19 * fRec15[0]))) + (fSlow18 * fRec14[1]));
			fVec2[IOTA&8191] = (1e-20f + (0.35355339059327373f * fRec14[0]));
			float fTemp2 = ((fTemp0 + fVec2[(IOTA-iConst14)&8191]) - (0.6f * fRec12[1]));
			fVec3[IOTA&1023] = fTemp2;
			fRec12[0] = fVec3[(IOTA-iConst15)&1023];
			float 	fRec13 = (0.6f * fVec3[IOTA&1023]);
			float fTemp3 = (fRec13 + fRec9);
			fRec19[0] = ((fConst7 * (fRec2[1] + fRec2[2])) + (fConst6 * fRec19[1]));
			fRec18[0] = ((fSlow29 * (fRec2[1] + (fSlow28 * fRec19[0]))) + (fSlow27 * fRec18[1]));
			fVec4[IOTA&8191] = (1e-20f + (0.35355339059327373f * fRec18[0]));
			float fTemp4 = (fVec4[(IOTA-iConst19)&8191] - (fTemp0 + (0.6f * fRec16[1])));
			fVec5[IOTA&2047] = fTemp4;
			fRec16[0] = fVec5[(IOTA-iConst20)&2047];
			float 	fRec17 = (0.6f * fVec5[IOTA&2047]);
			fRec23[0] = ((fConst7 * (fRec6[1] + fRec6[2])) + (fConst6 * fRec23[1]));
			fRec22[0] = ((fSlow38 * (fRec6[1] + (fSlow37 * fRec23[0]))) + (fSlow36 * fRec22[1]));
			fVec6[IOTA&8191] = (1e-20f + (0.35355339059327373f * fRec22[0]));
			float fTemp5 = (fVec6[(IOTA-iConst24)&8191] - (fTemp0 + (0.6f * fRec20[1])));
			fVec7[IOTA&1023] = fTemp5;
			fRec20[0] = fVec7[(IOTA-iConst25)&1023];
			float 	fRec21 = (0.6f * fVec7[IOTA&1023]);
			float fTemp6 = (fRec21 + (fRec17 + fTemp3));
			fRec27[0] = ((fConst7 * (fRec1[1] + fRec1[2])) + (fConst6 * fRec27[1]));
			fRec26[0] = ((fSlow47 * (fRec1[1] + (fSlow46 * fRec27[0]))) + (fSlow45 * fRec26[1]));
			fVec8[IOTA&16383] = (1e-20f + (0.35355339059327373f * fRec26[0]));
			float fTemp7 = (0.3f * (float)input1[i]);
			float fTemp8 = ((fTemp7 + fVec8[(IOTA-iConst29)&16383]) + (0.6f * fRec24[1]));
			fVec9[IOTA&2047] = fTemp8;
			fRec24[0] = fVec9[(IOTA-iConst30)&2047];
			float 	fRec25 = (0 - (0.6f * fVec9[IOTA&2047]));
			fRec31[0] = ((fConst7 * (fRec5[1] + fRec5[2])) + (fConst6 * fRec31[1]));
			fRec30[0] = ((fSlow56 * (fRec5[1] + (fSlow55 * fRec31[0]))) + (fSlow54 * fRec30[1]));
			fVec10[IOTA&8191] = (1e-20f + (0.35355339059327373f * fRec30[0]));
			float fTemp9 = ((fTemp7 + fVec10[(IOTA-iConst34)&8191]) + (0.6f * fRec28[1]));
			fVec11[IOTA&2047] = fTemp9;
			fRec28[0] = fVec11[(IOTA-iConst35)&2047];
			float 	fRec29 = (0 - (0.6f * fVec11[IOTA&2047]));
			fRec35[0] = ((fConst7 * (fRec3[1] + fRec3[2])) + (fConst6 * fRec35[1]));
			fRec34[0] = ((fSlow65 * (fRec3[1] + (fSlow64 * fRec35[0]))) + (fSlow63 * fRec34[1]));
			fVec12[IOTA&16383] = (1e-20f + (0.35355339059327373f * fRec34[0]));
			float fTemp10 = ((fVec12[(IOTA-iConst39)&16383] + (0.6f * fRec32[1])) - fTemp7);
			fVec13[IOTA&2047] = fTemp10;
			fRec32[0] = fVec13[(IOTA-iConst40)&2047];
			float 	fRec33 = (0 - (0.6f * fVec13[IOTA&2047]));
			fRec39[0] = ((fConst7 * (fRec7[1] + fRec7[2])) + (fConst6 * fRec39[1]));
			fRec38[0] = ((fSlow74 * (fRec7[1] + (fSlow73 * fRec39[0]))) + (fSlow72 * fRec38[1]));
			fVec14[IOTA&16383] = (1e-20f + (0.35355339059327373f * fRec38[0]));
			float fTemp11 = ((fVec14[(IOTA-iConst44)&16383] + (0.6f * fRec36[1])) - fTemp7);
			fVec15[IOTA&1023] = fTemp11;
			fRec36[0] = fVec15[(IOTA-iConst45)&1023];
			float 	fRec37 = (0 - (0.6f * fVec15[IOTA&1023]));
			fRec0[0] = (fRec12[1] + (fRec8[1] + (fRec16[1] + (fRec20[1] + (fRec24[1] + (fRec28[1] + (fRec32[1] + (fRec36[1] + (fRec37 + (fRec33 + (fRec29 + (fRec25 + fTemp6))))))))))));
			fRec1[0] = (0 - ((fRec24[1] + (fRec28[1] + (fRec32[1] + (fRec36[1] + (fRec37 + (fRec33 + (fRec25 + fRec29))))))) - (fRec12[1] + (fRec8[1] + (fRec16[1] + (fRec20[1] + fTemp6))))));
			float fTemp12 = (fRec17 + fRec21);
			fRec2[0] = (0 - ((fRec16[1] + (fRec20[1] + (fRec32[1] + (fRec36[1] + (fRec37 + (fRec33 + fTemp12)))))) - (fRec12[1] + (fRec8[1] + (fRec24[1] + (fRec28[1] + (fRec29 + (fRec25 + fTemp3))))))));
			fRec3[0] = (0 - ((fRec16[1] + (fRec20[1] + (fRec24[1] + (fRec28[1] + (fRec29 + (fRec25 + fTemp12)))))) - (fRec12[1] + (fRec8[1] + (fRec32[1] + (fRec36[1] + (fRec37 + (fRec33 + fTemp3))))))));
			float fTemp13 = (fRec13 + fRec17);
			float fTemp14 = (fRec9 + fRec21);
			fRec4[0] = (0 - ((fRec8[1] + (fRec20[1] + (fRec28[1] + (fRec36[1] + (fRec37 + (fRec29 + fTemp14)))))) - (fRec12[1] + (fRec16[1] + (fRec24[1] + (fRec32[1] + (fRec33 + (fRec25 + fTemp13))))))));
			fRec5[0] = (0 - ((fRec8[1] + (fRec20[1] + (fRec24[1] + (fRec32[1] + (fRec33 + (fRec25 + fTemp14)))))) - (fRec12[1] + (fRec16[1] + (fRec28[1] + (fRec36[1] + (fRec37 + (fRec29 + fTemp13))))))));
			float fTemp15 = (fRec13 + fRec21);
			float fTemp16 = (fRec9 + fRec17);
			fRec6[0] = (0 - ((fRec8[1] + (fRec16[1] + (fRec28[1] + (fRec32[1] + (fRec33 + (fRec29 + fTemp16)))))) - (fRec12[1] + (fRec20[1] + (fRec24[1] + (fRec36[1] + (fRec37 + (fRec25 + fTemp15))))))));
			fRec7[0] = (0 - ((fRec8[1] + (fRec16[1] + (fRec24[1] + (fRec36[1] + (fRec37 + (fRec25 + fTemp16)))))) - (fRec12[1] + (fRec20[1] + (fRec28[1] + (fRec32[1] + (fRec33 + (fRec29 + fTemp15))))))));


			float reverb0 = (float)(0.37f * (fRec1[0] + fRec2[0]));
			float reverb1 = (float)(0.37f * (fRec1[0] - fRec2[0]));

			output0[i] = (input0[i] * (1-_dryWet)) + (reverb0 * _dryWet );
			output1[i] = (input1[i] * (1-_dryWet)) + (reverb1 * _dryWet );

			// post processing
			fRec7[2] = fRec7[1];
			fRec7[1] = fRec7[0];
			fRec6[2] = fRec6[1];
			fRec6[1] = fRec6[0];
			fRec5[2] = fRec5[1];
			fRec5[1] = fRec5[0];
			fRec4[2] = fRec4[1];
			fRec4[1] = fRec4[0];
			fRec3[2] = fRec3[1];
			fRec3[1] = fRec3[0];
			fRec2[2] = fRec2[1];
			fRec2[1] = fRec2[0];
			fRec1[2] = fRec1[1];
			fRec1[1] = fRec1[0];
			fRec0[2] = fRec0[1];
			fRec0[1] = fRec0[0];
			fRec36[1] = fRec36[0];
			fRec38[1] = fRec38[0];
			fRec39[1] = fRec39[0];
			fRec32[1] = fRec32[0];
			fRec34[1] = fRec34[0];
			fRec35[1] = fRec35[0];
			fRec28[1] = fRec28[0];
			fRec30[1] = fRec30[0];
			fRec31[1] = fRec31[0];
			fRec24[1] = fRec24[0];
			fRec26[1] = fRec26[0];
			fRec27[1] = fRec27[0];
			fRec20[1] = fRec20[0];
			fRec22[1] = fRec22[0];
			fRec23[1] = fRec23[0];
			fRec16[1] = fRec16[0];
			fRec18[1] = fRec18[0];
			fRec19[1] = fRec19[0];
			fRec12[1] = fRec12[0];
			fRec14[1] = fRec14[0];
			fRec15[1] = fRec15[0];
			fRec8[1] = fRec8[0];
			IOTA = IOTA+1;
			fRec10[1] = fRec10[0];
			fRec11[1] = fRec11[0];
		}
	}

private:
	float _dryWet;
	float _rt60;
	int iConst0;
	float fConst1;
	float fConst2;
	float _damping;
	float fConst3;
	float fConst4;
	float fConst5;
	float fConst6;
	float fConst7;
	float fRec11[2];
	float fRec10[2];
	int   IOTA;
	float fVec0[8192];
	float fConst8;
	int iConst9;
	float fVec1[2048];
	int iConst10;
	float fRec8[2];
	float fConst11;
	float fConst12;
	float fRec15[2];
	float fRec14[2];
	float fVec2[8192];
	float fConst13;
	int iConst14;
	float fVec3[1024];
	int iConst15;
	float fRec12[2];
	float fConst16;
	float fConst17;
	float fRec19[2];
	float fRec18[2];
	float fVec4[8192];
	float fConst18;
	int iConst19;
	float fVec5[2048];
	int iConst20;
	float fRec16[2];
	float fConst21;
	float fConst22;
	float fRec23[2];
	float fRec22[2];
	float fVec6[8192];
	float fConst23;
	int iConst24;
	float fVec7[1024];
	int iConst25;
	float fRec20[2];
	float fConst26;
	float fConst27;
	float fRec27[2];
	float fRec26[2];
	float fVec8[16384];
	float fConst28;
	int iConst29;
	float fVec9[2048];
	int iConst30;
	float fRec24[2];
	float fConst31;
	float fConst32;
	float fRec31[2];
	float fRec30[2];
	float fVec10[8192];
	float fConst33;
	int iConst34;
	float fVec11[2048];
	int iConst35;
	float fRec28[2];
	float fConst36;
	float fConst37;
	float fRec35[2];
	float fRec34[2];
	float fVec12[16384];
	float fConst38;
	int iConst39;
	float fVec13[2048];
	int iConst40;
	float fRec32[2];
	float fConst41;
	float fConst42;
	float fRec39[2];
	float fRec38[2];
	float fVec14[16384];
	float fConst43;
	int iConst44;
	float fVec15[1024];
	int iConst45;
	float fRec36[2];
	float fRec0[3];
	float fRec1[3];
	float fRec2[3];
	float fRec3[3];
	float fRec4[3];
	float fRec5[3];
	float fRec6[3];
	float fRec7[3];

	/// Long nasty function setting initial values
	void init(int samplingFreq)
	{
		// wet by default!
		_dryWet = 1;

		_rt60 = 5.0f;
		iConst0 = min(192000, max(1, samplingFreq));
		fConst1 = floorf((0.5f + (0.174713f * iConst0)));
		fConst2 = ((0 - (6.907755278982138f * fConst1)) / iConst0);
		_damping = 10000.f;
		fConst3 = (6.283185307179586f / float(iConst0));
		fConst4 = (1.0f / tanf((1256.6370614359173f / iConst0)));
		fConst5 = (1 + fConst4);
		fConst6 = (0 - ((1 - fConst4) / fConst5));
		fConst7 = (1.0f / fConst5);
		for (int i=0; i<2; i++) fRec11[i] = 0;
		for (int i=0; i<2; i++) fRec10[i] = 0;
		IOTA = 0;
		for (int i=0; i<8192; i++) fVec0[i] = 0;
		fConst8 = floorf((0.5f + (0.022904f * iConst0)));
		iConst9 = int((int((fConst1 - fConst8)) & 8191));
		for (int i=0; i<2048; i++) fVec1[i] = 0;
		iConst10 = int((int((fConst8 - 1)) & 2047));
		for (int i=0; i<2; i++) fRec8[i] = 0;
		fConst11 = floorf((0.5f + (0.153129f * iConst0)));
		fConst12 = ((0 - (6.907755278982138f * fConst11)) / iConst0);
		for (int i=0; i<2; i++) fRec15[i] = 0;
		for (int i=0; i<2; i++) fRec14[i] = 0;
		for (int i=0; i<8192; i++) fVec2[i] = 0;
		fConst13 = floorf((0.5f + (0.020346f * iConst0)));
		iConst14 = int((int((fConst11 - fConst13)) & 8191));
		for (int i=0; i<1024; i++) fVec3[i] = 0;
		iConst15 = int((int((fConst13 - 1)) & 1023));
		for (int i=0; i<2; i++) fRec12[i] = 0;
		fConst16 = floorf((0.5f + (0.127837f * iConst0)));
		fConst17 = ((0 - (6.907755278982138f * fConst16)) / iConst0);
		for (int i=0; i<2; i++) fRec19[i] = 0;
		for (int i=0; i<2; i++) fRec18[i] = 0;
		for (int i=0; i<8192; i++) fVec4[i] = 0;
		fConst18 = floorf((0.5f + (0.031604f * iConst0)));
		iConst19 = int((int((fConst16 - fConst18)) & 8191));
		for (int i=0; i<2048; i++) fVec5[i] = 0;
		iConst20 = int((int((fConst18 - 1)) & 2047));
		for (int i=0; i<2; i++) fRec16[i] = 0;
		fConst21 = floorf((0.5f + (0.125f * iConst0)));
		fConst22 = ((0 - (6.907755278982138f * fConst21)) / iConst0);
		for (int i=0; i<2; i++) fRec23[i] = 0;
		for (int i=0; i<2; i++) fRec22[i] = 0;
		for (int i=0; i<8192; i++) fVec6[i] = 0;
		fConst23 = floorf((0.5f + (0.013458f * iConst0)));
		iConst24 = int((int((fConst21 - fConst23)) & 8191));
		for (int i=0; i<1024; i++) fVec7[i] = 0;
		iConst25 = int((int((fConst23 - 1)) & 1023));
		for (int i=0; i<2; i++) fRec20[i] = 0;
		fConst26 = floorf((0.5f + (0.210389f * iConst0)));
		fConst27 = ((0 - (6.907755278982138f * fConst26)) / iConst0);
		for (int i=0; i<2; i++) fRec27[i] = 0;
		for (int i=0; i<2; i++) fRec26[i] = 0;
		for (int i=0; i<16384; i++) fVec8[i] = 0;
		fConst28 = floorf((0.5f + (0.024421f * iConst0)));
		iConst29 = int((int((fConst26 - fConst28)) & 16383));
		for (int i=0; i<2048; i++) fVec9[i] = 0;
		iConst30 = int((int((fConst28 - 1)) & 2047));
		for (int i=0; i<2; i++) fRec24[i] = 0;
		fConst31 = floorf((0.5f + (0.192303f * iConst0)));
		fConst32 = ((0 - (6.907755278982138f * fConst31)) / iConst0);
		for (int i=0; i<2; i++) fRec31[i] = 0;
		for (int i=0; i<2; i++) fRec30[i] = 0;
		for (int i=0; i<8192; i++) fVec10[i] = 0;
		fConst33 = floorf((0.5f + (0.029291f * iConst0)));
		iConst34 = int((int((fConst31 - fConst33)) & 8191));
		for (int i=0; i<2048; i++) fVec11[i] = 0;
		iConst35 = int((int((fConst33 - 1)) & 2047));
		for (int i=0; i<2; i++) fRec28[i] = 0;
		fConst36 = floorf((0.5f + (0.256891f * iConst0)));
		fConst37 = ((0 - (6.907755278982138f * fConst36)) / iConst0);
		for (int i=0; i<2; i++) fRec35[i] = 0;
		for (int i=0; i<2; i++) fRec34[i] = 0;
		for (int i=0; i<16384; i++) fVec12[i] = 0;
		fConst38 = floorf((0.5f + (0.027333f * iConst0)));
		iConst39 = int((int((fConst36 - fConst38)) & 16383));
		for (int i=0; i<2048; i++) fVec13[i] = 0;
		iConst40 = int((int((fConst38 - 1)) & 2047));
		for (int i=0; i<2; i++) fRec32[i] = 0;
		fConst41 = floorf((0.5f + (0.219991f * iConst0)));
		fConst42 = ((0 - (6.907755278982138f * fConst41)) / iConst0);
		for (int i=0; i<2; i++) fRec39[i] = 0;
		for (int i=0; i<2; i++) fRec38[i] = 0;
		for (int i=0; i<16384; i++) fVec14[i] = 0;
		fConst43 = floorf((0.5f + (0.019123f * iConst0)));
		iConst44 = int((int((fConst41 - fConst43)) & 16383));
		for (int i=0; i<1024; i++) fVec15[i] = 0;
		iConst45 = int((int((fConst43 - 1)) & 1023));
		for (int i=0; i<2; i++) fRec36[i] = 0;
		for (int i=0; i<3; i++) fRec0[i] = 0;
		for (int i=0; i<3; i++) fRec1[i] = 0;
		for (int i=0; i<3; i++) fRec2[i] = 0;
		for (int i=0; i<3; i++) fRec3[i] = 0;
		for (int i=0; i<3; i++) fRec4[i] = 0;
		for (int i=0; i<3; i++) fRec5[i] = 0;
		for (int i=0; i<3; i++) fRec6[i] = 0;
		for (int i=0; i<3; i++) fRec7[i] = 0;
	}
};

#endif // OPENAV_DSP_REVERB_H
