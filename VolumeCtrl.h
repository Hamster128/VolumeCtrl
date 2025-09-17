#pragma once
#include "pch.h"

#define NUM_INPUTS 2
#define NUM_OUTPUTS 2

struct VolumeData {
  int percent;
};

enum {
  kParamVolume = 0,
  kParamMinDB,
  kParamMaxDB,
  NUM_PARAMS
};

class CVolumeCtrl : public AudioEffectX
{
public:
  CVolumeCtrl(audioMasterCallback audioMaster);
  virtual ~CVolumeCtrl();
  bool getVendorString(char * text);
  bool getProductString(char * text);
  bool getEffectName(char * name);
  VstInt32 getVendorVersion();
  void processReplacing(float ** inputs, float ** outputs, VstInt32 sampleFrames);

  void setParameter(VstInt32 index, float value);
  float getParameter(VstInt32 index);

  void getParameterName(VstInt32 index, char * label);
  void getParameterDisplay(VstInt32 index, char * text);
  void getParameterLabel(VstInt32 index, char * label);

  float fCurrentVol;

private:
  int percent;
  float fMinDB;
  float fMaxDB;
};

