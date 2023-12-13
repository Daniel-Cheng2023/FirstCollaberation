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