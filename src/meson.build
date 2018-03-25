version_hxx = vcs_tag(
    input  : 'version.hxx.in',
    output : 'version.hxx')

luppp_src = files(
  'audiobuffer.cxx',
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
  'jack.cxx',
  'jacksendreturn.cxx',
  'logic.cxx',
  'looperclip.cxx',
  'looper.cxx',
  'main.cxx',
  'metronome.cxx',
  'timemanager.cxx',
  'trackoutput.cxx'
  )

subdir('cjson')
subdir('dsp')
subdir('controller')
subdir('observer')
subdir('state')
subdir('avtk')

if(get_option('tests') == true)
  subdir('tests')
endif
