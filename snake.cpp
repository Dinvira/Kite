/*************************************************************头文件*************************************************************************************************/
#include<stdio.h>
#include<stdlib.h>
#include<Windows.h>
#include<time.h>
#include<conio.h>
/*************************************************************函数声明***************************************************************************************************/
void Pos(int x,int y);                                      //光标位置设定
void Game_area();                                           //打印游戏区域
void initSnake();                                           //蛇身的初始化
void createFood();                                          //创建食物
char reDirection();                                         //识别方向
int snakeMove();                                            //蛇的移动
int crossWall();                                            //不可以穿墙
int eatSelf();                                              //不可以吃自己
void Pause();                                               //暂停游戏
void Welcometogame();                                       //开始界面


/*************************************************************结构体***************************************************************************************************/
typedef struct Snake                                        //相当于蛇一个节点
{
	int x;                                                  //横坐标
	int y;                                                  //纵坐标
	struct Snake  *next;
}snake;

/*************************************************************全局变量***************************************************************************************************/
snake* head;                                                //蛇头指针
snake* p;                                                   //遍历蛇的时候用的指针
snake* food_1;                                              //食物指针
int	score = 0, add = 10;                                    //总分与一个食物的分数
char status = 'L';                                          //初始化蛇的方向是向左
int leap = 0;                                               //用来标志游戏是否结束，1代表蛇死了
int endleap = 0;                                            //结束标志，1就是结束
int sleepTime = 500;                                        //时间间隔

/*************************************************************自定义函数***************************************************************************************************/
void Pos(int x,int y)
{
	COORD pos;                                              //表示一个字符在控制台屏幕上的坐标，左上角(0,0)
	HANDLE hOutput;
	pos.X = x;
	pos.Y = y;
	hOutput = GetStdHandle(STD_OUTPUT_HANDLE);              //返回标准的输入、输出或错误的设备的句柄，也就是获得输入、输出/错误的屏幕缓冲区的句柄
	SetConsoleCursorPosition(hOutput, pos);
}
void Welcometogame()                                        //开始界面
{
	Pos(40, 12);
	printf("欢迎来到贪吃蛇游戏！");
	Pos(40, 20);
	printf("作者：吴棋涵\n");
	system("pause");
	system("cls");
}
void Game_area() 
{
	int i;
	for ( i = 0; i <= 60; i+=2)                             //方块水平方向占两个单位
	{
		Pos(i, 0);                 
		printf("▇");                                       //上行
		Pos(i, 26);
		printf("▇");                                       //下行
	}
	for ( i = 0; i <=25; i++)                               //方块垂直方向占1个单位
	{
		Pos(0, i);                                          //左列
		printf("▇");
		Pos(60, i);                                         //右列
		printf("▇");
	}
}
void initSnake()                                            //蛇身初始化，给定一个长度，用结构体表示是蛇的骨架，真正要显示出来是打印
{
	int i;
	snake* tail;                                            //尾指针
	tail = (snake*)malloc(sizeof(snake));                   //第一个节点/头结点
	tail->x = 30;                                           //2的倍数，因为方块的长是两个单位
	tail->y = 10;                                           //1个单位
	tail->next = NULL;
	for (i = 1; i <= 4; i++)                               //尾插法
	{
		head = (snake*)malloc(sizeof(snake));               //申请一个节点 
		head->next = tail;									//连接成链
		head->x = 30 - 2 * i;                               //下一个节点的位置
		head->y = 10;
	    tail = head;
	}
	while (tail != NULL)                                      //遍历打印出来
	{
		Pos(tail->x, tail->y);
		printf("▇");
		tail = tail->next;
	}
}
void Pause()                                                //判断是否暂停游戏
{
	while (1)
	{
		Sleep(300);
		if (GetAsyncKeyState(VK_SPACE))
		{
			break;
		}
	}
}
void createFood()                                           //创建食物
{
	snake* food;
	food = (snake*)malloc(sizeof(snake));
	srand((unsigned int)time(NULL));                        //随着时间变化，产生不一样种子，就会得到没规律的食物
	while (food->x%2!=0)                                    /*保证其为偶数，且与🐍head对齐*/
	{
		food->x = rand() % 56 + 2;
	}
	food->y = rand() % 23 + 1;                               /*确认食物不会出现在墙壁里面*/
	p = head;
	while (p->next!=NULL)                                   //解决食物出现在蛇本身的问题
	{
		if (food->x == p->x && food->y == p->y)
		{
			free(food);
			createFood();
		}
		p = p->next;
	}
	Pos(food->x,food->y);
	food_1 = food;                                          //food1用来标记的作用
	printf("▇");                                           
	Pos(64, 11);                                            //游戏介绍
	printf("不能穿墙，不能咬到自己\n");
	Pos(64, 13);
	printf("分别用↑.↓.←.→控制蛇的移动\n");
	Pos(64, 15);
	printf("F7是加速，F8是减速,加速将能得到更高的分数\n");
	Pos(64, 17);
	printf("SPACE:暂停游戏\n");
	Pos(70,20);                                               //解决有光标闪烁的办法
	printf("您的分数是：%d",score);
}
char reDirection()
{
	    if (GetAsyncKeyState(VK_F7))
	    {
		    if (sleepTime >= 100)                                //加速,最多减到100
		    {
			    sleepTime -= 50;                               //间隔每次减50
			    add++;                                         //每加速一次食物加一分
			    if (sleepTime == 750)
			    {
				    add = 2;                                   //防止减到1之后再加回来有错
			    }
		    }
	    }
		if (GetAsyncKeyState(VK_F8))
		{
			if (sleepTime < 800)                                 //同理减速，最多加到800
			{
				sleepTime += 50;                               //间隔每次加50
				add--;                                         //每加速一次食物减一分
				if (sleepTime == 800)
				{
					add = 1;                                   //保证最低分为1
				}
			}
		}
		if (GetAsyncKeyState(VK_UP) && status != 'D')
		{
			status = 'U';
		}
		else if (GetAsyncKeyState(VK_LEFT) && status != 'R')
		{
			status = 'L';
		}
		else if (GetAsyncKeyState(VK_DOWN) && status != 'U')
		{
			status = 'D';
		}
		else if (GetAsyncKeyState(VK_RIGHT) && status != 'L')
		{
			status = 'R';
		}
		else if (GetAsyncKeyState(VK_SPACE) && status != 'L')
		{
			Pause();
		}
		return status;
}
int snakeMove()
{
	snake* nexthead;
	nexthead = (snake*)malloc(sizeof(snake));
	if (status=='R')                                        //向右走
	{
		nexthead->x = head->x + 2;
		nexthead->y = head->y;
		if (nexthead->x == food_1->x&&nexthead->y==food_1->y)  //吃掉了食物
		{
			nexthead->next = head;
			head = nexthead;
			p = head;                                       //p用来从头遍历，打印方块
			while (p!=NULL)
			{
				Pos(p->x, p->y);
				printf("▇");
				p = p->next;
			}
			score = score + add;
			createFood();                                   //吃掉了食物得重新创造一个新的食物
		}
		else                                                //没有食物
		{
			nexthead->next = head;
			head = nexthead;
			p = head;                                       //p用来从头遍历，打印方块
			while (p->next->next!= NULL)
			{
				Pos(p->x, p->y);
				printf("▇");
				p = p->next;
			}
			Pos(p->next->x,p->next->y);
			printf(" ");                                    //会带来一个光标闪烁
			Pos(70,20);                                     //解决办法
			printf("您的分数是：%d",score);
			free(p->next);
			p->next = NULL;
		}
	}
	if (status == 'L')//向左走
	{
		nexthead->x = head->x - 2;
		nexthead->y = head->y;
		if (nexthead->x == food_1->x && nexthead->y == food_1->y)  //吃掉了食物
		{
			nexthead->next = head;
			head = nexthead;
			p = head;                                       //p用来从头遍历，打印方块
			while (p != NULL)
			{
				Pos(p->x, p->y);
				printf("▇");
				p = p->next;
			}
			score = score + add;
			createFood();                                   //吃掉了食物得重新创造一个新的食物
		}
		else                                                //没有食物
		{
			nexthead->next = head;
			head = nexthead;
			p = head;                                       //p用来从头遍历，打印方块
			while (p->next->next!=NULL)
			{
				Pos(p->x, p->y);
				printf("▇");
				p = p->next;
			}
			Pos(p->next->x, p->next->y);
			printf(" ");
			Pos(70, 20);
			printf("您的分数是：%d", score);
			free(p->next);
			p->next = NULL;
		}
	}
	if (status == 'U')                                      //向上走
	{
		nexthead->x = head->x;
		nexthead->y = head->y - 1;
		if (nexthead->x == food_1->x && nexthead->y == food_1->y)          //吃掉了食物
		{
			nexthead->next = head;
			head = nexthead;
			p = head;                                       //p用来从头遍历，打印方块
			while (p != NULL)
			{
				Pos(p->x, p->y);
				printf("▇");
				p = p->next;
			}
			score = score + add;
			createFood();                                   //吃掉了食物得重新创造一个新的食物
		}
		else                                                //没有食物
		{
			nexthead->next = head;
			head = nexthead;
			p = head;                                       //p用来从头遍历，打印方块
			while (p->next->next != NULL)
			{
				Pos(p->x, p->y);
				printf("▇");
				p = p->next;
			}
			Pos(p->next->x, p->next->y);
			printf(" ");
			Pos(70, 20);
			printf("您的分数是：%d", score);
			free(p->next);
			p->next = NULL;
		}
	}
	if (status == 'D')                                      //向下走
	{
		nexthead->x = head->x;
		nexthead->y = head->y + 1;
		if (nexthead->x == food_1->x && nexthead->y == food_1->y)         //吃掉了食物
		{
			nexthead->next = head;
			head = nexthead;
			p = head;                                          //p用来从头遍历，打印方块
			while (p != NULL)
			{
				Pos(p->x, p->y);
				printf("▇");
				p = p->next;
			}
			score = score + add;
			createFood();                                   //吃掉了食物得重新创造一个新的食物
		}
		else                                                //没有食物
		{
			nexthead->next = head;
			head = nexthead;
			p = head;                                       //p用来从头遍历，打印方块
			while (p->next->next!=NULL)
			{
				Pos(p->x, p->y);
				printf("▇");
				p = p->next;
			}
			Pos(p->next->x, p->next->y);
			printf(" ");
			Pos(70, 20);
			printf("您的分数是：%d", score);
			free(p->next);
			p->next = NULL;
		}
	}
	Sleep(sleepTime);                                          //蛇移动的速度，里面是毫秒，越大速度越慢
	status = reDirection();                                    //先判断一下方向
	if (crossWall()==1||eatSelf()==1)                          //判断是否撞墙或撞到自己
	{
		endleap = 1;                                           
	}
	return endleap;
}
int crossWall()                                             //判断蛇是否撞到墙
{
	if (head->x == 0 ||head->y==0||head->x==60||head->y==25)
	{
		leap = 1;
	}
	return leap;
}
int eatSelf()                                                 //判断是否咬到了自己
{
	snake* q;
	q = head->next;                                         //p用来从头遍历，打印方块
	while (q!=NULL)
	{
		if (q->x==head->x&&q->y==head->y)
		{
			leap = 1;
		}
		q = q->next;
	}
	return leap;
}

	
/*************************************************************主函数***************************************************************************************************/
int main()
{
	Welcometogame();                                        //开始界面
	Game_area();                                            //打印游戏区域
	initSnake();                                            //蛇身的初始化
	createFood();                                           //创建食物
	while (1)                                               //死循环，让蛇一直动起来，直至蛇撞到墙壁或自己
	{
		if (snakeMove()==1)                                 //判断是否结束
		{
			Pos(70, 23);
			printf("你的蛇死了");
			system("pause");                                //用来暂停
			Pos(70,24);
			break;
		}
	}
	printf("是否继续游戏，Y or N:");                        //是否继续游戏
	if (_getch() == 'Y' || _getch() == 'y')                 //重新开始游戏
	{
		status = 'L';                                       //初始方向的状态
		score = 0;                                          //分数
		add = 10;                                           //一个食物的分
		leap = 0;                                           //用来标志游戏是否结束，1代表蛇死了
		endleap = 0;                                        //结束标志，1就是结束
		sleepTime = 500;
		system("cls");                                      //清理屏幕
		main();                                             //自己调用自己
	}
	if (_getch()=='N'||_getch()=='n')                       //不继续              
	{
		Pos(70, 25);
		exit(0);                                            //退出程序
	}
	return 0;
}
