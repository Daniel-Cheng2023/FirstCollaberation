// 查询权限函数
int get_authority_new( int p , int o ){
    int i, a;
    char *s;

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
            if( (user.departments[o][i]).class!=NULL ){
                printf( "%d. %-34s%-6s权限: %-12s\n", i+1, (user.departments[o][i]).name, (user.departments[o][i]).class, s );
            }
            else{
                printf( "%d. %-40s权限: %-12s\n", i+1, (user.departments[o][i]).name, s );
            }
        }
        if( p==1 ){
            printf( "%d. %-40s权限: %-12s\n", i+1, (user.courses[o][i]).name, s );
        }
        if( p==2 ){
            printf( "%d. %-40s权限: %-12s\n", i+1, (user.clubs[o][i]).name, s );
        }
    }
    return 0;
}

//回调函数 
int callback(void *data, int argc, char **argv, char **azColName) {
   	int i;
// 	fprintf(stdout , "%s", (const char *)data);
   	for(i = 0; i < argc; i++){
      printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
   	}
   printf("\n");
   return 0;
}

//回调ID
int callback_id(void *data , int argc , char **argv , char **azColName) {
	//do sth.
	int *p; p = (int*) data;
	(*p) = atoi(argv[0]);
	return 0;
}

int Save( char *Title , char *Text , char *Announcer_name, char *announcer_id ) {
	char *ErrMsg = NULL , *ErrNum = NULL;
    int nw, nw2, cur = 0; //nw 表示异常状态 ErrMsg 为抛出的异常, cur 为发送的这条消息在 NOTICES 中的 ID
	const char *op = "insert into NOTICES(title, content, announcer_name, announcer_id) values(%Q , %Q , %Q, %Q)";
	char *sql = sqlite3_mprintf(op , Title , Text , Announcer_name, announcer_id );  //操作
	Free( 2, &Title, &Text );
	nw = sqlite3_exec(db , sql , callback , 0 , &ErrMsg); 
	if(nw != SQLITE_OK) {
		fprintf(stderr , "保存失败1\n");
		sqlite3_free(ErrMsg);
		return -1;
	}
	else {
	//	fprintf(stdout , "保存成功\n");
		char *sql2 = "select max(rowid) from NOTICES";
		int *p = &cur;
		nw2 = sqlite3_exec(db , sql2, callback_id , p , &ErrNum);
		if(nw2 != SQLITE_OK) {
			fprintf(stderr , "保存失败2\n");	
			sqlite3_free(ErrNum); 
			return -1;
		}
		fprintf(stdout , "保存成功\n");
		return cur;
	}
}

//输入文本(参数为名字)
int Init(char *NAME, char *ID) {
	char *Title = NULL , *Text = NULL; 
	input_text("请输入标题:\n", 60 , &Title);
	input_text("请输入正文:\n", 2000 , &Text);
	return Save( Title , Text , NAME, ID);
} 

//社团中选择成员
int Choose_1( int Id ) {
	char *ErrMsg = NULL, *Errsnd = NULL , *sql = NULL;
    int nw;
	const char *data = "社团: "; char *ID = NULL;
	get_authority_new( 2, 1 );
	int cnt = 10;
	for(;cnt > 0;cnt--) {
		input_text("请输入你想要发送信息的社团ID", 5 , &ID );
		if( ID[0] == 'e' ){
			break;
		}
		else {
			int Real_ID = atoi(ID);
			const char *OP = "insert into NOTICE_CLUB(club_ID , ID) values(%Q , %Q)";
			char *sql = sqlite3_mprintf( OP , Real_ID , Id);
			int NW = sqlite3_exec(db , sql , callback , 0 , &Errsnd);
			if(NW != SQLITE_OK) {
				fprintf(stderr , "发送失败\n");
				continue;
			}
			else {
				fprintf(stdout , "发送成功\n");
				return 0;
			}
		}
	}
	fprintf(stderr , "连接超时\n");
	return -1;
}

// 学院中选择成员
int Choose_2( int Id ) {
	char *ErrMsg = NULL, *Errsnd = NULL , *sql = NULL;
    int nw;
	const char *data = "学院: "; char *ID_1 = NULL , *ID_2 = NULL; //1 学院 2 班级
	printf("\n你在如下组织担任发送者: \n");
	get_authority_new( 0, 1 );
	int cnt = 10;
	for(;cnt > 0;cnt--) {
		input_text("\n请输入你想要发送信息的学院ID", 5  , &ID_1);
		if(ID_1[0] == 'e') break;
 		input_text("请输入你想要发送信息的班级ID", 5 , &ID_2);
		if(ID_2[0] == 'e') break;
		else {
			int Real_ID_1 = atoi(ID_1);
			int Real_ID_2 = atoi(ID_2);
			char *OP = "insert into NOTICE_DEPARTMENT(department_ID, ID , class) values(%Q , %d , %Q);";
			sql = sqlite3_mprintf(OP , ID_1 , Id , ID_2);
//			printf("%s\n",sql);
			int NW = sqlite3_exec(db , sql , callback , 0 , &Errsnd );
			if(NW != SQLITE_OK) {
				fprintf(stderr , "发送失败\n%s\n", Errsnd);
				sqlite3_free(Errsnd);
				continue;
			}
			else {
				fprintf(stdout , "发送成功\n");
				return 0;
			}
		}
	}
	fprintf(stderr , "连接超时\n");
	return -1;
}

//课程中选择成员
int Choose_3( int Id ) {
	char *ErrMsg = NULL, *Errsnd = NULL , *sql = NULL;
    int nw;
	const char *data = "课程: "; char *ID = NULL;
	get_authority_new( 1, 1 );
	int cnt = 10;
	for(;cnt > 0;cnt--) {
		input_text("请输入你想要发送信息的课程ID", 5 , &ID );
		if( ID[0] == 'e' ){
			break;
		}
		else {
			int Real_ID = atoi(ID);
			const char *OP = "insert into NOTICE_COURSE(club_ID , ID) values(%Q , %Q)";
			char *sql = sqlite3_mprintf( OP , Real_ID , Id);
			int NW = sqlite3_exec(db , sql , callback , 0 , &Errsnd);
			if(NW != SQLITE_OK) {
				fprintf(stderr , "发送失败\n");
				continue;
			}
			else {
				fprintf(stdout , "发送成功\n");
				return 0;
			}
		}
	}
	fprintf(stderr , "连接超时\n");
	return -1;
}

//发送的主过程
int Send( char *NAME, char *ID ) {
	int Num = 0, err=-1;
	char type = 0;

	while( type!='0' && type!='1' && type!='2' ){
		printf( "\n请选择发送通知给哪些组织(0: 院系, 1: 社团, 2: 课程, e: 返回菜单):" );
		type = getchar();
		clear_input_buffer();
		if(type=='e'){
			return -1;
		}
	}
	while(1) {
		++Num; //防止卡死
		if(Num == 100) {
			fprintf(stderr , "错误次数过多\n");
			break;
		}
		int tmp = 0; //暂存返回结果
		tmp = Init(NAME, ID);
		if(tmp == -1){
			continue;
		}
		if(type == '1') {
			if(Choose_1( tmp) == -1){
				continue;
			}
			else{
				break;
			}
		}
		if(type == '0') {
			if(Choose_2( tmp) == -1){
				continue;
			}
			else break;
		}
		if(type == '2'){
			if(Choose_3(tmp) == -1){
				continue;
			}
			else{
				break;
			}
		}
	}
	return 0;
}