project(
    'minos', 'cpp',
    default_options: ['cpp_std=c++11']
)

protoc = find_program('protoc')
pb_gen = generator(protoc, \
    output: ['@BASENAME@.pb.cc', '@BASENAME@.pb.h'],
    arguments: ['--proto_path=@SOURCE_DIR@/proto', '--cpp_out=@BUILD_DIR@', '@INPUT@']
)

grpc_cpp_plugin = find_program('grpc_cpp_plugin')
grpc_gen = generator(protoc, \
    output: ['@BASENAME@.grpc.pb.cc', '@BASENAME@.grpc.pb.h', '@BASENAME@_mock.grpc.pb.h'],
    arguments: ['--proto_path=@SOURCE_DIR@/proto', '--grpc_out=@BUILD_DIR@', '--plugin=protoc-gen-grpc='+grpc_cpp_plugin.path(), '@INPUT@']
)

cdata = configuration_data()
cdata.set('sysconfdir', get_option('sysconfdir'))
config_h = configure_file(
    input: 'src/config.h.in',
    output: 'config.h',
    configuration: cdata
)

deps = [
    dependency('protobuf'),
    dependency('grpc'),
    dependency('grpc++')
]
test_dep = dependency('gtest', main: true)

proto = pb_gen.process('proto/minos.proto')
grpc = grpc_gen.process('proto/minos.proto')

grpc_srcs = [proto, grpc]

includes = include_directories('src')

executable(
    'minosd',
    'src/minosd.cc', 'src/server.cc', 'src/serialize.cc', 'src/configuration.cc', grpc_srcs,
    dependencies: deps,
    install: true,
    include_directories: includes
)
shared_library(
    'nss_minos',
    'src/libnss_minos.cc', 'src/deserialize.cc', grpc_srcs,
    version: '2',
    dependencies: deps,
    install: true,
    include_directories: includes
)
install_data(
    'etc/minos.conf',
    install_dir: get_option('sysconfdir'),
    install_mode: 'rw-r--r--'
)

# debug utilities
executable(
    'client',
    'src/client.cc', grpc_srcs,
    dependencies: deps,
    include_directories: includes
)
shared_library(
    'minos_preload',
    'src/preload.cc'
)

# tests
test_libnss_minos = executable(
    'test_libnss_minos',
    'test/test_libnss_minos.cc', 'src/libnss_minos.cc', 'src/server.cc', 'src/serialize.cc', 'src/deserialize.cc', grpc_srcs,
    dependencies: [deps, test_dep],
    include_directories: includes
)
test('nss', test_libnss_minos)

test_minosd = executable(
    'test_minosd',
    'test/test_minosd.cc', 'src/server.cc', 'src/serialize.cc', grpc_srcs,
    dependencies: [deps, test_dep],
    include_directories: includes
)
test('minosd', test_minosd)

test_roundtrip = executable(
    'test_roundtrip',
    'test/test_roundtrip.cc', 'src/serialize.cc', 'src/deserialize.cc', proto,
    dependencies: [deps, test_dep],
    include_directories: includes
)
test('roundtrip', test_roundtrip)

test_configuration = executable(
    'test_configuration',
    'test/test_configuration.cc', 'src/configuration.cc', config_h,
    dependencies: [test_dep],
    include_directories: includes
)
test(
    'configuration',
    test_configuration,
    env: 'TEST_CONFIGURATION_FILE=@0@/test/test_minos.conf'.format(meson.current_source_dir()),
    include_directories: includes
)
