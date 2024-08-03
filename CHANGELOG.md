# openAV Luppp Changelog

## 1.2.1 Monday 15th April 2019

### Features
*   Print version number with `luppp --version`

### Improvements
*   Minor meson build improvements
*   clang-format configuration to improve code style

## 1.2: Tuesday 24th July 2018

### Features
*   clear clip with MIDI
*   build with meson
*   space triggers special clip
*   manual BPM input (right click on Tap-Button)

### Improvements
*   avoid noise on all controls
*   reduce default metronome volume
*   make label code consistent
*   fix compiler warnings
*   remove all hard coded scene numbers
*   add some debug outputs
*   metronome fancy fades
*   better icon file

### Fixes
*   fix several leaks and errors
*   fix broken waveforms
*   fix fltk/ntk conflict
*   fix generic MIDI launch bug
*   fix wrong output mapping
*   fix input signal flow
*   fix input volume for clip recording
*   fix timing issues after changing playspeed
*   fix scenes losing names once a scene is played

## 1.1.1: Sunday 9th April 2017
### Cleanup
*   astyle: global restyle of codebase
*   cleanup startup prints and blank lines
*   cmake: update makefiles to not clobber flags vars

### Features
*   create project dir if not already there
*   pan for each track
*   full stereo signal chain
*   cmake: fix build flags to enable optimized builds on x86_64

### Fixes
*   fix possible mem leak if loading a MIDI binding fails
*   fixed possible mem leak in goptions dialog

## 1.1: Saturday 24th December 2016
### Features
*   Added saveDirectory option to set the save dir for Luppp sessions
*   Added possibility to enable metronom on startup and set volume
*   Added possibility to save clips
*   Added per track outputs
*   Added per track sends/returns
*   Added Lag function on slider to avoid jitter
*   Added hotkey symbols for en-GB Layout
*   Added french version of app data and desktop file
*   Added .ctrl file for Launchpad S and APC Mini and APC Key25
*   Added ability to adjust knobs with mouse wheel
*   Added metronome volume
*   New header images

### Fixes
*   Fixed internal buffer misconduct
*   Fixed glitches on beats/bars
*   Fixed Jack transport state control
*   Fixed loosing sync after some time
*   Fixed tempo tap issues while transport is not running
*   Fixed horizontal Slider issue
*   Fixed glitches in output
*   Fixed metronome MIDI mapping
*   Fixed track volume not becoming totally silent

### Improvements
*   Improved Jack Transport 
*   Improved pitch shifting
*   Improved TimeManager
*   Improved building on non x86_64 plattforms
*   Improved BPM Dial

## 1.0.1: Saturday 30th August 2014

*   Updated header graphics
*   CLang static analysis
*   Icon updated & shows

*   Metronome moved from master out to headphones port
*   Special clip keybindings,
*   Captital keybindings for clear clip

*   AudioEditor shows green / red based on BPM analysis of the sample 
*   Clearing clips bug fixed
*   Loading bug re-trigger bug fixed

## 1.0: Tuesday, 10th December 2013

## Announced: Friday, 5th December 2013
