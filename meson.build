project( 'openav_luppp', ['c','cpp'],
  default_options : [
    'cpp_std=c++11',
    ])

conf_data = configuration_data()
conf_data.set('version', '1.3.0-rc0')


if(get_option('tests') == true)
  add_project_arguments('-DBUILD_TESTS', language : 'cpp')
endif

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
executable('luppp', luppp_src + [version_hxx],
    install: true,
    dependencies: deps)

install_data('resources/metadata/luppp.desktop', install_dir: 'share/applications')
install_data('resources/metadata/luppp.appdata.xml', install_dir: 'share/appdata')
install_data('resources/icons/luppp.png', install_dir: 'share/pixmaps')
