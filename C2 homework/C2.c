#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sqlite3.h>
#include "input.c"

struct department{
    char *id;
    char *name;
    char *class;
};

struct club{
    char *id;
    char *name;
};

struct course{
    char *id;
    char *name;
};
/*
   当前使用者的账号信息
*/
struct account{
    char *id;
    char *name;
    char *password;
    struct department departments[3][100];
    struct course courses[3][100];
    struct club clubs[3][100];
};
struct account user;
int num[3][3];

sqlite3 *db;

const char *column_name[3] = { "学号", "姓名", "密码" };
#include "login.c"
#include "refresh.c"
#include "account.c"

int main( int argc, char *argv[] ){
    int err = 0;
    char *errmsg;
    
    err = sqlite3_open( "C2.db", &db );
    if( err!=0 ){
      fprintf( stderr, "无法打开数据库: %s\n", sqlite3_errmsg(db) );
      sqlite3_close(db);
      return(1);
    }
    else{
      fprintf(stdout,"成功打开数据库\n");
    }
    err = sqlite3_exec( db, "PRAGMA foreign_keys=true;", NULL, NULL, &errmsg );
    exec_report( err, errmsg, "无法设置外键", "成功设置外键", 1, 0 );
    while( user.id == NULL ){
        char a = 2;
        while( a!='e' && a!='0' && a!='1' ){
            printf( "\n选择登录或注册(0: 登录, 1: 注册, e: 退出程序): " );
            a = getchar();
            clear_input_buffer();
        }
        switch(a){
            case '0':{
                err = login();
                break;
            }
            case '1':{
                err = regist();
                break;
            }
            case 'e':{
                user.id = "exit";
                return 0;
            }
        }
    }
    if( err ){
        return 1;
    }
    refresh();
/*
    while(1) {
        char a = 2;
        while(a != '0' && a != '1' && a != 'e') {
            printf("\n选择功能(0: 发信息, 1: 查看收件箱, e: 退出程序): ");
            a = getchar();
            clear_input_buffer();
        }
        switch(a) {
            case '0':{
                err = Send(); break;
            }
            case '1':{
                err = Receive(); break;
            }
            case 'e':{
                id = "exit";
                return 0;
            }
        }

    }
*/
    return 0;
}