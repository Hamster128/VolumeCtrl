# VolumeCtrl
VST plugin which is a volume control which can be controlled via http api
I use it with EqualizerAPO and iTunesRemote to control the volume


GET http://localhost:8088/volume?value=50
0-100

GET http://localhost:8088/volume?value=up
GEThttp://localhost:8088/volume?value=down

GET http://localhost:8088/volume
