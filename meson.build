project(
    'minos', 'c',
    default_options: ['c_std=gnu99']
)

glib = dependency('glib-2.0')

executable(
    'minos-server',
    'common.c', 'server.c',
    dependencies: glib,
    install: true
)
