#include "App.h"

int main(int argc, char* argv[]) {
    App blink3D;
    
    if (!blink3D.init()) {
        return -1;
    }
    
    blink3D.run();
    
    return 0;
}

