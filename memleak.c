#include <pwd.h>
int main() {
    for (int i = 0; i < 1000; ++ i) getpwuid(1000);
    for (int i = 0; i < 1000; ++ i) getpwuid(2000);
}
