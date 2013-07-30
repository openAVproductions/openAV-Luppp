#! /usr/bin/env python

VERSION='5.0.0'
APPNAME='Luppp'

top = '.'
out = '.build'

def options(opt):
  opt.load('compiler_cxx')

def configure(conf):
  conf.load('compiler_cxx')
  #conf.env.append_value('CXXFLAGS', ['-g', '-Wall','-Werror','-Wextra']) #'-O2'
  conf.env.append_value('CXXFLAGS', ['-g', '-Wall','-march=native','-msse','-mfpmath=sse','-ffast-math']) #'-O2','-Werror'
  conf.check_cfg(package='ntk',at_least_version='1.3',args='--cflags --libs',uselib_store='NTK')
  conf.check_cfg(package='jack',at_least_version='0.118',args='--cflags --libs',uselib_store='JACK')
  #conf.check_cfg(package='lilv-0',at_least_version='1.0',args='--cflags --libs',uselib_store='LILV')
  conf.check_cfg(package='sndfile',at_least_version='1.0',args='--cflags --libs',uselib_store='SNDFILE')

def build(bld):
  
  sources = ['src/gui.cxx',
             'src/main.cxx',
             'src/jack.cxx',
             'src/gtrack.cxx',
             'src/looper.cxx',
             'src/controller/apc.cxx',
             'src/eventhandlergui.cxx',
             'src/eventhandlerdsp.cxx']
  
  bld.program(source = sources, target='luppp5', use='JACK NTK SNDFILE')

