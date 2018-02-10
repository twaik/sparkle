#include "were_event_loop.h"
#include <unistd.h>

int main(int argc, char *argv[])
{
    WereEventLoop l1;

    l1.run();
    
    fprintf(stdout, "ok\n");

    
    return 0;
}

