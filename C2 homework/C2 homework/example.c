#include <stdio.h>
#include <sqlite3.h>
int k=0;
static int callback(void *Not_used, int argc, char **argv, char **azColName){
    int i;
    k++;
    putchar('\n');
    fprintf( stderr, "查找到的第 %d 组数据\n", k );
    fprintf( stderr, "%s: \n", (char*)(Not_used) );
    for(i=0; i<argc; i++){
        printf( "%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL" );
    }
    printf("\n");
    return 0;
}

/*callback函数中
argc是table中的列数，
argv是字符串数组的指针，这些字符串为select语句选择出来的值，
azColName是table的栏目名称*/

  
int main(int argc, char *argv[]){
    sqlite3 *db;
    char *zErrMsg = 0;
    int rc;
    char *header = "回调函数输出";
    if( argc!=3 ){
      fprintf(stderr, "格式: %s 数据库名 SQL语句\n请重新输入!\n", argv[0]);
      return(1);
    }
    rc = sqlite3_open(argv[1], &db);
    if( rc ){
      fprintf(stderr, "无法打开数据库: %s\n", sqlite3_errmsg(db));
      printf( "Err = %d\n", rc );
      sqlite3_close(db);
      return(1);
    }
    else{
      fprintf(stdout,"成功打开数据库\n");
    }
    rc = sqlite3_exec(db, "PRAGMA count_changes = true;", &callback, header, &zErrMsg);
    if( rc != SQLITE_OK ){
      fprintf(stderr, "SQL error: %s\n", zErrMsg);
      printf( "Err = %d\n", rc );
      sqlite3_free(zErrMsg);
    }                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                      
    rc = sqlite3_exec(db, argv[2], &callback, header, &zErrMsg);
    if( rc != SQLITE_OK ){
      fprintf(stderr, "SQL error: %s\n", zErrMsg);
      printf( "Err = %d\n", rc );
      sqlite3_free(zErrMsg);
    }
    else{
      fprintf(stdout,"操作成功完成\n");
    }
    sqlite3_close(db);
    return 0;
}