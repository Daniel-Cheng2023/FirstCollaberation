// 注册函数
/*
   向 db 的ACCOUNTS表中注册账号

   return:
   -1: 中间退出
   0: 成功注册
   1: statement 无法生成
   2: statement 执行出错
*/
int regist(){
    int err = 1;
    char *errmsg = NULL, *ID = NULL, *NAME = NULL, *passwd = NULL;

    err = input_id(&ID);
    if( err==-1 ){
        return -1;
    }
    err = input_name(&NAME);
    if( err==-1 ){
        Free(1,&ID);
        return -1;
    }
    err = input_passwd(&passwd);
    if( err==-1 ){
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
        fprintf( stderr, "无法生成注册账号语句!\n" );
        return 1;
    }
    err = sqlite3_exec( db, stmt, NULL, NULL, &errmsg);
    if( err ){
        sqlite3_free(stmt);
        fprintf( stderr, "SQL error: %s\n", errmsg );
        sqlite3_free(errmsg);
        return 2;
    }
    sqlite3_free(stmt);
    fprintf( stderr, "成功注册!\n");
    printf( "注册后需重新登录\n" );
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
int login_callback( void* data,int argc,char **argv,char **notused ){
    int i = 0, a, *t = (int *)data;(*t)++;
    char **p = &user.id;

    putchar('\n');
    if( (*t) > 1 ){
        printf( "有多个符合条件的账号!\n请使用学号登录!\n" );
        for( i=0;i<argc;i++ ){
            (*p) = NULL;
            p++;
        }
        return 1;
    }
    for( i=0;i<argc;i++ ){
        if( argv[i]!=NULL ){
            a = strlen( argv[i] ) + 1;
            *p = (char *)malloc(a);
            memcpy( (*p), argv[i], a );
        }
        else{
            (*p) = NULL;
        }
        p++;
    }
    putchar('\n');
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
   1: statement 无法生成
   2: statement 执行出错
*/
int login(){
    int err = -1, t = 0;
    char type = 0, *errmsg = NULL, *ID_name, *passwd, *stmt0;
    
    while( err==-1 ){
        while( type!='e' && type!='0' && type!='1' ){
            printf("\n请输入登录方式(0: 学号登录, 1: 姓名登录, e: 返回上一级): ");
            type = getchar();
            clear_input_buffer();
        }
        switch(type){
            case '0':{
                err = input_id( &ID_name );
                stmt0 = "select * from ACCOUNTS where (ID == %Q) and (password == %Q)";
                break;
            }
            case '1':{
                err = input_name( &ID_name );
                stmt0 = "select * from ACCOUNTS where (name == %Q) and (password == %Q)";
                break;
            }
            case 'e':{
                return -1;
            }
        }
    }
    // input passwd
    err = input_passwd( &passwd );
    if( err== -1 ){
        Free( 1, &ID_name );
        return -1;
    }
    char *stmt;
    stmt = sqlite3_mprintf( stmt0, ID_name, passwd );
    Free( 2, &ID_name, &passwd );
    if( stmt == NULL ){
        sqlite3_free( stmt );
        fprintf( stderr, "无法生成获取登录数据语句!\n" );
        return 1;
    }
    err = sqlite3_exec( db, stmt, login_callback, &t, &errmsg);
    if( err ){
        sqlite3_free( stmt );
        fprintf( stderr, "SQL error: %s\n", errmsg );
        sqlite3_free(errmsg);
        return 2;
    }
    system( "clear" );
    if( user.id!=NULL ){
        printf( "成功登录!\n");
        char **p = &user.id;
        int i;
        for( i=0;i<3;i++ ){
            printf( "%s 为 %s\n", column_name[i], *p ? *p : "NULL" );
            p++;
        }
    }
    else if( t==0 ){
        printf( "未查找到该账号\n若为第一次使用, 请先注册一个账号\n" );
    }
    sqlite3_free(stmt);
    return 0;
}

/*
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
    if( err ){
      fprintf( stderr, "无法设置外键: %s\n", errmsg );
      sqlite3_free( errmsg );
    }
    else{
      fprintf(stdout,"成功设置外键\n");
    }
    while( user.id == NULL ){
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
                user.id = "exit";
                break;
            }
        }
    }
    return 0;
}
*/