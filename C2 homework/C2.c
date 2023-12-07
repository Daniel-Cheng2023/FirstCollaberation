#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sqlite3.h>
#include "sql.c"

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
int regist(sqlite3*);
int callback_login(void*,int,char**,char**);
int login(sqlite3*);

int main( int argc, char *argv[] ){
    sqlite3 *db;
    int Err = 0;
    
    Err = sqlite3_open( "C2.db", &db );
    if( Err ){
      fprintf( stderr, "无法打开数据库: %s\n", sqlite3_errmsg(db) );
      sqlite3_close(db);
      return(1);
    }
    else{
      fprintf(stdout,"成功打开数据库\n");
    }
    while( id == NULL ){
        int a = 2;
        while( a!='e' && a!='0' && a!='1' ){
            printf( "\n选择登录或注册(0: 登录, 1: 注册, e: 退出程序): " );
            a = getchar();
            clear_input_buffer();
        }
        switch(a){
            case '0':{
                Err = login( db );
                break;
            }
            case '1':{
                Err = regist( db );
                break;
            }
            case 'e':{
                id = "exit";
                break;
            }
        }
    }
    return 0;
}

// 注册函数
/*
   向 db 的ACCOUNTS表中注册账号

   return:
   -1: 中间退出
   0: 成功注册
   1: statement 无法正常生成
   2: statement 执行出错
*/
int regist( sqlite3 *db ){
    int Err = 1;
    char *ErrMsg = NULL, *ID = NULL, *NAME = NULL, *passwd = NULL;

    Err = input_id(&ID);
    if( Err==-1 ){
        return -1;
    }
    Err = input_name(&NAME);
    if( Err==-1 ){
        Free(1,&ID);
        return -1;
    }
    Err = input_passwd(&passwd);
    if( Err==-1 ){
        Free(2,&ID,&NAME);
        return -1;
    }
    const char *insert = "insert into ACCOUNTS(ID,name,password) \
VALUES(%Q,%Q,%Q);";
    char *stmt;
    stmt = sqlite3_mprintf( insert, ID, NAME, passwd );
    Free( 3, &ID, &NAME, &passwd );
    if( stmt == NULL ){
        sqlite3_free( stmt );
        fprintf( stderr, "无法生成SQL语句!\n" );
        return 1;
    }
    Err = sqlite3_exec( db, stmt, NULL, NULL, &ErrMsg);
    if( Err ){
        fprintf( stderr, "SQL error: %s\n", ErrMsg );
        sqlite3_free(ErrMsg);
        return 2;
    }
    fprintf( stderr, "成功注册!\n");
    sqlite3_free(stmt);
    printf( "注册后需重新登录\n" );
    return 0;
}

// 登录函数
/*
   以 type 确定登录方式, 并将数据存入 user 

   type:
   0: 通过 ID 登录
   1: 通过 name 登录

   return:
   -1: 中途退出
   0: 成功登录
   1: ID number 不是10位
   2: password 小于6位
*/
int login( sqlite3 *db ){
    int Err = 1, type = 2;
    char *ErrMsg = NULL, *ID_name, *passwd;
    char *select;
    
    while( type!='e' && type!='0' && type!='1' ){
        printf("\n请输入登录方式(0: 学号登录, 1: 姓名登录, e: 返回上一级): ");
        type = getchar();
        clear_input_buffer();
    }
    switch(type){
        case '0':{
            Err = input_id( &ID_name );
            select = "select * from ACCOUNTS \
where (ID == %Q) and (password == %Q)";
            break;
        }
        case '1':{
            Err = input_name( &ID_name );
            select = "select * from ACCOUNTS where \
(name == %Q) and (password == %Q)";
            break;
        }
        case 'e':{
            return -1;
        }
    }
    if( Err==-1 ){
        return -1;
    }
    //input passwd
    Err = input_passwd( &passwd );
    if( Err== -1 ){
        Free( 1, &ID_name );
        return -1;
    }
    char *stmt;
    stmt = sqlite3_mprintf( select, ID_name, passwd );
    Free( 2, &ID_name, &passwd );
    if( stmt == NULL ){
        sqlite3_free( stmt );
        fprintf( stderr, "无法生成SQL语句!\n" );
        return 1;
    }
    Err = sqlite3_exec( db, stmt, callback_login, NULL, &ErrMsg);
    if( Err ){
        fprintf( stderr, "SQL error: %s\n", ErrMsg );
        sqlite3_free(ErrMsg);
        return 2;
    }
    if( id!=NULL ){
        printf( "成功登录!\n");
        char **p = &id;
        int i;
        for( i=0;i<8;i++ ){
            printf( "%s 为 %s\n", column_name[i], *p ? *p : "NULL" );
            p++;
        }
    }
    else if( flag==0 ){
        printf( "未查找到该账号\n若为第一次使用, 请先注册一个账号\n" );
    }
    flag = 0;
    sqlite3_free(stmt);
    return 0;
}

// 将登录得到的结果存入结构变量account
/*
   data 为sqlite3_exec函数传入的数据
   argc 为SQL语句选择出来的数据个数
   argv 指向选择出的第一个字符串的指针
   column_name 指向列名的第一个字符串的指针

   注意: 函数内对 user 内的指针使用了malloc
   return:
   0: 正常完成
   1: 有多个符合条件的账号
*/
int callback_login( void* data,int argc,char **argv,char **notused ){
    int i = 0;
    char *a = *argv, **p = &id;

    putchar('\n');
    if(flag==1){
        printf( "有多个符合条件的账号!\n请使用学号登录!\n" );
        for( i=0;i<argc;i++ ){
            p = &id;
            (*p) = NULL;
            p++;
        }
        return 1;
    }
    for( i=0;i<argc;i++ ){
        if( argv[i]!=NULL ){
            *p = malloc(strlen(argv[i]+1));
            memcpy( (*p), argv[i], strlen(argv[i])+1 );
        }
        else{
            (*p) = NULL;
        }
        p++;
    }
    flag = 1;
    putchar('\n');
    return 0;
}
