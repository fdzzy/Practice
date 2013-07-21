#ifndef DORM_H
#define DORM_H

#define DB_NAME "dorm.db"
typedef int bool;
#define false 0
#define true 1

enum {
    LOG_CRIT = 0,
    LOG_ERR,
    LOG_WARN,
    LOG_INFO,
    LOG_DBG
};

#define LOG_BUF_LEN 1024
#define BUF_LEN 1024

#endif
