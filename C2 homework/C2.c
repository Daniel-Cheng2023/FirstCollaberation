#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sqlite3.h>
#include "input.c"

/*
   当前使用者的账号信息
*/
struct account{
    char *id;
    char *name;
    char *password;
    char *major_school;
    char *major_department;
    char *minor_school;
    char *minor_department;
    char *identity;
    char **courses;
    char **clubs;
};
struct account user;

#define id user.id
#define name user.name
#define password user.password
#define major_school user.major_school
#define major_department user.major_department
#define minor_school user.minor_school
#define minor_department user.minor_department
#define identity user.identity

int flag = 0;
const char *column_name[8] = { "学号", "姓名", "密码", "主修学院", "主修系", "辅修学院", "辅修系", "身份" };
#include "login.c"
#include "notice.c"

int main( int argc, char *argv[] ){
    sqlite3 *db;
    int err = 0;
    char *errmsg;
    
    err = sqlite3_open( "C2.db", &db );
    if( err ){
      fprintf( stderr, "无法打开数据库: %s\n", sqlite3_errmsg(db) );
      sqlite3_close(db);
      return(1);
    }
    else{
      fprintf(stdout,"成功打开数据库\n");
    }
    err = sqlite3_exec( db, "PRAGMA foreign_keys=true;", NULL, NULL, &errmsg );
    exec_report( err, errmsg, "无法设置外键", "成功设置外键" );
    while( id == NULL ){
        int a = 2;
        while( a!='e' && a!='0' && a!='1' ){
            printf( "\n选择登录或注册(0: 登录, 1: 注册, e: 退出程序): " );
            a = getchar();
            clear_input_buffer();
        }
        switch(a){
            case '0':{
                err = login( db );
                break;
            }
            case '1':{
                err = regist( db );
                break;
            }
            case 'e':{
                id = "exit";
                return 0;
            }
        }
    }
    refresh( db );
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
