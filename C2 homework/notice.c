#include "login.c"

// 输出查询到的通知
/*
   输出格式:
   标题: ...
   正文: ...(限20个字符)
   发布者: ...
   发布时间: ...
*/
int refresh_callback( void *data, int argc, char **argv, char **column){
    int i, k;

    putchar('\n');
    for( i=0;i<argc;i++ ){
        switch(i){
            case 0:{
                printf( "标题: %s", argv[i] );
                break;
            }
            case 1:{
                printf( "正文: " );
                for( k=0;k<20;k++ ){
                    if( argv[1][k]==0 ){
                        break;
                    }
                    putchar( argv[1][k] );
                }
                break;
            }
            case 2:{
                printf( "发布者: %s", argv[i] );
                break;
            }
            case 3:{
                printf( "发布时间: %s", argv[i] );
                break;
            }
        }
    }
    putchar('\n');
    return 0;
}

// 刷新查询状态函数
/*
   刷新以 id 字符串为名字的表, 并输出查询到的通知

   注意: 在函数中新建了名为 id 字符串的表, 需要在后续drop掉

   return:
   0: 正常完成
   1: 无法生成建表语句
   2: 无法建表
   3: 无法生成查询语句
   4: 无法查询
*/
int refresh( sqlite3 *db ){
    int err;
    char *table0, *table, *stmt, *select, *errmsg;
    table0 = "create table %q \
    AS select * from NOTICES where rowid in (\
        select ID from NOTICE_DEPARTMENT where \
            department_ID in (select department_ID from ACCOUNT_DEPARTMENT where ID == %Q)\
            and\
            class in (select class from ACCOUNT_DEPARTMENT where ID == %Q)\
        union\
        select ID from NOTICE_CLUB where club_ID in (\
            select club_ID from ACCOUNT_CLUB where ID == %Q)\
        union\
        select ID from NOTICE_COURSE where course_ID in (\
            select course_ID from ACCOUNT_COURSE where ID == %Q)\
    )";
    table = sqlite3_mprintf( table0, id, id, id, id, id );
    if( table == NULL ){
        sqlite3_free( table );
        printf( "无法生成建表语句\n" );
        return 1;
    }
    err = sqlite3_exec( db, table, NULL, NULL, &errmsg );
    exec_report( err, errmsg, "建表失败", "成功建表" );
    if( err!= 0){
        sqlite3_free( table );
        return 2;
    }
    sqlite3_free( table );
    select  = "select * from %q";
    stmt = sqlite3_mprintf( select, id );
    if( table == NULL ){
        sqlite3_free( table );
        printf( "无法生成查询语句\n" );
        return 3;
    }
    err = sqlite3_exec( db, stmt, refresh_callback, NULL, &errmsg );
    exec_report( err, errmsg, "刷新失败", "成功刷新" );
    if( err!= 0){
        sqlite3_free( stmt );
        return 4;
    }
    sqlite3_free( stmt );
    return 0;
}