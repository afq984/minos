# Maintainer: Yu, Li-Yu <https://github.com/afq984>

pkgname=minos-git
pkgver=0.0.10.b0bf9e6
pkgrel=1
pkgdesc='daemon and NSS module to provide synchronized user/group/shadow databases'
url='https://github.com/afq984/minos'
arch=('x86_64')
license=('BSD')
source=('git+https://github.com/afq984/minos')
sha256sums=('SKIP')
conflits=('minos')
provides=('minos')
backup=('etc/minos.d/server.conf' 'etc/minos.d/client.conf')
depends=('glib2' 'zeromq')
makedepends=('git' 'meson' 'ninja')

pkgver() {
    cd minos
    printf "0.0.%s.%s" "$(git rev-list --count HEAD)" "$(git rev-parse --short HEAD)"
}

build() {
	arch-meson minos build
	ninja -C build
}

check() {
	ninja -C build test
}

package() {
	DESTDIR="$pkgdir" ninja -C build install
	install -Dm644 minos/LICENSE "${pkgdir}/usr/share/licenses/${pkgname}/LICENSE"
}
