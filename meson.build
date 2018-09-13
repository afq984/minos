project(
    'minos', 'c',
    default_options: ['c_std=gnu99']
)

glib = dependency('glib-2.0')
zmq = dependency('libzmq')

executable(
    'minos-server',
    'common.c', 'server.c',
    dependencies: [glib, zmq],
    install: true
)
