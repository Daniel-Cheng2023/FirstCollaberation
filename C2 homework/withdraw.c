int callback_check(void *data , int argc , char **argv , char **azColName) {
    int *fi; fi = (int *)data;
    fi[++cnt] = atoi(argv[0]);
    return 0;
}

int is_not_in(int id , int *a) {
    for(int i = 1;i <= cnt;i++) {
        if(a[i] == id) return 1;
    }
    return 0;
}

int Choose_to_Withdraw() {
    char *ID = NULL , *Err1 = NULL, *Err2 = NULL , *Err3 = NULL;
    int id = atoi(ID), nw1, nw2, nw3; Len = cnt = 0;
    const char *op1 = "select max(rowid) from %q_announce";
    char *sql1 = sqlite3_mprintf(op1 , user.id);
    nw1 = sqlite3_exec(db , sql1 , callback_id, &Len , &Err1);
    int *a = (int *)malloc(Len * sizeof(int));
    const char *op2 = "select * from %q_announce";
    int tot = 10;
    for(;tot > 0;tot--) {
        input_text("请输入你要撤回的信息的ID(输入e退出)" , 5 , &ID );
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

int print() {
    fprintf(stdout , "已发送信息列表\n");
    return select_notices(1);
}

int Withdraw() {
    int num = 10;
    for(;num > 0;num--) {
        if( print() != 0 ) continue;
        if( Choose_to_Withdraw() == 1 ) return 0;
    }
    fprintf(stderr , "连接超时\n");
    return 1;
}

int choose( int *p ){
    int err, t = 0;
    char *stmt0, *stmt, *errmsg = NULL;

    select_notices( 1 );
    stmt0 = "select * from %q_announce;";
    stmt = sqlite3_mprintf( stmt0, user.id );
    if( stmt==NULL ){
        sqlite3_free(stmt);
        printf("无法生成查询个数语句\n");
        return 1;
    }
    err = sqlite3_exec( db, stmt, callback_frequency, &t, &errmsg );
    sqlite3_free(stmt);
    if( err!=0 ){
        printf("无法执行查询次数语句\n");
        return 2;
    }
    (*p) = -1;
    while( (*p)<=0 || (*p)>t ){
        printf("请输入撤回通知的正整数编号(-1: 返回上一级): ");
        scanf("%d", p);
        clear_input_buffer();
        if( (*p)==-1 ){
            return -1;
        }
    }
    return 0;
}

int back( int n ){
    int err;
    char *stmt0, *stmt, *errmsg;

    stmt0 = "delete from NOTICES where rowid in (select id from %q_announce limit 1 offset %d);";
    stmt = sqlite3_mprintf( stmt0, user.id, n );
    if( stmt==NULL ){
        printf("无法生成撤回语句\n");
        sqlite3_free(stmt);
        return 1;
    }
    err = sqlite3_exec( db, stmt, NULL, NULL, &errmsg );
    sqlite3_free(stmt);
    if( err ){
        printf("无法执行撤回语句\n");
    }
    return err;
}

int withdraw(){
    int err, n;

    err = -1;
    while( err==-1 ){
        choose(&n);
        err = back( n-1 );
    }
    if( err==0 ){
        printf( "成功撤回\n" );
    }
    return err;
}