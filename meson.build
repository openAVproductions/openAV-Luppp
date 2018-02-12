project('openav_luppp', ['c','cpp'])

conf_data = configuration_data()
conf_data.set('version', '1.1.1')

#add_project_arguments('-std=c99', language : 'c')
#add_project_arguments('-std=c++11', language : 'cpp')
add_project_arguments('-Wno-unused-variable', language : 'cpp')
add_project_arguments('-Wno-reorder', language : 'cpp')
add_project_arguments('-Wno-sign-compare', language : 'cpp')

cc  = meson.get_compiler('c')
cpp = meson.get_compiler('cpp')

luppp_src = []
subdir('src')


dep_names = [
  'ntk',
  'cairo',
  'liblo',
  'jack',
  'sndfile',
  'samplerate',
  'x11'
  ]
deps = []

foreach dep : dep_names
  deps += dependency(dep)
endforeach

# compile the main project
executable('luppp', luppp_src,
    dependencies: deps)
