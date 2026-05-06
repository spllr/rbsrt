require 'mkmf'

extension_name = 'rbsrt/rbsrt'

LIBDIR     = CONFIG['libdir']
INCLUDEDIR = CONFIG['includedir']

HEADER_DIRS = []
HEADER_DIRS << '/opt/local/include'    if File.directory?('/opt/local/include')
HEADER_DIRS << '/usr/local/include'    if File.directory?('/usr/local/include')
HEADER_DIRS << '/opt/homebrew/include' if File.directory?('/opt/homebrew/include')
HEADER_DIRS << INCLUDEDIR
HEADER_DIRS << '/usr/include'          if File.directory?('/usr/include')

LIB_DIRS = []
LIB_DIRS << '/opt/local/lib'    if File.directory?('/opt/local/lib')
LIB_DIRS << '/usr/local/lib'    if File.directory?('/usr/local/lib')
LIB_DIRS << '/opt/homebrew/lib' if File.directory?('/opt/homebrew/lib')
LIB_DIRS << LIBDIR
LIB_DIRS << '/usr/lib'          if File.directory?('/usr/lib')

if lib_srt_home_path = ENV["RBSRT_LIBSRT_HOME"]
  HEADER_DIRS << File.join(lib_srt_home_path, "include") if File.directory?(File.join(lib_srt_home_path, "include"))
  LIB_DIRS    << File.join(lib_srt_home_path, "lib")     if File.directory?(File.join(lib_srt_home_path, "lib"))
end

if lib_srt_lib_path = ENV["RBSRT_LIBSRT_LIB_DIR"]
  LIB_DIRS << lib_srt_lib_path if File.directory?(lib_srt_lib_path)
end

if lib_srt_include_path = ENV["RBSRT_LIBSRT_INCLUDE_DIR"]
  HEADER_DIRS << lib_srt_include_path if File.directory?(lib_srt_include_path)
end

# Try pkg-config first — it resolves transitive dependencies (OpenSSL, pthreads)
# automatically and works reliably on all platforms.
unless pkg_config('srt')
  # Manual fallback: pass the search-path arrays directly to find_header /
  # find_library so the paths built above are actually used.
  dir_config('srt')

  unless find_header('srt/srt.h', *HEADER_DIRS)
    abort "libsrt header not found. Please install libsrt: https://github.com/Haivision/srt"
  end

  # libsrt depends on OpenSSL; ensure those are linked before libsrt.
  have_library('ssl')
  have_library('crypto')

  unless find_library('srt', 'srt_create_socket', *LIB_DIRS)
    abort "libsrt not found. Please install libsrt: https://github.com/Haivision/srt"
  end
end

dir_config(extension_name)

create_makefile(extension_name)
