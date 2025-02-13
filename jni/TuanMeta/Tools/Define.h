#pragma once

#define Method(image, namespaze, clazz, name, args) \
GetMethodOffset(image, namespaze, clazz, name, args)
#define Field(image, namespaze, clazz, name) \
GetFieldOffset(image, namespaze, clazz, name)
#define TuanMetaHook(image, namespaze, clazz, name, args, hook, orig) \
Tools::Hook((void *)(uintptr_t)Method(image, namespaze, clazz, name, args), (void *)hook, (void **)&orig)