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

#ifdef DEBUG
#define CONFIG_CODEGEN_PATH "/home/mateus/pollen-lang/plugins/codegen/llvm.so"
#endif

#ifndef PREFIX
#define PREFIX "/usr"
#endif

#define POLLEN_CONFIG_PATH                         \
    PREFIX "/etc/pollen.conf"

#if defined PREFIX && 0
#define POLLEN_LIBRARY_SEARCH_PATH                 \
    PREFIX "/lib"
#else
#define POLLEN_LIBRARY_SEARCH_PATH "../runtime/"
#endif

#define POLLENRT0_PATH "../runtime/pollenrt0.o"

#define USE_SYSTEM_LINKER 0

#endif
