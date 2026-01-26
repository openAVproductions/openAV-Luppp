project( 'openav_luppp', ['c','cpp'],
  default_options : [
    'cpp_std=c++11',
    ])

conf_data = configuration_data()
conf_data.set('version', '1.3.0-rc0')


if(get_option('tests') == true)
  add_project_arguments('-DBUILD_TESTS', language : 'cpp')
endif

# Audio backend selection
audio_backend = get_option('audio_backend')
message('Selected audio backend: ' + audio_backend)

if audio_backend == 'jack'
  add_project_arguments('-DAUDIO_BACKEND_JACK', language : 'cpp')
elif audio_backend == 'rtaudio'
  add_project_arguments('-DAUDIO_BACKEND_RTAUDIO', language : 'cpp')
  
  # Platform-specific RtAudio configuration
  if host_machine.system() == 'darwin'
    add_project_arguments('-D__MACOSX_CORE__', language : 'cpp')
    message('RtAudio: Using CoreAudio backend for macOS')
  elif host_machine.system() == 'linux'
    add_project_arguments('-D__LINUX_ALSA__', language : 'cpp')
    message('RtAudio: Using ALSA backend for Linux')
  elif host_machine.system() == 'windows'
    add_project_arguments('-D__WINDOWS_WASAPI__', language : 'cpp')
    message('RtAudio: Using WASAPI backend for Windows')
  endif
endif

add_project_arguments('-Wno-unused-variable', language : 'cpp')
add_project_arguments('-Wno-reorder', language : 'cpp')
add_project_arguments('-Wno-sign-compare', language : 'cpp')

cc  = meson.get_compiler('c')
cpp = meson.get_compiler('cpp')

# Force ARM64 architecture on macOS (Apple Silicon)
if host_machine.system() == 'darwin'
  add_project_arguments('-arch', 'arm64', language: 'c')
  add_project_arguments('-arch', 'arm64', language: 'cpp')
  add_project_link_arguments('-arch', 'arm64', language: 'c')
  add_project_link_arguments('-arch', 'arm64', language: 'cpp')
  message('Forcing ARM64 architecture for Apple Silicon')
endif

luppp_src = []
subdir('src')

# macOS-specific FLTK detection using fltk-config
# Use find_program to locate fltk-config dynamically
fltk_config = find_program('fltk-config', required: true, dirs: ['/opt/homebrew/bin', '/usr/local/bin'])
fltk_result = run_command(fltk_config, '--cxxflags', check: true)
fltk_cflags_raw = fltk_result.stdout().strip().split()
fltk_result = run_command(fltk_config, '--ldflags', check: true)
fltk_ldflags_raw = fltk_result.stdout().strip().split()

# Filter out x86_64 architecture flags and ensure arm64 for Apple Silicon
fltk_cflags = []
foreach flag : fltk_cflags_raw
  if flag != '-arch' and flag != 'x86_64'
    fltk_cflags += flag
  endif
endforeach

fltk_ldflags = []
foreach flag : fltk_ldflags_raw
  if flag != '-arch' and flag != 'x86_64'
    fltk_ldflags += flag
  endif
endforeach

# Force arm64 architecture for Apple Silicon
fltk_cflags += ['-arch', 'arm64']
fltk_ldflags += ['-arch', 'arm64']

add_project_arguments(fltk_cflags, language : 'cpp')

# Dependencies
dep_names = [
  'cairo',
  'liblo',
  'sndfile',
  'samplerate',
  'jack',  # Always needed for jack_ringbuffer utility functions
  ]

deps = []

foreach dep : dep_names
  deps += dependency(dep)
endforeach

# RtAudio-specific platform dependencies
if audio_backend == 'rtaudio'
  if host_machine.system() == 'darwin'
    # macOS: Add CoreAudio, CoreMIDI and CoreFoundation frameworks
    deps += dependency('appleframeworks', modules: ['CoreAudio', 'CoreMIDI', 'CoreFoundation'])
  elif host_machine.system() == 'linux'
    # Linux: Add ALSA
    deps += dependency('alsa')
    deps += dependency('threads')
  elif host_machine.system() == 'windows'
    # Windows: WASAPI is part of Windows SDK, no extra deps needed
  endif
else
  # JACK backend also needs jack ringbuffer even though we already added jack dependency
  # This ensures all JACK symbols are available
endif

# Add FLTK as a declared dependency
fltk_dep = declare_dependency(
  compile_args: fltk_cflags,
  link_args: fltk_ldflags
)
deps += fltk_dep

# compile the main project
executable('luppp', luppp_src + [version_hxx],
    install: true,
    dependencies: deps)

install_data('resources/metadata/luppp.desktop', install_dir: 'share/applications')
install_data('resources/metadata/luppp.appdata.xml', install_dir: 'share/appdata')
install_data('resources/icons/luppp.png', install_dir: 'share/pixmaps')
