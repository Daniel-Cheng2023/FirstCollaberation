#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sqlite3.h"
#include "new_input.c"
#include "login.c"

//回调函数 
static int callback(void *data, int argc, char **argv, char **azColName) {
   int i;
// fprintf(stdout , "%s", (const char *)data);
   for(i = 0; i < argc; i++){
      printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
   }
   printf("\n");
   return 0;
}

//回调ID
static int callback_id(int *data , int argc , char **argv , char **azColName) {
	int i;
	return 0;
}

//输入文本(参数为名字)
inline int Init(sqlite3 *db,char *NAME) {
	char *Title = NULL , *Text = NULL; 
	input_text("请输入标题:\n", 60 , &Title);
	input_text("请输入正文\n:", 200 , &Text);
	return Save(db , Title , Text , NAME);
} 

inline int Save(sqlite3 *db , char *Title , char *Text , char *Announcer) {
	char *ErrMsg = NULL , *ErrNum = NULL, *nw = NULL , *nw2 = NULL;int cur = 0; //nw 表示异常状态 ErrMsg 为抛出的异常, cur 为发送的这条消息在 NOTICES 中的 ID
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


//选择成员
inline int Choose(sqlite3 *db , int Id) {
	char *nw = NULL, *ErrMsg = NULL, *Errsnd = NULL , *sql = NULL;
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
		input_text("请输入你想要发送信息的社团ID(输入e)退出", 5 , ID);
		if(ID[0] == 'e') break;
		else {
			int Real_ID = atoi(ID);
			const char *OP = "insert into NOTICE_CLUB(club_ID , ID) , value(%Q , %Q)";
			char *SQL = sqlite3_mprintf(op , Id , Real_ID);
			char *NW = sqlite3_exec(db , sql , callback , 0 , &Errsnd);
			if(NW != SQLITE_OK) {
				fprintf(stderr , "发送失败\n");
				continue;
			}
			else {
				fprintf(stdout , "发送成功\n");
				break;
			}
		}
	}
	return 0;
}

//发送的主过程
inline int Send(sqlite3 *db , char *NAME) {
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
		if(Choose(db , tmp) == -1) continue;
	}
	return 0 ;
}

/*
int main(int argc,char *argv[]){
    char p;

    return 0;
}
*/
