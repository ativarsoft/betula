/* Copyright (C) 2023 Mateus de Lima Oliveira */

#include <pollen/pollen.h>

#if defined _WIN32
int tmpl_get_num_processors()
{
	return -1;
}
#elif defined __unix__
#include <unistd.h>

int tmpl_get_num_processors()
{
	return sysconf(_SC_NPROCESSORS_ONLN);
}
#else
#error Unsupported platform or operating system.
#endif
