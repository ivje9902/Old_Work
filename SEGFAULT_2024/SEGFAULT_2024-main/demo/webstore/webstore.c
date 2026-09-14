#include <stdio.h>
#include <stdlib.h>

#include "libwebstore/ui.h"

int main(const int argc, char *argv[])
{
    if (argc > 2)
    {
        printf("Usage: %s number\n", argv[0]);
        exit(1);
    }

    start_ui();
}
