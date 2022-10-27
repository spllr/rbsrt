require 'mkmf'

extension_name = 'rbsrt/rbsrt'

# RbConfig::MAKEFILE_CONFIG['CC'] = ENV['CC'] if ENV['CC']

LIBDIR      = CONFIG['libdir']
INCLUDEDIR  = CONFIG['includedir']

HEADER_DIRS = []
HEADER_DIRS << '/opt/local/include' if File.directory?('/opt/local/include')
HEADER_DIRS << '/usr/local/include' if File.directory?('/usr/local/include')
HEADER_DIRS << '/opt/homebrew/include' if File.directory?('/opt/homebrew/include')
HEADER_DIRS << INCLUDEDIR
HEADER_DIRS << '/usr/include' if File.directory?('/usr/include')


LIB_DIRS = []
LIB_DIRS << '/opt/local/lib' if File.directory?('/opt/local/lib')
LIB_DIRS << '/usr/local/lib' if File.directory?('/usr/local/lib')
LIB_DIRS << '/opt/homebrew/lib' if File.directory?('/opt/homebrew/lib')
LIB_DIRS << LIBDIR
LIB_DIRS << '/usr/lib' if File.directory?('/usr/lib')

if lib_srt_home_path = ENV["RBSRT_LIBSRT_HOME"]
  HEADER_DIRS << File.join(lib_srt_home_path, "include") if File.directory?(File.join(lib_srt_home_path, "include"))
  LIB_DIRS << File.join(lib_srt_home_path, "lib") if File.directory?(File.join(lib_srt_home_path, "lib"))
end

if lib_srt_lib_path = ENV["RBSRT_LIBSRT_LIB_DIR"]
  LIB_DIRS << lib_srt_lib_path if File.directory?(lib_srt_lib_path)
end

if lib_srt_include_path = ENV["RBSRT_LIBSRT_INCLUDE_DIR"]
  LIB_DIRS << lib_srt_include_path if File.directory?(lib_srt_include_path)
end

dir_config('srt', HEADER_DIRS, LIB_DIRS)
# dir_config('srt', HEADER_DIRS, LIB_DIRS)
# dir_config('c++', HEADER_DIRS, LIB_DIRS)

# srt dependencies

unless find_header('srt/srt.h')
  abort "libsrt is missing no header.  please install libsrt: https://github.com/Haivision/srt"
end

unless find_library('srt', 'srt_create_socket')
  abort "libsrt is missing no sock.  please install libsrt: https://github.com/Haivision/srt"
end

dir_config(extension_name)

create_makefile(extension_name)
