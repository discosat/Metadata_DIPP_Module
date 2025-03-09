project('example', 'c', 'cpp',subproject_dir: 'lib',default_options: [
	'c_std=gnu11', 
	'b_lto=false',
	'default_library=static',
	'csp:packet_padding_bytes=42', 
	'csp:buffer_count=1000',
	'csp:buffer_size=2048',
	'csp:conn_max=20',
	'csp:conn_rxqueue_len=1000',
	'csp:qfifo_len=1000',
	'csp:rdp_max_window=1000',
	'csp:port_max_bind=16',
	'csp:use_rtable=true',
	'param:have_fopen=true', 
	'param:list_dynamic=true',
	'param:vmem_fram=false', 
])
project_name = 'example'

# Compiler and linker flags
cflags = ['-Wall']

# Dependencies
proto_c_dep = meson.get_compiler('c').find_library('protobuf-c', required : false)
csp_dep = dependency('csp', fallback: ['csp', 'csp_dep'], static:true)
param_dep = dependency('param', fallback: ['param', 'param_dep'], static:true)

# Source files
sources = [
    'src/module_template.c', # module file (REMEMBER TO CHANGE!)
    'src/utils/memory_util.c',
    'src/utils/config_util.c',
    'src/utils/batch_util.c',
    'src/utils/error_util.c',
    'src/utils/metadata_util.c',
    'src/utils/metadata.pb-c.c',
    'src/param_config.c'
]

dirs = include_directories(
    'src/include', 
    'src/include/utils',
)


# Shared library (SO)
shared_library(project_name, sources,
    include_directories: dirs,
    c_args: cflags + ['-DSHARED_MEMORY=1'],
    dependencies: [proto_c_dep, csp_dep, param_dep]
)

# Check for cross-compiling
if not get_option('cross-compile')
    # If not cross-compiling, include the executable target
    sources += 'src/test.c'
    sources += 'src/utils/yaml_parser.c'

    libyaml_dep = dependency('yaml-0.1')
    m_dep = meson.get_compiler('c').find_library('m', required : false)

    # Executable
    executable(project_name + '-exec', sources,
        include_directories: dirs,
        c_args: cflags + ['-g', '-DSHARED_MEMORY=0'],
        dependencies: [libyaml_dep, m_dep, proto_c_dep, csp_dep, param_dep]
    )
endif