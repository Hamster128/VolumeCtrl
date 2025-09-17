# VolumeCtrl
VST plugin which is a volume control which can be controlled via http api
I use it with EqualizerAPO and iTunesRemote to control the volume


`GET http://localhost:8088/volume?value=50`
0-100

`GET http://localhost:8088/volume?value=up`

`GET http://localhost:8088/volume?value=down`

`GET http://localhost:8088/volume`


## Example for controlling it with AutoHotKey

```
; --- Overlay Function ---
overlayTime := 2000   ; ms to keep overlay visible

ShowOverlay(msg) {
    global overlayTime
    Progress, B1 FS10 ZH0 W60 X50 Y210, %msg%, , , Arial
    SetTimer, HideOverlayFunc, -%overlayTime%
}

HideOverlayFunc() {
    Progress, Off
}

HttpGet(url) {
  try {
      whr := ComObjCreate("WinHttp.WinHttpRequest.5.1")
      whr.Open("GET", url, true)
      whr.Send()
      whr.WaitForResponse()
      return whr.ResponseText
    } catch e {
        return "off"
    }    
}

~Volume_Up::
vol := HttpGet("http://127.0.0.1:8088/volume?value=up")
ShowOverlay(vol)
return

~Volume_Down::
vol := HttpGet("http://127.0.0.1:8088/volume?value=down")
ShowOverlay(vol)
return
```