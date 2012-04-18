#!/usr/bin/python

VERSION='0.0.1'
APPNAME='Luppp'
top = './'
out = './.build'

def options(ctx):
  ctx.tool_options('compiler_cxx')
  
  ctx.add_option('--debug', action='store', default=True, help='Enable debuggable build')

def init(null):
  #print '\nInitializing Luppp...'
  pass

def configure(ctx):
  
  ctx.check_tool ('compiler_cxx')
  
  ctx.env.CXXFLAGS = ['-pg','-g','-ldl','-Wall','-std=gnu++0x','-Wextra','-fpermissive']
  
  #   Engine Depends
  ctx.check_cfg(package='jack',at_least_version='0.118',args='--cflags --libs',uselib_store='JACK')
  ctx.check_cfg(package='sndfile',args='--cflags --libs',uselib_store='SNDFILE')
  ctx.check_cfg(package='gtkmm-2.4',at_least_version='2.4.0',args='--cflags --libs',uselib_store='GTKMM')
  ctx.check_cfg(package='glibmm-2.4',at_least_version='2.0.0',args='--cflags --libs',uselib_store='GLIBMM')
  ctx.check_cfg(package='fluidsynth',args='--libs',uselib_store='FLUIDSYNTH')
  ctx.check_cfg(package='libconfig++',at_least_version='1.4.8',args='--libs',uselib_store='LIBCONFIG')
  ctx.check_cfg(package='lilv-0',args='--cflags --libs',uselib_store='LILV')
  ctx.check_cfg(package='suil-0',args='--cflags --libs',uselib_store='SUIL')
  ctx.check_cfg(package='liblo',at_least_version='1.0.0',args='--cflags --libs',uselib_store='LIBLO') # harmonySeq integration
  
  # Check for headers:
  ctx.check(header_name="lv2.h",mandatory=True,uselib_store='LV2.H')
  ctx.check(header_name="ladspa.h",mandatory=True,uselib_store='LADSPA.H')
  ctx.check(header_name="lv2/lv2plug.in/ns/ext/event/event.h",mandatory=True,uselib_store='EVENT.H')
  ctx.check(header_name="lv2/lv2plug.in/ns/ext/uri-map/uri-map.h",mandatory=True,uselib_store='URIMAP.H')
  ctx.check(header_name="lv2/lv2plug.in/ns/ext/event/event-helpers.h",mandatory=True,uselib_store='EVENTHELPERS.H')

def build(ctx):
  # copy the needed materials to the build folder
  ctx.add_post_fun(copyResources)
  
  #     ENGINE
  engineList=['src/top.cpp',
              'src/rtqueue.cpp',
              'src/lvtwohost.cpp',
              'src/audioqueue.cpp',
              'src/audiosource.cpp',
              'src/audiobuffer.cpp',
              'src/offlineworker.cpp',
              'src/ladspahost.cpp',
              'src/jackaudiosource.cpp',
              'src/bufferaudiosource.cpp',
              'src/fluidsynthaudiosource.cpp',
              'src/whitenoiseaudiosource.cpp',
              'src/trancegate.cpp',
              'src/mixer.cpp',
              'src/settings.cpp',
              'src/effect.cpp',
              'src/beatsmash.cpp',
              'src/statestore.cpp',
              'src/engineevent.cpp',
              'src/audiotrack.cpp',
              'src/time.cpp',
              'src/controller.cpp',
              'src/audiosinkoutput.cpp',
              'src/jackclient.cpp']
  
  guiList = [ 'src/g_window.cpp',
              'src/g_track.cpp',
              'src/g_buffersource.cpp',
              'src/waveformcache.cpp',
              'src/g_widgets.cpp',
              'src/g_sends.cpp',
              'src/g_automove.cpp',
              'src/g_waveview.cpp',
              'src/g_scope.cpp',
              'src/g_masterprogress.cpp',
              'src/g_delay.cpp',
              'src/g_reverb.cpp',
              'src/g_lvtwodisplay.cpp',
              'src/g_fileselector.cpp',
              'src/g_effectselector.cpp',
              'src/g_instrumentselector.cpp',
              'src/g_beatsmash.cpp',
              'src/g_ampitchshift.cpp',
              'src/g_widgetbase.cpp',
              'src/g_statestore.cpp',
              'src/g_equalizer.cpp',
              'src/g_transient.cpp',
              'src/g_progress.cpp',
              'src/g_limiter.cpp',
              'src/g_compressor.cpp',
              'src/g_audiosource.cpp',
              'src/g_mastersource.cpp',
              'src/g_masterreturn.cpp',
              'src/g_frequencygraph.cpp',
              'src/g_titlebar.cpp',
              'src/g_block.cpp',
              'src/g_mute.cpp',
              'src/g_lowpass.cpp',
              'src/g_lowpass_small.cpp',
              'src/g_highpass.cpp',
              'src/g_rec.cpp',
              'src/g_solo.cpp',
              'src/g_fader.cpp',
              'src/g_dial.cpp',
              'src/g_clipselector.cpp',
              'src/g_masteroutput.cpp',
              'src/g_trackoutput.cpp']
  
  engineDepends = 'JACK SNDFILE GTKMM FLUIDSYNTH GLIBMM LIBCONFIG LILV LV2.H LADSPA.H SUIL EVENT.H URIMAP.H EVENTHELPERS.H LIBLO'
  
  #print 'Building ENGINE'
  buildList = engineList + guiList
  dependList = engineDepends
  
  #print 'Depends list:',dependList
  
  lib = ctx.new_task_gen(
    features  = 'cxx cxxstlib',
    source    = buildList,
    target    = 'engine_gui',
    uselib    = engineDepends,
    export_includes = '.')
  
  
  main = ctx.new_task_gen(
    features  = 'cxx cprogram',
    source    = 'src/main.cpp',
    use       = 'engine_gui',
    uselib    = engineDepends,
    target    = 'luppp' )




def copyResources(ctx):
  print ( 'Copying resources...' )
  import shutil, os
  
  sourceList = ['src/ui.glade',
                'src/headphones.png',
                'src/lupppIcon.png' ]
  
  destination = '.build/'
  
  # print ( os.getcwd() )
  for currentFilename in sourceList:
    print ( 'Copying ' + currentFilename + ' to ' + destination )
    shutil.copy( os.path.join( os.getcwd(), currentFilename ), os.path.join( os.getcwd(), destination ) )
