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

Note: if compiling/installing manually, make sure that `libnss_minos.so.2` gets put into the right location on your distribution.
It should live beside existing nss modules:

```
$ ls /usr/lib/libnss*.so.2
/usr/lib/libnss_compat.so.2  /usr/lib/libnss_hesiod.so.2      /usr/lib/libnss_resolve.so.2
/usr/lib/libnss_db.so.2      /usr/lib/libnss_minos.so.2       /usr/lib/libnss_systemd.so.2
/usr/lib/libnss_dns.so.2     /usr/lib/libnss_myhostname.so.2
/usr/lib/libnss_files.so.2   /usr/lib/libnss_mymachines.so.2
```


## Troubleshooting

If minos does not work, check the following:

1.  Check that the data is transferred to the client machines.
    On the client machine, see if the files `/etc/passwd.minos`, `/etc/group.minos`, `/etc/shadow.minos` exist.
    They should have the user, group, shadow entries you configured to synchronize.
    
2.  Check nss is working properly on client machines.
    On the client machine, run getent:
    
    ```
    getent -s minos passwd
    getent -s minos group
    sudo getent -s minos shadow
    ```

    They should output the same stuff as `/etc/{passwd,group,shadow}.minos` if `/etc/nsswitch.conf` is properly configured.
