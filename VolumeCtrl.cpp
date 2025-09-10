#include "pch.h"
#include "VolumeCtrl.h"
#include "httplib.h"
#include <thread>
#include <atomic>
#include <algorithm>

/*
http://localhost:8088/volume?value=1.0
*/

#define INITIAL_PERCENT 20
#define MIN_DB -50.0f
#define MAX_DB  15.0f

static std::atomic<float> gVolume(1.0f);
int percent = INITIAL_PERCENT;

CVolumeCtrl::CVolumeCtrl(audioMasterCallback audioMaster) : AudioEffectX(audioMaster, 1, NUM_PARAMS)
{
  float dB = MIN_DB + (percent / 100.0f) * (MAX_DB - MIN_DB);
  fCurrentVol = std::pow(10.0f, dB / 20.0f);
  gVolume.store(fCurrentVol);

  if (audioMaster)
  {
    setNumInputs(NUM_INPUTS);
    setNumOutputs(NUM_OUTPUTS);
    canProcessReplacing();
    isSynth();
    setUniqueID('VCRT');
    programsAreChunks(true);
  }

  // Launch HTTP server in background thread
  std::thread([]() {
    httplib::Server svr;

    svr.Get("/volume", [](const httplib::Request& req, httplib::Response& res) {

      if (req.has_param("value")) {
        std::string value = req.get_param_value("value");

        if (value == "up") {
          percent += 2;
        }
        else if (value == "down") {
          percent -= 2;
        }
        else {
          percent = std::stoi(value);
        }

        // Map 0–100 ->  dB to 0 dB
        float dB = MIN_DB + (percent / 100.0f) * (MAX_DB - MIN_DB);

        // Convert dB to linear gain
        float newVol = std::pow(10.0f, dB / 20.0f);

        gVolume.store(newVol);
      }

      percent = std::clamp(percent, 0, 100);

      res.set_content(std::to_string(percent), "text / plain");
    });


    // Run on localhost:8080
    svr.listen("0.0.0.0", 8088);
  }).detach();
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

  float vol = gVolume.load();

  for (VstInt32 i = 0; i < sampleFrames; i++)
  {
    if (fCurrentVol > vol) {
      fCurrentVol -= 0.001;

      if (fCurrentVol < vol)
        fCurrentVol = vol;
    }
    else if (fCurrentVol < vol) {
      fCurrentVol += 0.001;

      if (fCurrentVol > vol)
        fCurrentVol = vol;
    }

    outL[i] = inL[i] * fCurrentVol;
    outR[i] = inR[i] * fCurrentVol;
  }
}
