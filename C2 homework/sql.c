#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sqlite3.h>

void Free(int,...);
void clear_input_buffer(void);
int inputs(const char*, int, char**);
int input_id(char**);
int input_name(char**);
int input_passwd(char**);

// 释放指针+初始化函数
/*
   释放 n 个char*类型的指针
*/
void Free( int n, ... ){
    int i;
    char **p = NULL;
    va_list a;

    va_start(a, n);
    for( i=0;i<n;i++ ){
        p = va_arg( a, char** );
        free(*p);
        *p = NULL;
    }
    va_end(a);
}

//清除输入缓冲区
/*
   清楚输入缓冲
*/
void clear_input_buffer( void ){
    int c;
    while( (c = getchar())!='\n' && c!=EOF );
    return;
}

// 输入字符串函数
/*
   输出自己输入的 guide 字符串, 限制字符串长度不长于 limit 
   将输入的字符串存入指针 ap 指向的字符串指针

   注意: 函数内对 ap 指向的指针使用了malloc

   return:
   -1: 超出范围
   0: 正常输入
   1: 中止输入
*/
int inputs( const char *guide, int limit, char **ap ){
    fprintf( stderr, "%s(0: null, e: exit): ", guide );
    *ap = malloc(limit);
    scanf( "%s", *ap );
    clear_input_buffer();
    if( strcmp(*ap,"0")==0 ){
        (*ap) = NULL;
    }
    else if( strcmp(*ap,"e")==0 ){
        Free(1,ap);
        return 1;
    }
    else if( (*ap)[limit] != 0 ){
        fprintf( stderr, "Memory leak risk when inputting %s!\n", guide );
        Free(1,ap);
        return -1;
    }
    return 0;
}

// 输入ID函数
/*
   输入字符串存入指针 idp 指向的字符串指针
   用于输入 ID 时
   return:
   0: 正常输入
   -1: 中止输入
*/
int input_id(char **idp){
    int Err = 1;
    while(Err){
        Err = inputs( "学号", 10, idp );
        if(Err==1){
            Free(1,idp);
            return -1;
        }
        if(Err==0){
            if( (*idp)==NULL ){
                fprintf( stdout, "学号不能为空。\n" );
                Err = 1;
                Free(1,idp);
            }
            else if( !( (*idp)[10]==0 && (*idp)[9]!=0 ) ){
                fprintf( stdout, "学号只能为10位。\n" );
                Err = 1;
                Free(1,idp);
            }
        }
    }
    return 0;
}

// 输入name函数
/*
   输入字符串存入指针 np 指向的字符串指针
   用于输入 name 时
   return:
   0: 正常输入
   -1: 中止输入
*/
int input_name(char **np){
    int Err = 1;
    while(Err){
        Err = inputs( "名字", 100, np );
        if( Err==1 ){
            Free(1,np);
            return -1;
        }
        if( Err==-1 ){
            Free(1,np);
        }
    }
    return 0;
}

// 输入password函数
/*
   输入字符串存入指针 pp 指向的字符串指针
   用于输入 password 时
   return:
   0: 正常输入
   -1: 中止输入
*/
int input_passwd(char **pp){
    int Err = 1;
    while(Err){
        Err = inputs( "密码", 100, pp );
        if( Err==1 ){
            Free(1,pp);
            return -1;
        }
        if( Err==0 ){
            if( (*pp)==NULL ){
                fprintf( stdout, "密码不能为空。\n" );
                Err = -1;
                Free(1,pp);
            }
            else if( (*pp)[5]==0 ){
                fprintf( stdout, "密码应长于6位。\n" );
                Err = -1;
                Free(1,pp);
            }
        }
    }
    return 0;
}


/*
int main(int argc,char *argv[]){
    char p;

    return 0;
}
*/
