#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "sqlite3.h"
#include "input.c"
#include "login.c"
#include "new_send.c"
#include "notice.c"
#include "C2.c"
#include "refresh.c"

int Len = 0 , cnt = 0;
static int callback(void *data, int argc, char **argv, char **azColName) {
   int i;
// fprintf(stdout , "%s", (const char *)data);
   for(i = 0; i < argc; i++){
      printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
   }
   printf("\n");
   return 0;
}

static int callback_id(void *data , int argc , char **argv, char **azColName) {
    //do sth.
    int *p; p = (int*) data;
	p = atoi(argv[0]);
    return 0;   
}

static int callback_check(void *data , int argc , char **argv , char **azColName) {
    int *fi; fi = (int *)data;
    fi[++cnt] = atoi(argv[0]);
    return 0;
}

inline int is_not_in(int id , int *a) {
    for(int i = 1;i <= cnt;i++) {
        if(a[i] == id) return 1;
    }
    return 0;
}

inline int Choose_to_Withdraw(sqlite3 *db) {
    char *ID = NULL , *nw1 = NULL, *nw2 = NULL , *nw3 = NULL , *Err1 = NULL, *Err2 = NULL , *Err3 = NULL;
    int id = atoi(ID); Len = cnt = 0;
    const char *op1 = "select max(rowid) from %q_announce";
    char *sql1 = sqlite3_mprintf(op1 , user.id);
    nw1 = sqlite3_exec(db , sql1 , callback_id, &Len , &Err1);
    int *a = (int *)malloc(Len * sizeof(int));
    const char *op2 = "select * from %q_announce";
    int tot = 10;
    for(;tot > 0;tot--) {
        input_text("请输入你要撤回的信息的ID(输入e退出)" , 5 , ID);
        if(ID[0] == 'e') {
            free(a);
            return 0;
        }
        char *sql2 = sqlite3_mprintf(op2 , ID);
        nw2 = sqlite3_exec(db , sql2 , callback_check , a , &Err2);
        int id = atoi(ID);
        if(is_not_in(id , a) != 0) {
            fprintf(stderr, "您不能撤回这条消息\n");
            continue;
        }
        const char *op3 = "delete from NOTICES where rowid = %q";
        char *sql3 = sqlite3_mprintf(op3 , id);
        nw3 = sqlite3_exec(db , sql3 , callback , 0 , &Err3);
        if(nw3 != SQLITE_OK) {
            fprintf(stderr, "无法选择\n");
            free(a);
            sqlite3_free(Err3);
            return -1; 
        }
        else {
            fprintf(stdout , "选择成功\n");
            free(a);
            return 0;
        }
    }
    fprintf(stderr , "连接超时\n");
    return -1;
}

inline int print(sqlite3 *db) {
    fprintf(stdout , "已发送信息列表\n");
    return select_notices(1);
}

inline int Withdraw(sqlite3 *db) {
    int num = 10;
    for(;num > 0;num--) {
        refresh();
        if(print(db) == 1) continue;
        if(Choose_to_Withdraw(db) == 1) return 0;
    }
    fprintf(stderr , "连接超时\n");
    return -1;
}