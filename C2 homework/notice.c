// 输出收件箱或自己发布的通知函数
/*
   time 表示查询到第几个结果, 值由 void *t 传入
   正的表示收件箱的通知, 负的表示自己发布的通知

   输出格式:
   i.(表示第i个)
   标题: ...
   正文: ...(限20个字符)
   发布者: ...(姓名+id)(如果是自己发布的, 则不会输出)
   发布时间: ...
*/
int refresh_callback( void *t, int argc, char **argv, char **column){
    int i, k, *time = (int*)t;

    putchar('\n');
    printf( "%d.\n", abs((*time)) );
    if( *(time) > 0 ){
        (*time)++;
    }
    else{
        (*time)--;
    }
    for( i=0;i<argc;i++ ){
        switch(i){
            case 0:{
                printf( "标题: %s\n", argv[i] );
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
                putchar('\n');
                break;
            }
            case 2:{
                if( (*time) > 0 ){
                    printf( "发布者: %s%s\n", argv[i], argv[i+1] );
                }
                break;
            }
            case 4:{
                printf( "发布时间: %s\n", argv[i] );
                break;
            }
        }
    }
    putchar('\n');
    return 0;
}

// 查询通知
/*
   查询收件箱和自己发布的通知

   mode:
   0: 收到的通知
   1: 自己发的通知

   return:
   0: 正常完成
   1: 无法生成语句
   2: 无法执行语句
*/
int select( sqlite3 *db, int mode ){
    int err, t;
    char *stmt0, stmt, errmsg;

    (mode==0) ? printf( "收件箱: \n" ) : printf( "你发布的通知: \n");
    (mode==0) ? (t=1) : (t=-1);
    (mode==0) ? (stmt0="select * from %q_receive;") : (stmt0="select * from %q_announce;");
    stmt = sqlite3_mprintf( stmt0, id );
    err = mprintf_report( stmt, "无法生成查询语句" );
    if(err==1){
        return 1;
    }
    err = sqlite3_exec( db, stmt, refresh_callback, &t, &errmsg );
    err = exec_report( err, errmsg, "无法查询", "查询成功" );
    if( err!= 0){
        sqlite3_free( stmt );
        return 2;
    }
    sqlite3_free( stmt );
    return 0;
}

// 刷新查询状态函数
/*
   建立表 <id>_receive, <id>_announce, 并查询收到的信息和用户自己发布的信息

   注意: 表 <id>_receive, <id>_announce 需要在后续drop掉

   return:
   0: 正常完成
   1: 无法生成语句
   2: 无法执行语句

   <>: 表示将整体(包括尖括号)替换为尖括号内部字符指针对应的字符串
*/
int refresh( sqlite3 *db ){
    int err, t;
    char *stmt0, *stmt, *errmsg;

    // 消除之前可能存在的表
    stmt0 = "drop table %q_receive;";
    stmt = sqlite3_mprintf( stmt0, id );
    err = mprintf_report( stmt, "无法生成删表语句" );
    if(err==1){return 1;}
    stmt0 = "drop table %q_announce;";
    stmt = sqlite3_mprintf( stmt0, id );
    err = mprintf_report( stmt, "无法生成删表语句" );
    if(err==1){return 1;}
    // 收件箱建表
    stmt0 = "create table %q_receive \
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
    stmt = sqlite3_mprintf( stmt0, id, id, id, id, id );
    err = mprintf_report( stmt, "无法生成建表语句" );
    if(err==1){
        return 1;
    }
    err = sqlite3_exec( db, stmt, NULL, NULL, &errmsg );
    err = exec_report( err, errmsg, "无法建表", "建表成功" );
    if( err!= 0){
        sqlite3_free( stmt );
        return 2;
    }
    sqlite3_free( stmt );
    // 收件箱查询
    err = select( db, 0 );
    if(err){
        return err;
    }
    // 自己发送的通知建表
    stmt0 = "create table %q_announce as select * from NOTICES where announcer_id==%Q;";
    stmt = sqlite3_mprintf( stmt0, id, id );
    err = mprintf_report( stmt, "无法生成建表语句" );
    if(err==1){
        return 1;
    }
    err = sqlite3_exec( db, stmt, NULL, NULL, &errmsg );
    err = exec_report( err, errmsg, "无法建表", "建表成功" );
    if( err!= 0){
        sqlite3_free( stmt );
        return 2;
    }
    sqlite3_free( stmt );
    // 查询自己发的通知
    return select( db, 1 );
}

/*
int main(){
    sqlite3 *db;
    int err;
    char *errmsg;

    sqlite3_open( "C2.db", &db );
    refresh( db );
    return 0;
}
*/
