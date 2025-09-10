#pragma once
#include "pch.h"

#define NUM_PARAMS 0
#define NUM_INPUTS 2
#define NUM_OUTPUTS 2

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

  float fCurrentVol;
};

