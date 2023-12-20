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
        if( (*p)!=NULL ){
            free(*p);
            *p = NULL;
        }
    }
    va_end(a);
}

//清除输入缓冲区
/*
   清除输入缓冲
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

   注意: 函数内对 ap 指向的指针使用了malloc, 需要在后面free掉

   return:
   -1: 超出范围
   0: 正常输入
   1: 中止输入
*/
int inputs( const char *guide, int limit, char **ap ){
    printf( "%s(0: null, e: exit): ", guide );
    *ap = (char *)malloc(limit);
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
    int err = 1;
    while(err){
        err = inputs( "学号", 10, idp );
        if(err==1){
            Free(1,idp);
            return -1;
        }
        if(err==0){
            if( (*idp)==NULL ){
                fprintf( stdout, "学号不能为空。\n" );
                err = 1;
                Free(1,idp);
            }
            else if( !( (*idp)[10]==0 && (*idp)[9]!=0 ) ){
                fprintf( stdout, "学号只能为10位。\n" );
                err = 1;
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
    int err = 1;
    while(err){
        err = inputs( "名字", 100, np );
        if( err==1 ){
            Free(1,np);
            return -1;
        }
        if( err==-1 ){
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
    int err = 1;
    while(err){
        err = inputs( "密码", 100, pp );
        if( err==1 ){
            Free(1,pp);
            return -1;
        }
        if( err==0 ){
            if( (*pp)==NULL ){
                fprintf( stdout, "密码不能为空。\n" );
                err = -1;
                Free(1,pp);
            }
            else if( (*pp)[5]==0 ){
                fprintf( stdout, "密码应长于6位。\n" );
                err = -1;
                Free(1,pp);
            }
        }
    }
    return 0;
}

// 输出sqlite3_exec错误的函数
/*
   rc 为exec返回的整数, ErrMsg 为exec最后的参数, fail 为失败时的输出, success 为成功时的输出
   return: 错误时return err, 正确时return true
*/
int exec_report( int rc, char *ErrMsg, char *fail, char *success, int err, int true ){
    if( rc != SQLITE_OK ){
      fprintf(stderr, "%s: %s\n", fail, ErrMsg);
      printf( "err = %d\n", rc );
      sqlite3_free(ErrMsg);
      return err;
    }
    else{
      fprintf(stdout,"%s\n", success );
      return true;
    }
}

// 输出sqlite3_mprintf错误的函数
/*
   s为sqlite3_mprintf返回值, errmsg为自己输入的报错字符串, 错误时return err, 正确时return true
*/
int mprintf_report( char *s, const char *errmsg, int err, int true ){
    if( s == NULL ){
        sqlite3_free( s );
        printf( "%s\n", errmsg );
        return err;
    }
    return true;
}

// 输入文本
int input_text(const char *guide , int Limits , char **txt) {
//  fprintf( stderr, "%s(0: null, e: exit): ", guide);
	int err = 1;
	while(err) {
		err = inputs(guide , Limits , txt);
		if(err == 1) {
			Free(1 , txt);
			return 1;
		}
		if(err == -1) {
			Free(1 , txt);
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
