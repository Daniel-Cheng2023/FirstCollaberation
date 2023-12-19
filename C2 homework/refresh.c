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
   0: 成功完成
   1: 无法生成语句
   2: 无法执行语句
*/
int select_notices( int mode ){
    int err, t;
    char *stmt0, *stmt, *errmsg;

    (mode==0) ? (printf( "收件箱: \n" ), t=1, stmt0="select * from %q_receive") : \
(printf( "你发布的通知: \n"), t=-1, stmt0="select * from %q_announce;");
    stmt = sqlite3_mprintf( stmt0, user.id );
    if( stmt == NULL ){
        sqlite3_free( stmt );
        printf( "查询通知时无法生成语句\n" );
        return 1;
    }
    err = sqlite3_exec( db, stmt, refresh_callback, &t, &errmsg );
    if( err ){
        sqlite3_free( stmt );
        printf( "无法查询所在社团个数\nsql error: %s\n", errmsg );
        sqlite3_free( errmsg );
        return 1;
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
int refresh(){
    int err, t;
    char *stmt0, *stmt, *errmsg;

    // 消除之前可能存在的表
    stmt0 = "drop table %q_receive;";
    stmt = sqlite3_mprintf( stmt0, user.id );
    if( stmt == NULL ){
        sqlite3_free( stmt );
        printf( "删除收件箱表时无法生成语句\n" );
        return 1;
    }
    err = sqlite3_exec( db, stmt, NULL, NULL, &errmsg );
    if( err ){
        sqlite3_free( stmt );
        printf( "无法执行删除收件箱语句\nsql error: %s\n", errmsg );
        sqlite3_free( errmsg );
    }
    stmt0 = "drop table %q_announce;";
    stmt = sqlite3_mprintf( stmt0, user.id );
    if( err ){
        sqlite3_free( stmt );
        printf( "无法执行删除收件箱语句\nsql error: %s\n", errmsg );
        sqlite3_free( errmsg );
        return 2;
    }
    err = sqlite3_exec( db, stmt, NULL, NULL, &errmsg );
    if( err ){
        sqlite3_free( stmt );
        printf( "无法执行删除自己发送的通知的表语句\nsql error: %s\n", errmsg );
        sqlite3_free( errmsg );
    }
    // 收件箱建表
    stmt0 = "create table %q_receive(id TEXT not NULL,title TEXT,content TEXT not null,announcer_name TEXT,announcer_id TEXT,launch_time TEXT default current_timestamp,effect_time TEXT,dead_time TEXT);";
    stmt = sqlite3_mprintf( stmt0, user.id );
    if( stmt == NULL ){
        sqlite3_free( stmt );
        printf( "无法生成收件箱建表语句\n" );
        return 1;
    }
    err = sqlite3_exec( db, stmt, NULL, NULL, &errmsg );
    if( err ){
        sqlite3_free( stmt );
        printf( "无法执行建立收件箱语句\nsql error: %s\n", errmsg );
        sqlite3_free( errmsg );
    }
    sqlite3_free(stmt);
    stmt0 = "insert into table %q_receive\
    (id, title, content, announcer_name, announcer_id, launch_time, effect_time, dead_time) \
    select rowid, * from NOTICES where rowid in (\
        select NOTICE_DEPARTMENT.ID from NOTICE_DEPARTMENT where \
            NOTICE_DEPARTMENT.department_ID in (\
                select ACCOUNT_DEPARTMENT.department_ID from ACCOUNT_DEPARTMENT where \
                    ACCOUNT_DEPARTMENT.ID == %Q)\
            and\
            NOTICE_DEPARTMENT.class in (\
                select ACCOUNT_DEPARTMENT.class from ACCOUNT_DEPARTMENT where \
                    ACCOUNT_DEPARTMENT.ID == %Q)\
            and\
            NOTICE_DEPARTMENT.identity in (\
                select ACCOUNT_DEPARTMENT.identity from ACCOUNT_DEPARTMENT where \
                    ACCOUNT_DEPARTMENT.ID == %Q)\
        union\
        select NOTICE_CLUB.ID from NOTICE_CLUB where \
            NOTICE_CLUB.club_ID in (\
                select ACCOUNT_CLUB.club_ID from ACCOUNT_CLUB where \
                    ACCOUNT_CLUB.ID == %Q)\
            and\
            NOTICE_CLUB.identity in (\
                select ACCOUNT_CLUB.identity from ACCOUNT_CLUB where \
                    ACCOUNT_CLUB.ID == %Q)\
        union\
        select NOTICE_COURSE.ID from NOTICE_COURSE where \
            NOTICE_COURSE.course_ID in (\
                select ACCOUNT_COURSE.course_ID from ACCOUNT_COURSE where \
                    ACCOUNT_COURSE.ID == %Q)\
            and\
            NOTICE_COURSE.identity in (\
                select ACCOUNT_COURSE.identity from ACCOUNT_COURSE where \
                    ACCOUNT_COURSE.ID == %Q)\
    )";
    stmt = sqlite3_mprintf( stmt0, user.id, user.id, user.id, user.id, user.id, user.id, \
    user.id, user.id );
    if( stmt == NULL ){
        sqlite3_free( stmt );
        printf( "无法生成收件箱插表语句\n" );
        return 1;
    }
    err = sqlite3_exec( db, stmt, NULL, NULL, &errmsg );
    if( err ){
        sqlite3_free( stmt );
        printf( "无法执行插入收件箱语句\nsql error: %s\n", errmsg );
        sqlite3_free( errmsg );
    }
    sqlite3_free( stmt );
    // 收件箱查询
    err = select_notices(0);
    if(err){
        return err;
    }
    // 自己发送的通知建表
    stmt0 = "create table %q_announce(id TEXT not NULL,title TEXT,content TEXT not null,announcer_name TEXT,announcer_id TEXT,launch_time TEXT default current_timestamp,effect_time TEXT,dead_time TEXT);";
    stmt = sqlite3_mprintf( stmt0, user.id );
    if( stmt == NULL ){
        sqlite3_free( stmt );
        printf( "无法生成自己发送建表语句\n" );
        return 1;
    }
    err = sqlite3_exec( db, stmt, NULL, NULL, &errmsg );
    if( err ){
        sqlite3_free( stmt );
        printf( "无法执行自己发送建表语句\nsql error: %s\n", errmsg );
        sqlite3_free( errmsg );
    }
    sqlite3_free(stmt);
    stmt0 = "insert into %q_announce(id, title, content, announcer_name, announcer_id, launch_time, effect_time, dead_time) select rowid, * from NOTICES where announcer_id==%Q;";
    stmt = sqlite3_mprintf( stmt0, user.id, user.id );
    if( stmt == NULL ){
        sqlite3_free( stmt );
        printf( "无法生成自己发送插表语句\n" );
        return 1;
    }
    err = sqlite3_exec( db, stmt, NULL, NULL, &errmsg );
    if( err ){
        sqlite3_free( stmt );
        printf( "无法执行自己发送插表语句\nsql error: %s\n", errmsg );
        sqlite3_free( errmsg );
    }
    sqlite3_free( stmt );
    // 查询自己发的通知
    err = select_notices(1);
    return err;
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