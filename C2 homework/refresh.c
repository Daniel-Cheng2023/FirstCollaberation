// 查询记录个数函数
/*
   p 指向的整数即为记录的个数
*/
int callback_frequency( void *p, int argc, char **argv, char **column ){
    int *i = (int *)p;
    (*i)++;
    return 0;
}

// 记录是哪个院系/课程/社团的成员及权限
/*
   将查询到的本账号的组织关系和权限存入 user
   ~[0]表示在其中担任管理员的组织, ~[1]表示在其中担任发送者的组织, ~[2]表示在其中担任接收者的组织

   mode: 
   0: 查询院系
   1: 查询课程
   2: 查询社团

   return:
   0:
   1: 
   2: 
*/
int callback_record( void *p, int argc, char **argv, char **column ){
    int j, i, a, b, mode = *((int *)p);
    char **s, **s0, **s1, **s2;

    a = atoi( argv[argc-1] );
    if( mode==0 ){
        s0 = &(user.departments[0][num[0][0]].id);
        s1 = &(user.departments[1][num[0][1]].id);
        s2 = &(user.departments[2][num[0][2]].id);
    }
    else if( mode==1 ){
        s0 = &(user.courses[0][num[1][0]].id);
        s1 = &(user.courses[1][num[1][1]].id);
        s2 = &(user.courses[2][num[1][2]].id);
    }
    else if( mode==2 ){
        s0 = &(user.clubs[0][num[2][0]].id);
        s1 = &(user.clubs[1][num[2][1]].id);
        s2 = &(user.clubs[2][num[2][2]].id);
    }
    for( j=0;j<3;j++ ){
        if( j==0 ){
            if( (a&4)==4 ){
                if( mode==0 ){
                    num[0][0]++;
                }
                else if( mode==1 ){
                    num[1][0]++;
                }
                else if( mode==2 ){
                    num[2][0]++;
                }
                s = s0;
            }
            else{
                continue;
            }
        }
        if( j==1 ){
            if( (a&2)==2 ){
                if( mode==0 ){
                    num[0][1]++;
                }
                else if( mode==1 ){
                    num[1][1]++;
                }
                else if( mode==2 ){
                    num[2][1]++;
                }
                s = s1;
            }
            else{
                continue;
            }
        }
        if( j==2 ){
            if( (a&1)==1 ){
                if( mode==0 ){
                    num[0][2]++;
                }
                else if( mode==1 ){
                    num[1][2]++;
                }
                else if( mode==2 ){
                    num[2][2]++;
                }
                s = s2;
            }
            else{
                continue;
            }
        }
        for( i=0;i<argc-1;i++ ){
            if( argv[i]==NULL ){
                (*s) = NULL;
            }
            else{
                b = strlen( argv[i] ) + 1;
                (*s) = (char *)malloc(b);
                memcpy( (*s), argv[i], b );
            }
            s++;
        }
    }
    return 0;
}

// 存储院系、课程及社团信息
/*
   return:
   0: 正常完成
   1: 无法生成语句
   2: 无法执行语句
*/
int record(){
    int err, a = 0, b = 1, c = 2, i, j;
    char *stmt0, *stmt, *errmsg;

    // 初始化
    for( i=0;i<3;i++ ){
        for( j=0;j<4;j++ ){
            num[i][j] = 0;
        }
    }
    for( i=0;i<100;i++ ){
        for( j=0;j<3;j++ ){
            Free( 7, &(user.departments[j][i]).id, &(user.departments[j][i]).name, &(user.departments[j][i]).class, &(user.courses[j][i]).id, &(user.courses[j][i]).name, &(user.clubs[j][i]).id, &(user.clubs[j][i]).id);
        }
    }
    // 存储院系信息
    stmt0 = "select DEPARTMENTS.ID, DEPARTMENTS.name, ACCOUNT_DEPARTMENT.class, \
    ACCOUNT_DEPARTMENT.identity from DEPARTMENTS inner join ACCOUNT_DEPARTMENT on \
    DEPARTMENTS.ID == ACCOUNT_DEPARTMENT.department_ID and ACCOUNT_DEPARTMENT.ID == %Q;";
    stmt = sqlite3_mprintf( stmt0, user.id );
    if( stmt == NULL ){
        sqlite3_free( stmt );
        printf( "存储社团信息时无法生成语句\n" );
        return 1;
    }
    err = sqlite3_exec( db, stmt, callback_record, &a, &errmsg );
    if( err ){
        sqlite3_free( stmt );
        printf( "无法存储所在院系个数\nsql error: %s\n", errmsg );
        sqlite3_free( errmsg );
        return 2;
    }
    // 存储课程信息
    stmt0 = "select COURSES.ID, COURSES.name, ACCOUNT_COURSE.identity \
    from COURSES inner join ACCOUNT_COURSE on \
    COURSES.ID == ACCOUNT_COURSE.course_ID and ACCOUNT_COURSE.ID == %Q;";
    stmt = sqlite3_mprintf( stmt0, user.id );
    if( stmt == NULL ){
        sqlite3_free( stmt );
        printf( "存储课程信息时无法生成语句\n" );
        return 1;
    }
    err = sqlite3_exec( db, stmt, callback_record, &b, &errmsg );
    if( err ){
        sqlite3_free( stmt );
        printf( "无法存储所在课程个数\nsql error: %s\n", errmsg );
        sqlite3_free( errmsg );
        return 2;
    }
    // 存储社团信息
    stmt0 = "select CLUBS.ID, CLUBS.name, ACCOUNT_CLUB.identity \
    from CLUBS inner join ACCOUNT_CLUB on \
    CLUBS.ID == ACCOUNT_CLUB.course_ID and ACCOUNT_CLUB.ID == %Q;";
    stmt = sqlite3_mprintf( stmt0, user.id );
    if( stmt == NULL ){
        sqlite3_free( stmt );
        printf( "存储社团信息时无法生成语句\n" );
        return 1;
    }
    err = sqlite3_exec( db, stmt, callback_record, &c, &errmsg );
    if( err ){
        sqlite3_free( stmt );
        printf( "无法存储所在社团个数\nsql error: %s\n", errmsg );
        sqlite3_free( errmsg );
        return 2;
    }
    return 0;
}

// 输出所在组织权限
/*
   p: 
   0: 查询院系
   1: 查询课程
   2: 查询社团

   o:
   0: 查询管理员权限
   1: 查询发送者权限
   2: 查询接收者权限

*/
int get_authority( int p , int o ){
    int i, a;
    char *s;

    putchar('\n');
    if( o==0 ){
        s = "管理员";
    }
    else if( o==1 ){
        s = "发送者";
    }
    else if( o==2 ){
        s = "接收者";
    }
    // 查询院系权限
    for( i=0;i<num[p][o];i++ ){
        if( p==0 ){
            printf( "%d. %s%s 权限: %s\n", i+1, (user.departments[o][i]).name, (user.departments[o][i]).class, s );
        }
        if( p==1 ){
            printf( "%d. %s 权限: %s\n", i+1, (user.courses[o][i]).name, s );
        }
        if( p==2 ){
            printf( "%d. %s 权限: %s\n", i+1, (user.clubs[o][i]).name, s );
        }
    }
    putchar('\n');
    return 0;
}

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

    if( mode==0 ){
        printf( "收件箱: \n" );
        t=1;
        stmt0="select * from %q_receive;";
    }
    else{
        printf( "你发布的通知: \n");
        t=-1;
        stmt0="select * from %q_announce;";
    }
    stmt = sqlite3_mprintf( stmt0, user.id );
    if( stmt == NULL ){
        sqlite3_free( stmt );
        printf( "查询通知时无法生成语句\n" );
        return 1;
    }
    err = sqlite3_exec( db, stmt, refresh_callback, &t, &errmsg );
    if( err ){
        sqlite3_free( stmt );
        printf( "无法查询通知个数\nsql error: %s\n", errmsg );
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
    int err, t, i, j;
    char *stmt0, *stmt, *errmsg;

    putchar('\n');
    printf( "你好, %s %s\n", user.name, user.id );
    // 消除之前可能存在的表
    stmt0 = "drop table %q_receive;";
    stmt = sqlite3_mprintf( stmt0, user.id );
    if( stmt == NULL ){
        sqlite3_free( stmt );
        printf( "删除收件箱表时无法生成语句\n" );
        return 1;
    }
    printf("\n%s\n", stmt);
    err = sqlite3_exec( db, stmt, NULL, NULL, &errmsg );
    printf("\n%s\n", stmt);
    if( err ){
        sqlite3_free( stmt );
        printf( "无法执行删除收件箱语句\nsql error: %s\n", errmsg );
        sqlite3_free( errmsg );
    }
    stmt0 = "drop table %q_announce;";
    stmt = sqlite3_mprintf( stmt0, user.id );
    printf("\n%s\n", stmt);
    err = sqlite3_exec( db, stmt, NULL, NULL, &errmsg );
    printf("\n%s\n", stmt);
    sqlite3_free(stmt);
    if( err ){
        printf( "无法执行删除自己发送的通知的表语句\nsql error: %s\n", errmsg );
        sqlite3_free( errmsg );
    }
    // 收件箱建表
    stmt0 = "create table %q_receive(id TEXT not NULL,title TEXT,content TEXT not null,announcer_name TEXT,announcer_id TEXT,launch_time TEXT default current_timestamp,effect_time TEXT,dead_time TEXT);";
    stmt = sqlite3_mprintf( stmt0, user.id );
    printf("\n%s\n", stmt);
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
    if( err ){
        return err;
    }
    record();
    printf( "你所在的组织: \n" );
    for( i=0;i<3;i++ ){
        for( j=0;j<3;j++ ){
            get_authority( j, i );
        }
    }
    return 0;
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