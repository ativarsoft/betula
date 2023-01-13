#ifndef CONFIG_H
#define CONFIG_H

#ifdef TERMUX
#define TERMUX_ROOT_DIR "/data/data/com.termux/files/"
#else
#define USE_STORAGE
#define USE_VIRTUALIZATION
#endif

#endif
