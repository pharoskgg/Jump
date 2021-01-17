#include<stdio.h>
#include<stdlib.h> 
#include<windows.h>
#include<conio.h>
#include<string.h>
#include<cctype>
#include<time.h> 

//������ɫ
#define TOP_COLOR	 0x34	//�������ɫ 
#define EDGE_COLOR	 0x22	//�����߽����ɫ 
#define BLANK_COLOR	 0x00	//�հ�����ɫ 
#define FIRST_COLOR	 0x88	//ʵ����ɫ 			��ɫ 
#define SECOND_COLOR 0x44	//�̵���ɫ  		��ɫ 
#define THIRD_COLOR  0xdd	//�ɷ�ת����ɫ

#define N 100//������;
 
//Ԥ����߽��С
#define HIGH 31
#define WIDE 20
#define WX (WIDE+10)*2//����̨��ʵ��� 
#define WY HIGH 

//����ש������� 
typedef struct Floor
{
	int y;		//��һ��ש��λ�� 
	int x;		
	int type;	//ש�������
}Floor;

//ȫ�ֱ���
Floor floor[10];		//һ��ͼ���ж��ٿ�ذ�		1
int map[N][N];			//���ͼ��ֵ 				1
int man_x,man_y;		//����λ�� 					0
int mandownspeed=200; 	//���������ٶ� 				0
int floorupspeed=500;	//�ذ������ٶ�				0
int speedkey=200;       //������� 
int X,Y;				//��ͼ��С 					1
int lasttime,lasttime2;
int lasttime_getkey;//��ȡָ���ʱ���� 
int live=6;//��������ֵ
int score=0;//��ҵ÷� 


//��ͼ����
void DrawEdge();	//�����߽� 						1
void DrawFloor();	//���ذ�						1
void DrawMap();		//								1
void DrawMenue();	//����ʼ������Ϣ				1
void DrawText(int i,int j,unsigned short c,char s[N]);//ָ��λ�û���ָ����Ϣ 		1
void DrawIt(int i,int j);//����ָ��λ��������Ķ���	1
void DrawMan();//�����ﻭ���� 
void DrawInfo();//�����÷ֺ�����ֵ��Ϣ 
void DrawHelp();//������Ϣ 
 
 //ʵ�ú���
void GameInit();	//��ʼ��						1
void InsertMap();	//���ذ���ֵ�������� 			1
void ReadMap();		//��ȡ��ʼ��ͼ����				1
void PlayGame(); 	//								1
void CreateMan();	//��ʼ������λ��				1
void MoveMan(int ny,int nx);
void DownMan();//���������� 
void ActMap();//�õذ嶯���� 

//��⺯��
int HurtTop();//��ʱδ�� 
int HurtFloor();//��ʱδ�� 


//��������
void Gotoxy(int i,int j);//������� 				1
void SetColor(unsigned short c);//�����ı���ɫ 		1


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
				//DrawText(15,4,9,"��������˳�");
				Gotoxy(15,7);
				SetColor(9);
				printf("Game Over...��������˳�");
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
				printf("������˼��������ʱû��,��������������˵�");
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

	HANDLE app=GetStdHandle(STD_OUTPUT_HANDLE);//���ع�� 
	CONSOLE_CURSOR_INFO ci;
	GetConsoleCursorInfo(app,&ci);
	ci.bVisible=false;
	SetConsoleCursorInfo(app,&ci);
	SetConsoleTitle("Jump");//���ñ��� 
	
	COORD size={WX+50,WY};//���ÿ���̨��С 
	SetConsoleScreenBufferSize(app,size);
	SMALL_RECT rc={0,0,WX,WY};
	SetConsoleWindowInfo(app,1,&rc);
	lasttime_getkey=lasttime2=lasttime=clock();
	//��ʼ�ذ����ݣ�
	for(int i=0;i<10;i++)
	{
		if(i==4)//ʹ��5��ذ�һ��Ϊ��ɫ�ذ壬��Ϊ��Ҫ�ڴ˵ذ��� 
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
		if(kbhit() && (now-lasttime_getkey>speedkey))//����Ƿ��а����з���1�޷���0
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
					//DrawText(2,2,4,"Enter any key to continue");Ϊʲô��������������������������� 
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
		int flag=0;//����Ƿ��ж������ʧ  1Ϊ�еذ���ʧ  0Ϊû�еذ���ʧ 
		for(int i=0;i<10;i++)
		{
			floor[i].y--;//��i��ذ�����
			
			//�ж��˺���ӷ���Downman()������� 
			if(floor[i].y==man_y+1 && floor[i].type==2 && (floor[i].x==man_x || floor[i].x==man_x-1 ||floor[i].x==man_x-2 ||floor[i].x==man_x-3))
				live--;	
			if(floor[i].y==man_y+1 && floor[i].type==1 && (floor[i].x==man_x || floor[i].x==man_x-1 ||floor[i].x==man_x-2 ||floor[i].x==man_x-3))
				score+=10;		
					
			if(floor[i].y==0)//�ذ����������� 
			{	
				for(int j=0;j<4;j++)
					map[floor[i].y+1][floor[i].x+j]-=map[floor[i].y+1][floor[i].x+j];//����ԭ���ذ��ڵ�ͼ���ֵ
				floor[i].y=HIGH-1;//�ذ����õ��ײ� 
				floor[i].x=rand()%(WIDE-5)+1;//���õ�һ��ש��λ�� 
				floor[i].type=rand()%3+1;//�������ɵذ����� 
				flag=1;//��־����ĺ����������� 
			}		
			if(floor[i].y==man_y && (floor[i].x==man_x || floor[i].x==man_x-1 ||floor[i].x==man_x-2 ||floor[i].x==man_x-3 ))
			{//�ҳ������ڵĵذ��ϣ�û�ڵذ�����ִ�д��д��� 
				man_y--;
				
				if(man_y==0)//���������� 
				{
					man_y+=2;
					map[man_y][man_x]+=6;//��λ�ø�ֵ 
					DrawIt(man_y,man_x);
					map[1][man_x]-=6;//��λ�����ݲ��� 
					DrawIt(1,man_x);
					live--;
				}
							
				else
				{
					map[man_y][man_x]+=6;
					map[man_y+1][man_x]-=6;//������ԭ���ط���ֵ 					
				}
			} 
			
			if(flag==0)//����ʧ�ذ��򲻽����µĵذ帳ֵ��������� 
				for(int j=0;j<4;j++)
				{
					map[floor[i].y][floor[i].x+j]+=map[floor[i].y+1][floor[i].x+j];//Ϊ�ذ����λ�ø�ֵ 
					map[floor[i].y+1][floor[i].x+j]-=map[floor[i].y+1][floor[i].x+j];//����ԭ���ذ��ڵ�ͼ���ֵ
				} 
			else if(flag==1)
				flag=0;//���ñ�־ʹ������������ 
			
		}
		InsertMap(); //���ƶ���ĵذ�������ɵĵذ����ֵ�����ͼ������ 
		DrawFloor();//���»����ذ� 
		lasttime=clock();//��¼������ʱ�� 
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
			DrawIt(man_y,man_x);//����ԭ������ط���ͼ�� 
			man_y++;
			map[man_y][man_x]+=6;
			Gotoxy(man_y,man_x);
			DrawIt(man_y,man_x); //��������ط���ͼ�� 
			if(map[man_y+1][man_x]==2)//�䵽��ɫ�ذ����յ��˺� 
				live--;	
			else if(map[man_y+1][man_x]==1)	//�䵽��ɫ�ذ����յ��˺� 
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
	for(int i=1;i<WIDE;i++)		//�����������˺���ǽ 
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
		printf("��");
	}
	if(map[i][j]==9)
	{
		SetColor(TOP_COLOR);
		printf("��"); 		
	}	
	if(map[i][j]==8)
	{
		SetColor(EDGE_COLOR);
		printf("��"); 
	}
	if(map[i][j]==0)
	{
		SetColor(BLANK_COLOR);
		printf("  "); 
	}
	if(map[i][j]==1)
	{
		SetColor(FIRST_COLOR);
		printf("��"); 
	}
	if(map[i][j]==2)
	{
		SetColor(SECOND_COLOR);
		printf("��"); 
	}
	if(map[i][j]==3)
	{
		SetColor(THIRD_COLOR);
		printf("��"); 
	}
}

void DrawMenue()
{
	int i=8; 
	int j=10; 
	int c=7;
	DrawText(i-5,j,c-3,"�������ٹ�������");
	DrawText(i+0,j,c,"1:��ʼ��Ϸ");
	DrawText(i+1,j,c,"2:�鿴����");
	DrawText(i+2,j,c,"3:�鿴����");	
	DrawText(i+3,j,c,"4:�˳���Ϸ");
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
	DrawText(i,j  ,c,"a :�����ƶ�һ��");
	DrawText(i+1,j,c,"d :�����ƶ�һ��");
	DrawText(i+2,j,c,"���ں�ɫ�ذ��Ͽ�һ������ֵ");
	DrawText(i+3,j,c,"����������һ������ֵ������ֵΪ0ʱ������Ϸ����������ײ�");
	DrawText(i+4,j,c,"�䵽��ɫ�ذ��ϼ�10�֣����ڷ�ɫ�ذ���0.5��󣬷۵ذ���ʧ");
	DrawText(i+5,j,c,"��������������˵�");
}

void CreateMan()
{
	man_y=floor[4].y-1;
	man_x=floor[4].x;//����λ�� 
	map[man_y][man_x]=6;//������ֵ 
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

