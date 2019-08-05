# TeamSpeak3-GSI
[WIP] TeamSpeak3 integration for Project Aurora


Seriously... this doesn't work yet - for now it will display some of the events in TS3 console.

To display TeamSpeak3 console, run it with ``-console`` parameter.

# How to test
1. Build everything in Visual Studio (be sure to compile it with correct CPU arch - 32bit for 32bit TS, 64bit for 64bit TS - otherwise the plugin won't be loaded).

2. After succesfull building, copy ``.dll`` file to ``%appdata%/TS3Client/plugins`` folder and be sure it's correctly loaded and enabled in TeamSpeak itself

3. Do stuff in TS and observe console


-----
### Currently properly displayed events
* Connecting to server (all 4 stages)
* Moving between channels
* New user connected/disconnected
* Receiving PM messages (pokes)
* Receiving text messages in channel
* Talking users' username
-----

### Issues
* Moving between channels and new user connections/disconnections are displayed as the same message
* Sending texts in channel displays message in console (just like receiving it)
* Hotkey detection not wokring
-----

### TODO
* Make it work with Aurora - send messages as post events to Aurora's http server
* Fix formatting
* Try to fix hotkeys detection
* Check if kicks from channel/server are working properly
