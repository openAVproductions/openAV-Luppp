

#ifndef LUPPP_EQUALIZERSTATE
#define LUPPP_EQUALIZERSTATE

enum FrequencyGraphType {
  FREQUENCY_GRAPH_TYPE_LOWPASS,
  FREQUENCY_GRAPH_TYPE_HIGHPASS,
  FREQUENCY_GRAPH_TYPE_PARAMETRIC,
};

struct EqualizerState {
  bool active;
  FrequencyGraphType type;
  float Q[4];
  float gain[4];
  float cutoffFreq[4];
};

#endif
