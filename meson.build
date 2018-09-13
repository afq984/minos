project(
    'minos', 'c',
    default_options: ['c_std=gnu99']
)

glib = dependency('glib-2.0')
zmq = dependency('libzmq')

systemd_unit_dir = join_paths(get_option('prefix'), 'lib', 'systemd', 'system')

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

shared_library(
    'nss_minos',
    'nss.c',
    version: '2',
    install: true
)

install_data(
    'systemd/minos-server.service', 'systemd/minos-client.service',
    install_dir: systemd_unit_dir
)

install_data(
    'server.conf', 'client.conf',
    install_dir: '/etc/minos.d'
)
