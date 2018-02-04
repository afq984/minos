project(
    'minos', 'c', 'cpp',
    default_options: ['c_std=c99', 'cpp_std=c++11']
)

rpcgen = find_program('rpcgen')
rpcwrap = find_program('tools/rpcwrap.py')
rpcwrap_command = [
    rpcwrap,
    '--rpcgen=' + rpcgen.path(),
    '--infile=@INPUT@',
    '--outdir=@OUTDIR@'
]
minos_h = custom_target(
    'minos.h',
    command: rpcwrap_command + ['--mode=header'],
    input: 'src/minos.x',
    output: ['@BASENAME@.h']
)
minos_c = custom_target(
    'minos.x.c',
    command: rpcwrap_command + ['--mode=source'],
    input: 'src/minos.x',
    output: ['@BASENAME@_xdr.c', '@BASENAME@_svc.c', '@BASENAME@_clnt.c']
)
libminos_rpc = static_library(
    'minos_rpc',
    minos_h, minos_c,
    pic: true,
    c_args: ['-Wno-unused-variable', '-Wno-unused-parameter']
)

executable(
    'minosd',
    'src/minos_server.cc', 'src/serialize.cc', minos_h,
    link_with: libminos_rpc,
    install: true
)

shared_library(
    'nss_minos',
    'src/minos_client.cc', 'src/serialize.cc', minos_h,
    link_with: libminos_rpc,
    version: '2',
    install: true
)

minos_preload = shared_library(
    'minos_preload',
    'src/preload.c'
)

memleak = executable(
    'memleak',
    'memleak.c'
)
# valgrid = find_program('valgrind')

#test(
#    'leakcheck',
#    valgrid,
#    args: ['--leak-check=full', '--error-exitcode=1', memleak.full_path()],
#    env: [
#        'LD_PRELOAD=' + minos_preload.full_path(),
#        'LD_LIBRARY_PATH=' + meson.build_root(),
#    ]
#)
