#pragma once

#ifndef REMOVE_COPY(ClassName)
#define REMOVE_COPY(ClassName) \
ClassName(const ClassName&) = delete; \
ClassName& operator=(const ClassName&) = delete;
#endif // !REMOVE_COPY(ClassName)

#ifndef REMOVE_COPY_AND_MOVE(ClassName)
#define REMOVE_COPY_AND_MOVE(ClassName) \
ClassName(const ClassName&) = delete; \
ClassName& operator=(const ClassName&) = delete; \
ClassName(ClassName&&) = delete; \
ClassName& operator=(ClassName&&) = delete;
#endif // !REMOVE_COPY_AND_MOVE(ClassName)