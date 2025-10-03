#include "pch.h"
#include "VolumeCtrl.h"
#include "httplib.h"
#include <thread>
#include <atomic>
#include <algorithm>

/*
http://localhost:8088/volume?value=20
http://localhost:8088/volume?value=up
http://localhost:8088/volume?value=down
*/

#define INITIAL_PERCENT 20

static const char* SHM_NAME = "VolumeCtrlSharedMemory";
struct VolumeData volData;

static VolumeData* shared = nullptr;
float fMinDB = -50.0f;
float fMaxDB = 15.0f;
float fCurrentVol = 0;

static httplib::Server svr;
static std::thread gHttpThread;
static std::atomic<bool> gServerRunning{ false };

/**************************************************************************************/
static VolumeData* GetSharedVolume() {
  static VolumeData* shared = nullptr;

  if (shared) 
    return shared;

  HANDLE hMapFile = CreateFileMappingA(
    INVALID_HANDLE_VALUE,    // use paging file (RAM only)
    NULL,
    PAGE_READWRITE,
    0,
    sizeof(VolumeData),
    SHM_NAME
  );

  if (!hMapFile) {
    shared = &volData;
    shared->percent = INITIAL_PERCENT;
  }
  else
  {
    shared = (VolumeData*)MapViewOfFile(
      hMapFile,
      FILE_MAP_ALL_ACCESS,
      0, 0,
      sizeof(VolumeData)
    );

    if (GetLastError() == ERROR_ALREADY_EXISTS) {
      // another instance already created it
    }
    else
      shared->percent = INITIAL_PERCENT;
  }

  return shared;
}

/**************************************************************************************/
CVolumeCtrl::CVolumeCtrl(audioMasterCallback audioMaster) : AudioEffectX(audioMaster, 1, NUM_PARAMS)
{
  if (audioMaster)
  {
    setNumInputs(NUM_INPUTS);
    setNumOutputs(NUM_OUTPUTS);
    canProcessReplacing();
    isSynth();
    setUniqueID('VCRT');
    programsAreChunks(true);
  }

  float dB = fMinDB + (GetSharedVolume()->percent / 100.0f) * (fMaxDB - fMinDB);
  GetSharedVolume()->fCurrentVol = std::pow(10.0f, dB / 20.0f);

  if (!gServerRunning.exchange(true)) {
    std::thread([]() {
      svr.Get("/volume", [](const httplib::Request& req, httplib::Response& res) {

        if (req.has_param("value")) {
          std::string value = req.get_param_value("value");

          if (value == "up") {
            GetSharedVolume()->percent += 2;
          }
          else if (value == "down") {
            GetSharedVolume()->percent -= 2;
          }
          else {
            GetSharedVolume()->percent = std::stoi(value);
          }

          GetSharedVolume()->percent = std::clamp(GetSharedVolume()->percent, 0, 100);

          // Map 0-100 ->  dB to 0 dB
          float dB = fMinDB + (GetSharedVolume()->percent / 100.0f) * (fMaxDB - fMinDB);

          // Convert dB to linear gain
          GetSharedVolume()->fCurrentVol = std::pow(10.0f, dB / 20.0f);
        }

        res.set_content(std::to_string(GetSharedVolume()->percent) + " " + std::to_string(GetSharedVolume()->fCurrentVol) + " " + std::to_string(fCurrentVol) + " " + std::to_string(fMinDB) + " " + std::to_string(fMaxDB), "text / plain");
      });

      // Run on localhost:8080
      while (!svr.listen("0.0.0.0", 8088))
      {
        Sleep(100);
      }

    }).detach();
  }

}

/**************************************************************************************/
CVolumeCtrl::~CVolumeCtrl()
{
}

/**************************************************************************************/
bool CVolumeCtrl::getVendorString(char* text)
{
  vst_strncpy(text, "henry-kellner.net", kVstMaxVendorStrLen);
  return true;
}

/**************************************************************************************/
bool CVolumeCtrl::getProductString(char* text)
{
  vst_strncpy(text, "VolumeCtrl", kVstMaxProductStrLen);
  return true;
}

/**************************************************************************************/
bool CVolumeCtrl::getEffectName(char* name)
{
  vst_strncpy(name, "VolumeCtrl", kVstMaxEffectNameLen);
  return true;
}

/**************************************************************************************/
VstInt32 CVolumeCtrl::getVendorVersion()
{
  return 1002;
}

/**************************************************************************************/
void CVolumeCtrl::processReplacing(float** inputs, float** outputs, VstInt32 sampleFrames)
{
  float* inL = inputs[0];   // left input channel
  float* inR = inputs[1];   // right input channel

  float* outL = outputs[0]; // left output channel
  float* outR = outputs[1]; // right output channel

  float vol = GetSharedVolume()->fCurrentVol;

  for (VstInt32 i = 0; i < sampleFrames; i++)
  {
    if (fCurrentVol > vol) {
      fCurrentVol -= 0.0001;

      if (fCurrentVol < vol)
        fCurrentVol = vol;
    }
    else if (fCurrentVol < vol) {
      fCurrentVol += 0.0001;

      if (fCurrentVol > vol)
        fCurrentVol = vol;
    }

    outL[i] = inL[i] * fCurrentVol;
    outR[i] = inR[i] * fCurrentVol;
  }
}

