#ifndef SNAKE_DEBUG_H
#define SNAKE_DEBUG_H

#define DEBUG

//int log(const char *fmt, ...)
//{
//    int n = 0;
//#ifdef DEBUG
//    char buf[1024];
//    va_list ap;
//    va_start(ap, fmt);
//    n = vsnprintf(buf, sizeof(buf), fmt, ap);
//    va_end(ap);
//#endif
//    return n;
//}

#ifdef DEBUG
#define LOG(format, args...) printf(format, ##args)
#else
#define LOG(format, args...)
#endif

#endif
