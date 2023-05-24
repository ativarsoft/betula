/* Copyright (C) 2023 Mateus de Lima Oliveira */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pwd.h>
#include "yeast.h"

string_t get_home(void)
{
    string_t home_dir;
    struct passwd *pw = getpwuid(getuid());
    if (pw != NULL) {
        home_dir = pw->pw_dir;
    } else {
        fprintf(stderr, "Failed to get home directory.\n");
        return NULL;
    }
    return home_dir;
}

