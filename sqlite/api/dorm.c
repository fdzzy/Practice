#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sqlite3.h>
#include <stdarg.h>
#include <string.h>
#include "dorm.h"

/* global database handle */
sqlite3 *db;

char cmd_prompt = '>';

//int LOG_LEVEL = LOG_DBG;
int LOG_LEVEL = LOG_ERR;

const char* SUBMITTER[] = {
    "unknown",
    "zzy",
    "wwb",
    "other"
};

const char* LOG_PROMPT[] = {
    "DORM LOG CRITICAL: ",
    "DORM LOG ERROR:    ",
    "DORM LOG WARNING:  ",
    "DORM LOG INFO:     ",
    "DORM LOG DEBUG:    "
};

int dorm_log(const int level, const char* fmt, ...)
{
    va_list args;
    int i;
    char buf[LOG_BUF_LEN];    
    
    if(level > LOG_LEVEL)
        return 0;
    
    va_start(args, fmt);
    i=vsprintf(buf, fmt, args);
    va_end(args);
    printf("%s%s", LOG_PROMPT[level], buf); 

    return i;
}

bool file_exist(const char* file_name)
{
    if(NULL == file_name)
        return false;

    if(0 == access(file_name, F_OK)) {
        dorm_log(LOG_DBG, "%s exists!\n", file_name);
        return true;
    } else {
        dorm_log(LOG_DBG, "%s does not exist!\n", file_name);
        return false;
    }
}

/* Here are some sqlite functions with error handling */
void sqlite3_open_err(const char* filename, sqlite3 **ppDb)
{
    int rc;

    rc = sqlite3_open(filename, ppDb);
    if(rc) {
        dorm_log(LOG_CRIT, "Can't open database file %s: %s\n", filename, sqlite3_errmsg(*ppDb));
        sqlite3_close(*ppDb);
        exit(1);
    } else {
        dorm_log(LOG_INFO, "database %s opened successfully!\n", filename);
    }

}

void sqlite3_exec_err(sqlite3* pDb, const char* sql)
{
    int rc;
    char* errMsg;

    dorm_log(LOG_DBG, "exec SQL: %s\n", sql);
    rc = sqlite3_exec(pDb, sql, NULL, NULL, &errMsg);
    if(rc != SQLITE_OK) {
        if(errMsg != NULL) {
            dorm_log(LOG_ERR, "SQL error: %s\n", errMsg);
            sqlite3_free(errMsg);
        }
        exit(1);
    }
}

/* Creating Tables, Views here */
void db_init()
{
    char* sql;

    if(!db) {
        dorm_log(LOG_CRIT, "init database with NULL db handle!");
        exit(1);
    }

    sql = "create table dorm (id INTEGER primary key, amount REAL NOT NULL, date INTEGER DEFAULT (strftime('\%s','now')), descr TEXT DEFAULT NULL, type INTEGER DEFAULT 0, submitter INTEGER DEFAULT 0);";
    sqlite3_exec_err(db, sql);

    sql = "create view balance as select SUM(amount) as balance from dorm;";
    sqlite3_exec_err(db, sql);
     
    sql = "create view query as select id, amount, datetime(date, 'unixepoch', 'localtime') as date, descr, type, submitter from dorm;";
    sqlite3_exec_err(db, sql);
}

void db_open()
{
    bool initialized;
    
    initialized = file_exist("dorm.db"); 

    sqlite3_open_err(DB_NAME, &db);
    /* NOTE: init should be checked after opening the db handle */
    if(!initialized) {
        dorm_log(LOG_INFO, "creating tables for database %s\n", DB_NAME);
        db_init();
    }
}

void db_close()
{
    sqlite3_close(db);
    dorm_log(LOG_INFO, "database %s closed\n", DB_NAME);
}

bool is_space(const char ch)
{
    if( (ch == ' ') ||
        (ch == '\t') ||
        (ch == '\n') )
        return true;
    else
        return false;
}

/* get token and move the pstr forword */
char* get_token(char** ppstr)
{
    static char token[BUF_LEN];
    char* pstr; 
    int i=0;

    if(ppstr == NULL) return NULL;

    pstr = *ppstr;
    /* skip leading space */
    while( pstr && is_space(*pstr) ) pstr++; 
    
    if( pstr == NULL) return NULL;
    
    if( *pstr == '"' ) {
        token[i++] = *pstr++;
        while( (*pstr != '"') && (i<(BUF_LEN-2)) ) {
            token[i++] = *pstr++;
        }
        token[i++] = *pstr++;
        token[i] = '\0'; 
    } else if ( *pstr == '\'' ) {
        token[i++] = *pstr++;
        while( (*pstr != '\'') && (i<(BUF_LEN-2)) ) {
            token[i++] = *pstr++;
        }
        token[i++] = *pstr++;
        token[i] = '\0'; 
    } else {
        /* strncpy */ 
        while( !is_space(*pstr) && (i<(BUF_LEN-1)) ) {
            token[i++] = *pstr++;
        }
        token[i] = '\0';
    }

    *ppstr = pstr;
    return token; 
}

void print_help()
{
    printf("\ncommands:\n");
    printf("insert (amount) ('description') [submitter] [time]\t- insert record\n");
    printf("       submitter value: 0 is unknown [default], 1 is zzy, 2 is wwb, 3 is other\n");
    printf("       time format: 'YYYY-MM-DD HH:MM:SS'\n");
    printf("update (id) (amount) ('descr') [submitter]\t- update record\n");
    printf("delete (id)\t\t\t- delete the record with its id specified\n");
    printf("list/ls [all]\t\t\t- list all records\n");
    printf("list/ls last (number of days)\t- list records of last number of days\n");
    printf("balance\t\t\t\t- show balance\n");
    printf("exit\t\t\t\t- exit\n");
    printf("quit\t\t\t\t- the same as 'exit'\n");
    printf("\n");
}

/* id is ignored if insert is true */
static void __insert_update(double amount, char *descr, bool insert, int submitter, char *time, int id)
{
    char sql[BUF_LEN];
    char *errMsg;
    int rc;
    int type = (amount < 0) ? 0 : 1;

    if(insert) {
        if((time == NULL) || !time[0]) {
            snprintf(sql, BUF_LEN, "insert into dorm (amount, descr, type, submitter) values(%lf, %s, %d, %d)", amount, descr, type, submitter); 
        } else {
            snprintf(sql, BUF_LEN, "insert into dorm (amount, descr, type, submitter, date) values(%lf, %s, %d, %d, (strftime('%%s', %s)))", amount, descr, type, submitter, time); 
        }
    } else {
        if(submitter < 0) {
            snprintf(sql, BUF_LEN, "update dorm set amount=%lf, descr=%s, type=%d where id=%d", amount, descr, type, id); 
        } else {
            snprintf(sql, BUF_LEN, "update dorm set amount=%lf, descr=%s, type=%d, submitter=%d where id=%d", amount, descr, type, submitter, id); 
        }
    }

    dorm_log(LOG_DBG, "exec SQL: %s\n", sql);
    rc = sqlite3_exec(db, sql, NULL, NULL, &errMsg);
    if(rc != SQLITE_OK) {
        if(errMsg != NULL) {
            dorm_log(LOG_ERR, "SQL error: %s\n", errMsg);
            sqlite3_free(errMsg);
        }
    }
    
}

/* TODO: verify if id exists in database */
static bool __verify_id(int id)
{
    if(id <= 0) return false; 

    return true;
}

void insert(char* line)
{
    double amount;
    char descr[BUF_LEN];
    char time[BUF_LEN];
    int submitter;
    char *token; 

    get_token(&line);   // This is 'insert'
    token = get_token(&line);   // This is 'amount'
    if(!token) {
        printf("insert syntax error!\n");
        print_help();
        return;
    }
    sscanf(token, "%lf", &amount);
    token = get_token(&line);   // This is 'descr'
    if(!token) {
        printf("insert syntax error!\n");
        print_help();
        return;
    }
    strncpy(descr, token, BUF_LEN);
    token = get_token(&line);   // This is 'submitter'
    if(!token || !token[0]) {
        submitter = 0;
        goto out;
    } else {
        sscanf(token, "%d", &submitter);
    }
    token = get_token(&line);   // This is 'time'
    if(!token || !token[0]) {
        time[0] = '\0';
    } else {
        strncpy(time, token, BUF_LEN);
    }
    
out:
    /* id is ignored if insert is true */
    __insert_update(amount, descr, true, submitter, time, 0);
}

void update(char* line)
{
    double amount;
    int id, rc, submitter;
    char descr[BUF_LEN];
    char *token; 

    get_token(&line);   // This is 'update'
    token = get_token(&line);   // This is 'id'
    if(!token) {
        printf("update syntax error!\n");
        print_help();
        return;
    }
    rc = sscanf(token, "%d", &id);
    if( rc <= 0 ) {
        printf("update syntax error!\n");
        print_help();
        return;
    } else if (!__verify_id(id)) {
        printf("id does not exist in database!\n");
        return;
    }
    token = get_token(&line);   // This is 'amount'
    if(!token) {
        printf("update syntax error!\n");
        print_help();
        return;
    }
    sscanf(token, "%lf", &amount);
    token = get_token(&line);   // This is 'descr'
    if(!token) {
        printf("update syntax error!\n");
        print_help();
        return;
    }
    strncpy(descr, token, BUF_LEN);
    token = get_token(&line);   // This is 'submitter'
    if(!token || !token[0]) {
        submitter = -1;
    } else {
        sscanf(token, "%d", &submitter);
    }
    
    __insert_update(amount, descr, false, submitter, NULL, id);
}

void delete(char* line)
{
    char *token, *errMsg;
    char sql[BUF_LEN];
    int id, rc;

    get_token(&line); // This is 'delete'
    token = get_token(&line);
    if(!token) {
        printf("delete syntax error!\n");
        print_help();
    } else {
        rc = sscanf(token, "%d", &id); 
        if( rc <= 0  ) {
            printf("delete syntax error!\n");
            print_help();
            return;
        } else if (!__verify_id(id)) {
            printf("id does not exist in database!\n");
            return;
        }
        snprintf(sql, BUF_LEN, "delete from dorm where id=%d", id);

        dorm_log(LOG_DBG, "exec SQL: %s\n", sql);
        rc = sqlite3_exec(db, sql, NULL, NULL, &errMsg);
        if(rc != SQLITE_OK) {
            if(errMsg != NULL) {
                dorm_log(LOG_ERR, "SQL error: %s\n", errMsg);
                sqlite3_free(errMsg);
            }
        }
    }
}

static void __list(int days)
{
    int rc, ncols, i, id, type, submitter;
    double amount;
    sqlite3_stmt *stmt;
    char *sql;
    const char *date, *descr;
    char buf[BUF_LEN];

    if(days <= 0) {
        sql = "select * from query;";
    } else {
        snprintf(buf, BUF_LEN, "select id, amount, datetime(date, 'unixepoch', 'localtime') as date, descr, type, submitter from dorm where date>=strftime('%%s', 'now', '-%d days');", days);
        sql = buf;
    }

    dorm_log(LOG_DBG, "step SQL: %s\n", sql);

    rc = sqlite3_prepare_v2(db, sql, strlen(sql), &stmt, NULL);
    if(rc != SQLITE_OK) {
        dorm_log(LOG_ERR, "SQL error: %s\n", sqlite3_errmsg(db));
        return;
    }

    rc = sqlite3_step(stmt);
    ncols = sqlite3_column_count(stmt);

    /* Print colums information */
    for(i=0; i<ncols ; i++) {
        if(i) printf("|");
        printf("%s", sqlite3_column_name(stmt,i));
    }
    printf("\n");

    while(rc == SQLITE_ROW) {
        id = sqlite3_column_int(stmt, 0);
        amount = sqlite3_column_double(stmt, 1);
        date = sqlite3_column_text(stmt, 2);
        descr = sqlite3_column_text(stmt, 3);
        type = sqlite3_column_int(stmt, 4); 
        submitter = sqlite3_column_int(stmt, 5);
        if((submitter > 3) || (submitter < 0)) submitter = 0;
        printf("%d|%.2lf|%s|%s|%d|%s\n", id, amount, date, descr, type, SUBMITTER[submitter]);
        rc = sqlite3_step(stmt);
    }

    sqlite3_finalize(stmt);

    if(days <= 0)
        balance();
}

void list(char* line)
{
    char *token;
    int days;

    get_token(&line); // This is 'list'
    token = get_token(&line);
    if(!token) {
        __list(0);
    } else {
        if(strncmp(token, "all", 3) == 0) {
            __list(0);
        } else if(strncmp(token, "last", 4) == 0) {
            if( (token = get_token(&line)) ) {
                sscanf(token, "%d", &days);
                __list(days); 
            } else {
                printf("list syntax error!\n");
                print_help();
            }
        } else {
            __list(0);
        }
    }
}

void balance()
{
    double bal;
    int rc;
    sqlite3_stmt *stmt;
    char *sql  = "select * from balance;";

    dorm_log(LOG_DBG, "step SQL: %s\n", sql);

    rc = sqlite3_prepare_v2(db, sql, strlen(sql), &stmt, NULL);
    if(rc != SQLITE_OK) {
        dorm_log(LOG_ERR, "SQL error: %s\n", sqlite3_errmsg(db));
        return;
    }
    rc = sqlite3_step(stmt);
    bal = sqlite3_column_double(stmt, 0);
    sqlite3_finalize(stmt);
    printf("Balance: %.2lf\n", bal);
    if( bal < 0 ) {
        printf("ALERT: Please deposit!\n");
    }
}

void cmd(char* line)
{
    dorm_log(LOG_DBG, "CMD: %s", line);

    if(line[0] == 'h') {
        print_help();
    } else if( (strncmp(line, "lis", 3) == 0) ||
               (strncmp(line, "ls", 2) ==0) ){
        list(line);
    } else if(strncmp(line, "ins", 3) == 0) {
        insert(line);
    } else if(strncmp(line, "upd", 3) == 0) {
        update(line);
    } else if(strncmp(line, "del", 3) == 0) {
        delete(line);
    } else if(strncmp(line, "bal", 3) == 0) {
        balance();
    } else {
        printf("Error command: %s", line);
        printf("Use 'help' to show how to use this command line.\n\n");
    }
}

int main(int argc, char* argv[])
{
    char line[BUF_LEN];

    db_open();    

    printf("\nWelcome to use the dorm comand line tools.\nUse 'help' to show how to use this command line.\n");
    while(1) {
        printf("%c ", cmd_prompt);
        fgets(line, BUF_LEN, stdin);
        if( (strncmp(line, "exit", 4) == 0) || 
            (line[0] == 'q') )
            break;
        cmd(line); 
    } 

    db_close();
    
    return 0;
}
