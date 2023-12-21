int search_callback( void *p, int argc, char **argv, char **column_name ){
    int i = 0, a, *t = (int *)p;(*t)++;

    putchar('\n');
    if( (*t) > 1 ){
        printf( "有多个符合条件的账号!\n请使用学号登录!\n" );
        return 1;
    }
    return 0;
}

// 加人函数
/*
   ID 是加入的人的id, id 是加入的组织的id所在数组中的位置+1
*/
int add_people(){
    int err, t, id;
    char a, *errmsg = NULL, *ID, *class, *stmt0, *stmt, *select0, *select, authority;

    // 选择院系/课程/社团
    err = -1;
    a = 0;
    while( err==-1 ){
        while( a!='2' && a!='1' && a!='0' && a!='e' ){
            printf("\n请输入加人进入哪类组织(0: 院系, 1: 课程, 2: 社团, e: 返回菜单): ");
            a = getchar();
            clear_input_buffer();
        }
        if( a=='e' ){
            return -1;
        }
        id = 0;
        switch(a){
            case '0':{
                stmt0 = "insert into ACCOUNT_DEPARTMENT values(%Q,%Q,%Q,%c);";
                printf( "你在如下院系/班级担任管理员\n" );
                get_authority( a, 0 );
                err = 1;
                while( err!=0 ){
                    printf( "请选择加人进入哪个院系/班级(请输入院系前的编号, -1: 返回上一级): \n" );
                    scanf( "%d", &id );
                    clear_input_buffer();
                    if( id==-1 ){
                        err =-1;
                        break;
                    }
                    if( id >= num[a][0] || id <= 0 ){
                        err = 1;
                        printf( "输入错误\n" );
                        continue;
                    }
                    class = (user.departments[0][id-1]).class;
                    err = 0;
                }
                break;
            }
            case '1':{
                stmt0 = "insert into ACCOUNT_COURSE values(%Q,%Q,%c);";
                printf( "你在如下课程担任管理员\n" );
                get_authority( a, 0 );
                err = 1;
                while( err!=0 ){
                    printf( "请选择加人进入哪个课程(请输入课程的编号, e: 返回上一级): \n" );
                    scanf( "%d", &id );
                    clear_input_buffer();
                    if( id==-1 ){
                        err =-1;
                        break;
                    }
                    if( id >= num[a][0] || id <= 0 ){
                        err = 1;
                        printf( "输入错误\n" );
                        continue;
                    }
                    err = 0;
                }
                break;
            }
            case '2':{
                stmt0 = "insert into ACCOUNT_CLUB values(%Q,%Q,%c);";
                printf( "你在如下社团担任管理员\n" );
                get_authority( a, 0 );
                err = 1;
                while( err!=0 ){
                    printf( "请选择加人进入哪个社团(请输入社团的编号, e: 返回上一级): \n" );
                    scanf( "%d", &id );
                    clear_input_buffer();
                    if( id==-1 ){
                        err =-1;
                        break;
                    }
                    if( id >= num[a][0] || id <= 0 ){
                        err = 1;
                        printf( "输入错误\n" );
                        continue;
                    }
                    err = 0;
                }
                break;
            }
            case 'e':{
                return -1;
            }
        }
        if( err==-1 ){
            continue;
        }
        // 选择加人学号和权限
        err = -1;
        while( err==-1 ){
            printf( "被加者的" );
            input_id( &ID );
            if( ID[0]=='e' && ID[1]==0 ){
                err = -1;
                Free( 1, &ID );
                break;
            }
            t = 0;
            select0 = "select * from ACCOUNTS where ID == %Q;";
            select = sqlite3_mprintf( select0, ID );
            if( select == NULL ){
                sqlite3_free( select );
                free( ID );
                fprintf( stderr, "无法生成查找账号语句!\n" );
                return 1;
            }
            err = sqlite3_exec( db, select, callback_frequency, &t, &errmsg );
            if( err ){
                sqlite3_free(select);
                free( ID );
                fprintf( stderr, "SQL error: %s\n", errmsg );
                sqlite3_free(errmsg);
                return 2;
            }
            if( t==0 ){
                err = -1;
                printf( "未能找到该用户, 请重新输入\n" );
                Free( 1, &ID );
                sqlite3_free( select );
                continue;
            }
            err = -1;
            authority = 0;
            while( err==-1 ){
                printf( "被加者的权限\n(0: 无权限 1: 接收者 2: 发送者 3: 接收者+发送者 4: 管理员\n\
 5: 接收者+管理员 6: 发送者+管理员 7: 全都要 e: 返回上一级): " );
                authority = getchar();
                clear_input_buffer();
                if( authority=='e' ){
                    err = -1;
                    authority = 0;
                    break;
                }
                else if( authority > '7' || authority < '0' ){
                    err = -1;
                    authority = 0;
                    continue;
                }
                err = 0;
            }
        }
    }
    switch(a){
        case '0':{
            stmt = sqlite3_mprintf( stmt0, (user.departments[0][id-1]).id, ID, class, authority );
            break;
        }
        case '1':{
            stmt = sqlite3_mprintf( stmt0, (user.courses[0][id-1]).id, ID, authority );
            break;
        }
        case '2':{
            stmt = sqlite3_mprintf( stmt0, (user.clubs[0][id-1]).id, ID, authority );
            break;
        }
    }
    free(ID);
    if( stmt == NULL ){
        sqlite3_free( stmt );
        fprintf( stderr, "无法生成加人语句!\n" );
        return 1;
    }
    err = sqlite3_exec( db, stmt, NULL, NULL, &errmsg );
    if( err!=0 ){
        sqlite3_free( stmt );
        fprintf( stderr, "SQL error: %s\n", errmsg );
        sqlite3_free(errmsg);
        return 2;
    }
    sqlite3_free( stmt );
    return 0;
}

// 删人函数回调
int delete_callback( void *data, int argc, char **argv, char **column_name ){
    int i, a, *p = (int *)data;
    a = atoi(argv[argc-1]);
    (*p) = a;
    return 0;
}

// 删人函数
int delete_people(){
    int err, t, id;
    char a, *errmsg = NULL, *ID, *class, *stmt0, *stmt, *select0, *select, authority;

    // 选择院系/课程/社团
    err = -1;
    a = 0;
    while( err==-1 ){
        while( a!='2' && a!='1' && a!='0' && a!='e' ){
            printf("\n请输入从哪类组织中删人(0: 院系, 1: 课程, 2: 社团, e: 返回菜单): ");
            a = getchar();
            clear_input_buffer();
        }
        if( a=='e' ){
            return -1;
        }
        id = 0;
        switch(a){
            case '0':{
                stmt0 = "delete from ACCOUNT_DEPARTMENT where rowid==%d;";
                select0 = "select rowid from ACCOUNT_DEPARTMENT where department_ID==%Q and ID==%Q and class==%Q limit 1 offset 1;";
                printf( "你在如下院系/班级担任管理员\n" );
                get_authority( a, 0 );
                err = 1;
                while( err!=0 ){
                    printf( "请选择从哪个院系/班级删人(请输入院系前的编号, -1: 返回上一级): \n" );
                    scanf( "%d", &id );
                    clear_input_buffer();
                    if( id==-1 ){
                        err =-1;
                        break;
                    }
                    if( id >= num[a][0] || id <= 0 ){
                        err = 1;
                        printf( "输入错误\n" );
                        continue;
                    }
                    class = (user.departments[0][id-1]).class;
                    err = 0;
                }
                break;
            }
            case '1':{
                stmt0 = "delete from ACCOUNT_COURSE where rowid==%d;";
                select0 = "select rowid from ACCOUNT_COURSE where course_ID==%Q and ID==%Q limit 1 offset 1;";
                printf( "你在如下课程担任管理员\n" );
                get_authority( a, 0 );
                err = 1;
                while( err!=0 ){
                    printf( "请选择从哪个课程删人(请输入课程的编号, e: 返回上一级): \n" );
                    scanf( "%d", &id );
                    clear_input_buffer();
                    if( id==-1 ){
                        err =-1;
                        break;
                    }
                    if( id >= num[a][0] || id <= 0 ){
                        err = 1;
                        printf( "输入错误\n" );
                        continue;
                    }
                    err = 0;
                }
                break;
            }
            case '2':{
                stmt0 = "delete from ACCOUNT_CLUB where rowid==%d;";
                select0 = "select rowid from ACCOUNT_CLUB where club_ID==%Q and ID==%Q limit 1 offset 1;";
                printf( "你在如下社团担任管理员\n" );
                get_authority( a, 0 );
                err = 1;
                while( err!=0 ){
                    printf( "请选择从哪个社团删人(请输入社团的编号, e: 返回上一级): \n" );
                    scanf( "%d", &id );
                    clear_input_buffer();
                    if( id==-1 ){
                        err =-1;
                        break;
                    }
                    if( id >= num[a][0] || id <= 0 ){
                        err = 1;
                        printf( "输入错误\n" );
                        continue;
                    }
                    err = 0;
                }
                break;
            }
            case 'e':{
                return -1;
            }
        }
        if( err==-1 ){
            continue;
        }
        // 选择删人学号
        err = -1;
        while( err==-1 ){
            printf( "被删者的" );
            input_id( &ID );
            if( ID[0]=='e' && ID[1]==0 ){
                err = -1;
                Free( 1, &ID );
                break;
            }
            t = 0;
            switch(a){
                case '0':{
                    select = sqlite3_mprintf( select0, (user.departments[0][id-1]).id, ID, class );
                    break;
                }
                case '1':{
                    select = sqlite3_mprintf( select0, (user.courses[0][id-1]).id, ID );
                    break;
                }
                case '2':{
                    select = sqlite3_mprintf( select0, (user.clubs[0][id-1]).id, ID );
                    break;
                }
            }
            if( select == NULL ){
                sqlite3_free( select );
                free( ID );
                fprintf( stderr, "无法生成查找账号语句!\n" );
                return 1;
            }
            err = sqlite3_exec( db, select, delete_callback, &t, &errmsg );
            if( err ){
                sqlite3_free(select);
                free( ID );
                fprintf( stderr, "SQL error: %s\n", errmsg );
                sqlite3_free(errmsg);
                return 2;
            }
            if( t==0 ){
                err = -1;
                printf( "未能找到该用户, 请重新输入\n" );
                Free( 1, &ID );
                sqlite3_free( select );
                continue;
            }
            err = 0;
        }
    }
    stmt = sqlite3_mprintf( stmt0, t );
    free(ID);
    if( stmt == NULL ){
        sqlite3_free( stmt );
        fprintf( stderr, "无法生成删人语句!\n" );
        return 1;
    }
    err = sqlite3_exec( db, stmt, NULL, NULL, &errmsg );
    if( err!=0 ){
        sqlite3_free( stmt );
        fprintf( stderr, "SQL error: %s\n", errmsg );
        sqlite3_free(errmsg);
        return 2;
    }
    sqlite3_free( stmt );
    return 0;
}

// 修改成员函数
/*
   return:
   -1: 中途退出
   0: 成功完成
   1: 无法生成语句
   2: 无法执行语句
*/
int change_people(){
    char c = 0;
    int i, a, err=-1;

    while( err==-1 ){
        while( c!='0' && c!='1' && c!='e' ){
            printf("请输入加人或删人(0: 加人, 1: 删人, e: 返回菜单): ");
            c = getchar();
            clear_input_buffer();
        }
        switch(c){
            case '0':{
                err = add_people();
                if( err==0 ){
                    printf( "成功添加\n" );
                }
                break;
            }
            case '1':{
                err = delete_people();
                if( err==0 ){
                    printf( "成功删除\n" );
                }
                break;
            }
            case 'e':{
                return -1;
            }
        }
    }
    return 0;
}

// 登出函数
int log_out(){
    int i, j, err;
    char *errmsg=NULL, *stmt0, *stmt;

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
    Free( 3, &(user.id), &(user.name), &(user.password) );
    return 0;
}

// 注销函数
int close(){
    int i, j, err;
    char c=0, *errmsg=NULL, *stmt, *stmt0;

    while( c!='1' && c!='e' ){
        printf( "真的要注销吗?这会使你的账户消失(真的很久很久...)\n(1: 确认, e: 退出): " );
        c = getchar();
        clear_input_buffer();
        if( c=='e' ){
            return -1;
        }
    }
    stmt0 = "delete from ACCOUNTS where ID==%Q;";
    stmt = sqlite3_mprintf( stmt0, user.id );
    if( stmt == NULL ){
        sqlite3_free( stmt );
        fprintf( stderr, "无法生成注销语句!\n" );
        return 1;
    }
    err = sqlite3_exec( db, stmt, NULL, NULL, &errmsg );
    if( err ){
        sqlite3_free( stmt );
        fprintf( stderr, "无法执行注销语句!\nSQL error: %s\n", errmsg );
        sqlite3_free(errmsg);
        return 2;
    }
    err = log_out();
    return err;
}

// 修改密码函数
int change_password(){
    int i, j, err;
    char c=0, *errmsg=NULL, *stmt, *stmt0, *password;

    stmt0 = "update from ACCOUNTS set password=%Q where ID==%Q;";
    err = input_passwd( &password );
    if(err==-1){
        return -1;
    }
    stmt = sqlite3_mprintf( stmt0, password, user.id );
    if( stmt == NULL ){
        sqlite3_free( stmt );
        fprintf( stderr, "无法生成修改密码语句!\n" );
        free(password);
        return 1;
    }
    err = sqlite3_exec( db, stmt, NULL, NULL, &errmsg );
    if( err ){
        sqlite3_free( stmt );
        fprintf( stderr, "无法执行修改密码语句!\nSQL error: %s\n", errmsg );
        sqlite3_free(errmsg);
        free(password);
        return 2;
    }
    free(user.password);
    user.password = password;
    printf("修改密码成功\n");
    return 0;
}