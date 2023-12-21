//回调函数 
int callback(void *data, int argc, char **argv, char **azColName) {
   int i;
// fprintf(stdout , "%s", (const char *)data);
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

inline int Save(sqlite3 *db , char *Title , char *Text , char *Announcer) {
	char *ErrMsg = NULL , *ErrNum = NULL;
    int nw, nw2, cur = 0; //nw 表示异常状态 ErrMsg 为抛出的异常, cur 为发送的这条消息在 NOTICES 中的 ID
	const char *op = "insert into NOTICES(title, content, announcer) values(%Q , %Q , %Q)";
	char *sql = sqlite3_mprintf(op , Title , Text , Announcer);  //操作
	nw = sqlite3_exec(db , sql , callback , 0 , &ErrMsg); 
	if(nw != SQLITE_OK) {
		fprintf(stderr , "保存失败\n");
		sqlite3_free(ErrMsg);
		return -1;
	}
	else {
	//	fprintf(stdout , "保存成功\n");
		char *sql2 = "select max(rowid) from NOTICES";
		int *p = &cur;
		nw2 = sqlite3_exec(db , sql2, callback_id , p , &ErrNum);
		if(nw2 != SQLITE_OK) {
			fprintf(stderr , "保存失败\n");	
			sqlite3_free(ErrNum); 
			return -1;
		}
		fprintf(stdout , "保存成功\n");
		return cur;
	}
}

//输入文本(参数为名字)
inline int Init(sqlite3 *db,char *NAME) {
	char *Title = NULL , *Text = NULL; 
	input_text("请输入标题:\n", 60 , &Title);
	input_text("请输入正文\n:", 200 , &Text);
	return Save(db , Title , Text , NAME);
} 

//社团中选择成员
inline int Choose_1(sqlite3 *db , int Id) {
	char *ErrMsg = NULL, *Errsnd = NULL , *sql = NULL;
    int nw;
	const char *data = "社团: "; char *ID = NULL;
	const char *op = "select * from CLUBS";
	nw = sqlite3_exec(db , sql , callback , (void *)data , &ErrMsg);
	if(nw != SQLITE_OK) {
		fprintf(stderr , "获取失败\n");
		sqlite3_free(ErrMsg);
		return -1;
	}
	else fprintf(stdout , "获取成功\n");
	int cnt = 10;
	for(;cnt > 0;cnt--) {
		input_text("请输入你想要发送信息的社团ID(e: 退出)", 5 , &ID );
		if( ID[0] == 'e' ){
			break;
		}
		else {
			int Real_ID = atoi(ID);
			const char *OP = "insert into NOTICE_CLUB(club_ID , ID) , value(%Q , %Q)";
			char *sql = sqlite3_mprintf( op , Real_ID , Id);
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

inline int Choose_2(sqlite3 *db , int Id) {
	char *ErrMsg = NULL, *Errsnd = NULL , *sql = NULL;
    int nw;
	const char *data = "学院: "; char *ID_1 = NULL , *ID_2 = NULL; //1 学院 2 班级
	const char *op = "select * from CLUBS";
	nw = sqlite3_exec(db , sql , callback , (void *)data , &ErrMsg);
	if(nw != SQLITE_OK) {
		fprintf(stderr , "获取失败\n");
		sqlite3_free(ErrMsg);
		return -1;
	}
	else fprintf(stdout , "获取成功\n");
	int cnt = 10;
	for(;cnt > 0;cnt--) {
		input_text("请输入你想要发送信息的学院ID(e: 退出)", 5  , &ID_1);
		if(ID_1[0] == 'e') break;
 		input_text("请输入你想要发送信息的班级ID(e: 退出)", 5 , &ID_2);
		if(ID_2[0] == 'e') break;
		else {
			int Real_ID_1 = atoi(ID_1);
			int Real_ID_2 = atoi(ID_2);
			const char *OP = "insert into NOTICE_DEPARTMENT(department_ID, ID , class) value(%Q , %Q , %Q)";
			char *sql = sqlite3_mprintf(op , Id , Real_ID_1 , Id , Real_ID_2);
			int NW = sqlite3_exec(db , sql , callback , 0 , &Errsnd );
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
inline int Send(sqlite3 *db , char *NAME , int type) {
	int Num = 0;
	while(1) {
		++Num; //防止卡死
		if(Num == 100) {
			fprintf(stderr , "错误次数过多\n");
			break;
		}
		int tmp = 0; //暂存返回结果
		tmp = Init(db , NAME);
		if(tmp == -1) continue; 
		if(type == 1) {
			if(Choose_1(db , tmp) == -1) continue;
			else break;
		}
		if(type == 2) {
			if(Choose_2(db , tmp) == -1) continue;
			else break;
		}
	}
	return 0 ;
}

/*
int main(int argc,char *argv[]){
    char p;

    return 0;
}
*/