#include <nss.h>
#include <stdlib.h>

static void __attribute__((constructor)) nss_loader(void)
{
    __nss_configure_lookup("passwd", "minos");
    __nss_configure_lookup("group", "minos");
    __nss_configure_lookup("shadow", "minos");
}
