version_hxx = vcs_tag(
    input  : 'version.hxx.in',
    output : 'version.hxx',
    fallback: 'Version info could not be read.')

luppp_src = files(
  'audiobuffer.cxx',
  'audioengine.cxx',
  'controllerupdater.cxx',
  'debug.cxx',
  'diskreader.cxx',
  'diskwriter.cxx',
  'event.cxx',
  'eventhandlerdsp.cxx',
  'eventhandlergui.cxx',
  'gaudioeditor.cxx',
  'gmastertrack.cxx',
  'goptions.cxx',
  'gridlogic.cxx',
  'gtrack.cxx',
  'gui.cxx',
  'logic.cxx',
  'looperclip.cxx',
  'looper.cxx',
  'main.cxx',
  'metronome.cxx',
  'timemanager.cxx',
  'trackoutput.cxx'
  )

# Add backend-specific sources
if get_option('audio_backend') == 'jack'
  luppp_src += files(
    'jack.cxx',
    'jacksendreturn.cxx',
  )
elif get_option('audio_backend') == 'rtaudio'
  luppp_src += files(
    'rtaudiomidi/RtAudio.cpp',
    'rtaudiomidi/RtMidi.cpp',
  )
endif

subdir('cjson')
subdir('dsp')
subdir('controller')
subdir('observer')
subdir('state')
subdir('avtk')

if(get_option('tests') == true)
  subdir('tests')
endif
