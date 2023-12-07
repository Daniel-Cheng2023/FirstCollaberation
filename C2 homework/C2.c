#include "login.c"

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
    while( user.id == NULL ){
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
                user.id = "exit";
                break;
            }
        }
    }
    return 0;
}