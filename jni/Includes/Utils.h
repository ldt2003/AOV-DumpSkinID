#ifndef UTILS
#define UTILS

#include <jni.h>
#include <unistd.h>
#include <cstdio>
#include <cstring>
#include <string>
#include <cstdlib>
#include "Logger.h"

typedef unsigned long DWORD;
static uintptr_t libBase;

bool libLoaded = false;

DWORD findLibrary(const char *library) {
    char filename[0xFF] = {0},
            buffer[1024] = {0};
    FILE *fp = NULL;
    DWORD address = 0;

    sprintf(filename, OBFUSCATE("/proc/self/maps"));

    fp = fopen(filename, OBFUSCATE("rt"));
    if (fp == NULL) {
        perror(OBFUSCATE("fopen"));
        goto done;
    }

    while (fgets(buffer, sizeof(buffer), fp)) {
        if (strstr(buffer, library)) {
            address = (DWORD) strtoul(buffer, NULL, 16);
            goto done;
        }
    }

    done:

    if (fp) {
        fclose(fp);
    }

    return address;
}

DWORD getAbsoluteAddress(const char *libraryName, DWORD relativeAddr) {
    libBase = findLibrary(libraryName);
    if (libBase == 0)
        return 0;
    return (reinterpret_cast<DWORD>(libBase + relativeAddr));
}


jboolean isGameLibLoaded(JNIEnv *env, jobject thiz) {
    return libLoaded;
}

bool isLibraryLoaded(const char *libraryName) {
    //libLoaded = true;
    char line[512] = {0};
    FILE *fp = fopen(OBFUSCATE("/proc/self/maps"), OBFUSCATE("rt"));
    if (fp != NULL) {
        while (fgets(line, sizeof(line), fp)) {
            std::string a = line;
            if (strstr(line, libraryName)) {
                libLoaded = true;
                return true;
            }
        }
        fclose(fp);
    }
    return false;
}

struct lib_info {
    void* start_address;
    void* end_address;
    intptr_t size;
    std::string name;
};

lib_info find_library(const char *module_name){
    lib_info library_info{};
    char line[512], mod_name[64];

    FILE *fp = fopen("/proc/self/maps", "rt");
    if (fp != NULL)
    {
        while (fgets(line, sizeof(line), fp))
        {
            if (strstr(line, module_name))
            {
                sscanf(line, "%llx-%llx %*s %*ld %*s %*d %s",
                       (long long unsigned *)&library_info.start_address,
                       (long long unsigned *)&library_info.end_address, mod_name);

                library_info.size = (uintptr_t)library_info.end_address - (uintptr_t)library_info.start_address;

                if (library_info.name.empty())
                    library_info.name = std::string(mod_name);

                break;
            }
        }
        fclose(fp);
    }
    return library_info;
}

uintptr_t find_pattern(uint8_t* start, const size_t length, const char* pattern) {
    const char* pat = pattern;
    uint8_t* first_match = 0;
    for (auto current_byte = start; current_byte < (start + length); ++current_byte) {
        if (*pat == '?' || *current_byte == strtoul(pat, NULL, 16)) {
            if (!first_match)
                first_match = current_byte;
            if (!pat[2])
                return (uintptr_t)first_match;
            pat += *(uint16_t*)pat == 16191 || *pat != '?' ? 3 : 2;
        }
        else if (first_match) {
            current_byte = first_match;
            pat = pattern;
            first_match = 0;
        }
    } return 0;
}

uintptr_t find_pattern_in_module(const char* lib_name, const char* pattern) {
    lib_info lib_info = find_library(lib_name);
    return find_pattern((uint8_t*)lib_info.start_address, lib_info.size, pattern);
}

const char *GetPackageName() {
    //https://stackoverflow.com/questions/42918762/how-to-get-app-package-name-or-applicationid-using-jni-android
    char *application_id[256];
    FILE *fp = fopen("proc/self/cmdline", "r");
    if (fp) {
        fread(application_id, sizeof(application_id), 1, fp);
        fclose(fp);
    }
    return (const char *) application_id;
}

#endif