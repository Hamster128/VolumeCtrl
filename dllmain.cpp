// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include "VolumeCtrl.h"

/**************************************************************************************/

AudioEffect* createEffectInstance(audioMasterCallback audioMaster)
{
  return new CVolumeCtrl(audioMaster);
}

