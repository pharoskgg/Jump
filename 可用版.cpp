#include<stdio.h>
#include<stdlib.h> 
#include<windows.h>
#include<conio.h>
#include<string.h>
#include<cctype>
#include<time.h> 

//定义颜色
#define TOP_COLOR	 0x34	//顶点的颜色 
#define EDGE_COLOR	 0x22	//两个边界的颜色 
#define BLANK_COLOR	 0x00	//空白区颜色 
#define FIRST_COLOR	 0x88	//实地颜色 			灰色 
#define SECOND_COLOR 0x44	//刺地颜色  		红色 
#define THIRD_COLOR  0xdd	//可翻转地颜色

#define N 100//各类用途
 
//预定义边界大小
#define HIGH 31
#define WIDE 20
#define WX (WIDE+10)*2//控制台真实宽度 
#define WY HIGH 

//定义砖块的类型 
typedef struct Floor
{
	int y;		//第一块砖的位置 
	int x;		
	int type;	//砖块的类型
}Floor;

//全局变量
Floor floor[10];		//一个图里有多少块地板		1
int map[N][N];			//存地图数值 				1
int man_x,man_y;		//人物位置 					0
int mandownspeed=200; 	//人物下落速度 				0
int floorupspeed=500;	//地板上升速度				0
int speedkey=200;       //按键间隔 
int X,Y;				//地图大小 					1
int lasttime,lasttime2;
int lasttime_getkey;//获取指令的时间间隔 
int live=6;//人物生命值
int score=0;//玩家得分 


//画图函数
void DrawEdge();	//画出边界 						1
void DrawFloor();	//画地板						1
void DrawMap();		//								1
void DrawMenue();	//画开始界面信息				1
void DrawText(int i,int j,unsigned short c,char s[N]);//指定位置画出指定信息 		1
void DrawIt(int i,int j);//画出指定位置所代表的东西	1
void DrawMan();//将人物画出来 
void DrawInfo();//画出得分和生命值信息 
void DrawHelp();//帮助信息 
 
 //实用函数
void GameInit();	//初始化						1
void InsertMap();	//将地板数值插入数组 			1
void ReadMap();		//读取初始地图数据				1
void PlayGame(); 	//								1
void CreateMan();	//初始化人物位置				1
void MoveMan(int ny,int nx);
void DownMan();//让人物下落 
void ActMap();//让地板动起来 

//检测函数
int HurtTop();//暂时未做 
int HurtFloor();//暂时未做 


//辅助函数
void Gotoxy(int i,int j);//定点输出 				1
void SetColor(unsigned short c);//设置文本颜色 		1


int main()
{
	char ch;
	DrawMenue();	
	GameInit();
	ReadMap();
	InsertMap();
	while(1)
	{
		ch=getch();
		switch(ch)
		{
			case '1':
				system("cls");
				DrawMap();
				PlayGame();
				DrawInfo();
				//DrawText(15,4,9,"按任意键退出");
				Gotoxy(15,7);
				SetColor(9);
				printf("Game Over...按任意键退出");
				getch();
				exit(0); 
				break;
			case '2':
				system("cls");
				DrawHelp();
				getch();
				system("cls");
				DrawMenue(); 
				break;
			case '3':
				system("cls");
				Gotoxy(5,4);
				printf("不好意思，功能暂时没做,按任意键返回主菜单");
				getch();
				system("cls");
				DrawMenue();		 
				break;
			case '4':
				exit(0);	
		}
	}

	return 0;
}

void GameInit()
{
	srand(time(0));

	HANDLE app=GetStdHandle(STD_OUTPUT_HANDLE);//隐藏光标 
	CONSOLE_CURSOR_INFO ci;
	GetConsoleCursorInfo(app,&ci);
	ci.bVisible=false;
	SetConsoleCursorInfo(app,&ci);
	SetConsoleTitle("Jump");//设置标题 
	
	COORD size={WX+50,WY};//设置控制台大小 
	SetConsoleScreenBufferSize(app,size);
	SMALL_RECT rc={0,0,WX,WY};
	SetConsoleWindowInfo(app,1,&rc);
	lasttime_getkey=lasttime2=lasttime=clock();
	//初始地板数据：
	for(int i=0;i<10;i++)
	{
		if(i==4)//使第5块地板一定为灰色地板，因为人要在此地板上 
		{
			floor[i].y=3*i+3;
			floor[i].x=rand()%(WIDE-5)+1;
			floor[i].type=1; 
			continue;
		}
		floor[i].y=3*i+3;
		floor[i].x=rand()%(WIDE-5)+1;
		floor[i].type=rand()%3+1;
	} 
	
} 

void PlayGame()
{
	char ch;
	CreateMan();
	while(1)
	{
		DrawMan();
		DrawInfo();	
		int now=clock();	
		if(kbhit() && (now-lasttime_getkey>speedkey))//检测是否有案件有返回1无返回0
		{
			int nx=man_x;
			int ny=man_y;
	
			ch=getch();
			switch(ch)
			{
				case 'a':
					nx--;				
					break;
				case 'd': 
					nx++;
					break;
				case 'p':
					//DrawText(2,2,4,"Enter any key to continue");为什么这个函数不能用其他函数都能用 
					Gotoxy(HIGH/2,2);
					SetColor(4);
					printf("Enter any key to continue");
					getch();
					system("cls");
					DrawMap();
					break;	
			}
			MoveMan(ny,nx);
			lasttime_getkey=clock();						
		}
		ActMap();
		DownMan();
		if(live<=0 || man_y>=31)
			break;
	}
}

void MoveMan(int ny,int nx)
{
	int flag=0;
	if(map[ny][nx]!=8 && map[ny][nx]!=1 && map[ny][nx]!=2 && map[ny][nx]!=3)
	{
		/*if(map[man_y+1][man_x]==0)
			flag=1;*/
		map[ny][nx]+=6;
		map[man_y][man_x]-=6; 
		DrawIt(ny,nx);
		DrawIt(man_y,man_x);
		man_x=nx;
		man_y=ny;
	/*	if(flag==1)
			for(int i=0;i<10;i++)
		{
			if(man_y+1==floor[i].y && (floor[i].x==man_x || floor[i].x+1==man_x || floor[i].x+2 || floor[i].x+3) && floor[i].type==2)
				live--;
			else if(man_y+1==floor[i].y && (floor[i].x==man_x || floor[i].x+1==man_x || floor[i].x+2 || floor[i].x+3) && floor[i].type==1)
				score+=10;
		} */
	}
}

void ActMap()
{
	int now=clock();
	if(now-lasttime>floorupspeed)
	{
		int flag=0;//标记是否有定点板消失  1为有地板消失  0为没有地板消失 
		for(int i=0;i<10;i++)
		{
			floor[i].y--;//第i块地板上升
			
			//判断伤害与加分与Downman()函数结合 
			if(floor[i].y==man_y+1 && floor[i].type==2 && (floor[i].x==man_x || floor[i].x==man_x-1 ||floor[i].x==man_x-2 ||floor[i].x==man_x-3))
				live--;	
			if(floor[i].y==man_y+1 && floor[i].type==1 && (floor[i].x==man_x || floor[i].x==man_x-1 ||floor[i].x==man_x-2 ||floor[i].x==man_x-3))
				score+=10;		
					
			if(floor[i].y==0)//地板遇顶点消除 
			{	
				for(int j=0;j<4;j++)
					map[floor[i].y+1][floor[i].x+j]-=map[floor[i].y+1][floor[i].x+j];//消除原来地板在地图里的值
				floor[i].y=HIGH-1;//地板重置到底部 
				floor[i].x=rand()%(WIDE-5)+1;//重置第一块砖的位置 
				floor[i].type=rand()%3+1;//重新生成地板类型 
				flag=1;//标志下面的函数进不进行 
			}		
			if(floor[i].y==man_y && (floor[i].x==man_x || floor[i].x==man_x-1 ||floor[i].x==man_x-2 ||floor[i].x==man_x-3 ))
			{//找出人所在的地板上，没在地板上则不执行此行代码 
				man_y--;
				
				if(man_y==0)//人遇到顶点 
				{
					man_y+=2;
					map[man_y][man_x]+=6;//新位置赋值 
					DrawIt(man_y,man_x);
					map[1][man_x]-=6;//老位置数据擦除 
					DrawIt(1,man_x);
					live--;
				}
							
				else
				{
					map[man_y][man_x]+=6;
					map[man_y+1][man_x]-=6;//消除人原来地方的值 					
				}
			} 
			
			if(flag==0)//有消失地板则不进行新的地板赋值与擦出数据 
				for(int j=0;j<4;j++)
				{
					map[floor[i].y][floor[i].x+j]+=map[floor[i].y+1][floor[i].x+j];//为地板的新位置赋值 
					map[floor[i].y+1][floor[i].x+j]-=map[floor[i].y+1][floor[i].x+j];//消除原来地板在地图里的值
				} 
			else if(flag==1)
				flag=0;//重置标志使程序正常进行 
			
		}
		InsertMap(); //将移动后的地板和新生成的地板的数值插入地图数组里 
		DrawFloor();//重新画出地板 
		lasttime=clock();//记录下最后的时间 
	}

}

void DownMan()
{
	if(map[man_y+1][man_x]==0)
	{		
		int now2=clock();
		if(now2-lasttime2>mandownspeed)
		{
			map[man_y][man_x]-=6;
			DrawIt(man_y,man_x);//擦出原来人物地方的图像 
			man_y++;
			map[man_y][man_x]+=6;
			Gotoxy(man_y,man_x);
			DrawIt(man_y,man_x); //画出人物地方的图像 
			if(map[man_y+1][man_x]==2)//落到红色地板上收到伤害 
				live--;	
			else if(map[man_y+1][man_x]==1)	//落到灰色地板上收到伤害 
				score+=10; 
			lasttime2=clock();
		}
	}
}

void InsertMap()
{
	for(int i=0;i<10;i++)
	{
		map[floor[i].y][floor[i].x+0] = floor[i].type;
		map[floor[i].y][floor[i].x+1] = floor[i].type;
		map[floor[i].y][floor[i].x+2] = floor[i].type;
		map[floor[i].y][floor[i].x+3] = floor[i].type;
	} 	
}

void ReadMap()
{
	FILE *fp=fopen("map.txt","r");
	char t[N];
	while(fscanf(fp,"%s",t)!=EOF)
	{
		X=strlen(t);
		
		for(int i=0;t[i]!='\0';i++)
			map[Y][i]=t[i]-'0'; 
			
		Y++;
	}
	fclose(fp);
}

void DrawMap()
{
	DrawEdge();
	DrawFloor();
}

void DrawEdge()
{
	for(int i=1;i<WIDE;i++)		//画出顶部有伤害的墙 
		DrawIt(0,i);
	for(int i=0;i<HIGH;i++)
	{
		DrawIt(i,0);
		DrawIt(i,WIDE-1);
	}
}

void DrawFloor()
{
/*	for(int i=1;i<Y-1;i++)
	{
		for(int j=1;j<X-1;j++)
			DrawIt(i,j); 
	}*/
	
	for(int i=0;i<10;i++)
	{
		for(int j=0;j<4;j++) 
			DrawIt(floor[i].y,floor[i].x+j);
/*		DrawIt(floor[i].y,floor[i].x+0);
		DrawIt(floor[i].y,floor[i].x+1);
		DrawIt(floor[i].y,floor[i].x+2);
		DrawIt(floor[i].y,floor[i].x+3);*/
		for(int j=0;j<4;j++)
			DrawIt(floor[i].y+1,floor[i].x+j);
/*		DrawIt(floor[i].y+1,floor[i].x+0);
		DrawIt(floor[i].y+1,floor[i].x+1);
		DrawIt(floor[i].y+1,floor[i].x+2);
		DrawIt(floor[i].y+1,floor[i].x+3);*/
	}
	for(int i=1;i<WIDE-1;i++)
		DrawIt(1,i);
} 

void DrawText(int i,int j,int c,char s[])
{
	Gotoxy(i,j);
	SetColor(c);
	printf("%s",s); 
}

void DrawIt(int i,int j)
{
	Gotoxy(i,j*2);
	if(map[i][j]==6)
	{
		SetColor(6);
		printf("♀");
	}
	if(map[i][j]==9)
	{
		SetColor(TOP_COLOR);
		printf("■"); 		
	}	
	if(map[i][j]==8)
	{
		SetColor(EDGE_COLOR);
		printf("■"); 
	}
	if(map[i][j]==0)
	{
		SetColor(BLANK_COLOR);
		printf("  "); 
	}
	if(map[i][j]==1)
	{
		SetColor(FIRST_COLOR);
		printf("■"); 
	}
	if(map[i][j]==2)
	{
		SetColor(SECOND_COLOR);
		printf("■"); 
	}
	if(map[i][j]==3)
	{
		SetColor(THIRD_COLOR);
		printf("■"); 
	}
}

void DrawMenue()
{
	int i=8; 
	int j=10; 
	int c=7;
	DrawText(i-5,j,c-3,"老铁手速够不够？");
	DrawText(i+0,j,c,"1:开始游戏");
	DrawText(i+1,j,c,"2:查看帮助");
	DrawText(i+2,j,c,"3:查看排名");	
	DrawText(i+3,j,c,"4:退出游戏");
}

void DrawInfo()
{
	char s[N];
	sprintf(s,"LIVE:%d",live);
	DrawText(5,41,2,s);
	sprintf(s,"SCORE:%d",score);
	DrawText(6,41,2,s);
}

void DrawMan()
{
	DrawIt(man_y,man_x);
}

void DrawHelp()
{
	int i=5;
	int j=3;
	int c=3;
	DrawText(i,j  ,c,"a :向左移动一格");
	DrawText(i+1,j,c,"d :向右移动一格");
	DrawText(i+2,j,c,"落在红色地板上扣一点生命值");
	DrawText(i+3,j,c,"碰到顶部扣一点生命值，生命值为0时结束游戏，或掉落至底部");
	DrawText(i+4,j,c,"落到灰色地板上加10分，落在粉色地板上0.5秒后，粉地板消失");
	DrawText(i+5,j,c,"按任意键返回主菜单");
}

void CreateMan()
{
	man_y=floor[4].y-1;
	man_x=floor[4].x;//人物位置 
	map[man_y][man_x]=6;//人物数值 
}

void SetColor(unsigned short c)
{
	HANDLE app=GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(app,c);
} 

void Gotoxy(int y,int x)
{
	HANDLE app=GetStdHandle(STD_OUTPUT_HANDLE);
	COORD ci;
	ci.X=x;
	ci.Y=y;
	SetConsoleCursorPosition(app,ci);
}

