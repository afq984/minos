# minos
Minos provides:

1. Daemons to make passwd/group/shadow in a server available to clients.
2. A NSS module to access the exported passwd/group/shadow files.

## Use scenario
* Authenticate users in one or multiple clients using the same user/password from a server
* Synchronize user/group/shadow information from a server to clients
* Have a cluster system but find NIS ypbind/ypserv or LDAP authentication too cumbersome to setup

## Usage

### Server setup

1.  Change the `Address` in `/etc/minos.d/server.conf` to your server's address in the local network

    Example:
    ```
    Address=tcp://10.0.0.1:6148
    ```

    There are many other options in `server.conf`, which allow you to specify which users and groups
    you would like to make available to the client machines.

2.  Start and enable `minos-server.service`

    Example:
    ```
    systemctl start minos-server.service
    systemctl enable minos-server.service
    ```

### Client setup

1.  Change the `Address` in `/etc/minos.d/client.conf` to your server's address in the local network
2.  Start and enable `minos-client.service`
3.  Edit `/etc/nsswitch.conf` and add `minos` to `passwd` `group` and `shadow`

    Example:

    ```
    passwd: files mymachines systemd minos
    group: files mymachines systemd minos
    shadow: files minos
    ```

### Changing passwords (passwd) / shells (chsh)

Password changes on the server take effect (broadcasts to the clients) immediately.

Password changes on the client should be blocked by the system.

### Other hints

You do not need to run `minos-client` and `minos-server` in the same node.

## Installation

* Arch Linux - [PKGBUILD](https://github.com/afg984/minos-git.PKGBUILD)
* CentOS - [rpm spec file](https://github.com/afg984/minos.spec)
