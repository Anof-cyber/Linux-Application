
#include <iostream>
#include <dlfcn.h>

int main() {
    void* handle = dlopen("user.so", RTLD_LAZY);
    if (!handle) {
        std::cerr << "Unable to load the user.so library: " << dlerror() << std::endl;
        return 1;
    }

    

    dlclose(handle);
    return 0;
}
