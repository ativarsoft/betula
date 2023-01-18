/* Copyright (C) 2023 Mateus de Lima Oliveira */

#ifndef CONFIG_H
#define CONFIG_H

#ifdef TERMUX
#define TERMUX_ROOT_DIR "/data/data/com.termux/files/"
#define HAS_REGEX
#else
#define USE_STORAGE
#define USE_VIRTUALIZATION
#define HAS_REGEX
#endif

#endif
