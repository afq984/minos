project(
    'minos', 'c',
    default_options: ['c_std=gnu99']
)

glib = dependency('glib-2.0')
zmq = dependency('libzmq')

executable(
    'minos-server',
    'server.c', 'common.c',
    dependencies: [glib, zmq],
    install: true
)

executable(
    'minos-client',
    'client.c', 'common.c', 'atomic_update_file.c',
    dependencies: [glib, zmq],
    install: true
)
