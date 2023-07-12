#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <queue>
char start[100][100]= {
	{"#####.....#####"},
	{"#.............#"},
	{"#.###.....#####"},
	{"#.#.#.....#.#.#"},
	{"#...#..o..#...#"},
	{"#####.....#####"},
	{"..............."},
	{"#####.....#####"},
	{"#...#.....#...#"},
	{"#.#.#.....#.#.#"},
	{"#.###.....###.#"},
	{"#.............#"},
	{"#####.....#####"},
};
struct Point {
	int X, Y;
};
struct Player {
	char **mat;  // Game Data

	int row_cnt;  // Count of rows in current map

	int col_cnt;  // Count of columns in current map

	int ghost_posx[2];  // X-corordinate of 2 ghosts' position, index from 0
	int ghost_posy[2];  // Y-corordinate of 2 ghosts' position, index from 0

	int your_posx;  // X-corordinate of your AI's position, index from 0
	int your_posy;  // Y-corordinate of your AI's position, index from 0

	int opponent_posx;  // X-corordinate of opponent AI's position, index from 0
	int opponent_posy;  // Y-corordinate of opponent AI's position, index from 0

	int your_status;  // Your AI's status, -1 means died, x(x > 0) means your AI's
	// super status will sustain in x more rounds, 0 means normal status.

	int opponent_status;  // Opponent AI's status, -1 means died, x(x > 0) means
	// opponent AI's super status will sustain in x more rounds, 0 means normal status.

	int your_score;  // Your AI's score

	int opponent_score;  // Opponent AI's score
};

void pprint(Point p)
{
	printf("(%d,%d)",p.X,p.Y);
	return;
}


//your function here
int dx[4]= {-1,1,0,0};
int dy[4]= {0,0,-1,1};
int step=0;
int path[100][2];
int length[100][100];


















static const Point MOV[4] = {{-1, 0}, {1, 0}, {0, 1}, {0, -1}};
struct qNode {
	int x, y;
	int step;
};
static bool check(Point pos, Player *player)
{
	int x1 = pos.X, y1 = pos.Y;
	if (x1 >= 0 && x1 < player->row_cnt && y1 >= 0 && y1 < player->col_cnt && player->mat[x1][y1] != '#')
		return true;
	else
		return false;
}
static void bfs(Player *player, int step[100][100])
{
	std::queue<qNode> q;
	q.push({player->ghost_posx[0], player->ghost_posy[0], 0});
	q.push({player->ghost_posx[1], player->ghost_posy[1], 0});
	if(player->opponent_status>=1) {
		q.push({player->opponent_posx, player->opponent_posy, 0});
	}
	bool flag[61][61];
	memset(flag, false, sizeof(flag));
	flag[player->ghost_posx[0]][player->ghost_posy[0]] = true;
	flag[player->ghost_posx[1]][player->ghost_posy[1]] = true;
	if(player->opponent_status>=1) {
		flag[player->opponent_posx][player->opponent_posy] = true;
	}
	while (!q.empty()) {
		qNode tmp = q.front();
		q.pop();
		for (int i = 0; i < 4; i++) {
			int x1 = tmp.x + MOV[i].X, y1 = tmp.y + MOV[i].Y;
			if (check({x1, y1}, player) && !flag[x1][y1]) {
				if (tmp.step + 1 <= 5)
					q.push({x1, y1, tmp.step + 1});
				flag[x1][y1] = true;
				step[x1][y1] = tmp.step + 1;
			}
		}
	}
}


















int check(struct Player *player,int x,int y)
{
	if(x<0||y<0||x>=player->row_cnt||y>=player->col_cnt)
		return 0;
	if(player->mat[x][y]=='#')
		return 0;
	if(player->your_status==0){
		if(x==player->ghost_posx[0]&&y==player->ghost_posy[0])
			return 0;
		if(x==player->ghost_posx[1]&&y==player->ghost_posy[1])
			return 0;
		if(length[x][y]==1)
			return 0;
	}	
		return 1;
}

void bfs1(struct Player *player,int x,int y)
{
	for(int i=0; i<100; i++) {
		for(int j=0; j<100; j++) {
			length[i][j]=190000;
		}
	}
	int queue[100][2];
	int front=0,rear=0;
	queue[rear][0]=x;
	queue[rear][1]=y;
	rear++;
	length[x][y]=0;
	while(front!=rear) {
		int nowx=queue[front][0];
		int nowy=queue[front][1];
		front++;
		for(int i=0; i<4; i++) {
			int posx=nowx+dx[i];
			int posy=nowy+dy[i];
			if(check(player,posx,posy)==1) {
				queue[rear][0]=posx;
				queue[rear][1]=posy;
				rear++;
				if(length[posx][posy]>length[nowx][nowy]+1) {
					length[posx][posy]=length[nowx][nowy]+1;
				}
			}
		}
		if(length[nowx][nowy]==4) {
			break;
		}
	}

}

void init(struct Player *player)
{
	// This function will be executed at the begin of each game, only once.
}

int dfs(struct Player *player,int x,int y,int num,int sum)
{
	int max=0;
	if(num==6) {
		return sum;
	} else {
		for(int i=0; i<4; i++) {
			int posx=x+dx[i];
			int posy=y+dy[i];
			if(check(player,posx,posy)==1) {
				int maxtmp=0; 
				char tmp1=player->mat[posx][posy];
				if(player->mat[posx][posy]=='o'||player->mat[posx][posy]=='O') {
					if(player->mat[posx][posy]=='o') {
						player->mat[posx][posy]='.';
						maxtmp=dfs(player,posx,posy,num+1,sum+10);
						player->mat[posx][posy]='o';
					} else if(player->mat[posx][posy]=='O') {
						player->mat[posx][posy]='.';
						maxtmp=dfs(player,posx,posy,num+1,sum+100+500*(6-num));
						player->mat[posx][posy]='O';
					}
				}
				if(player->mat[posx][posy]=='.'){
					maxtmp=dfs(player,posx,posy,num+1,sum);
				}///////////////////////////////////////
				if(maxtmp>max) {
					max=maxtmp;
				}
				player->mat[posx][posy]=tmp1;
			}
		}
		return max;
	}
}

struct Point walk(struct Player *player)
{
	// This function will be executed in each round.
	struct Point ret = {player->your_posx, player->your_posy};
	player->mat[1][4]='#';
	player->mat[1][10]='#';
	player->mat[11][4]='#';
	for(int i=0; i<100; i++) {
		for(int j=0; j<100; j++) {
			length[i][j]=190000;
		}
	}
	bfs(player,length);
	int flag=0,max=0,nextpath=0;
	for(int i=0; i<4; i++) {
		int posx=player->your_posx+dx[i];
		int posy=player->your_posy+dy[i];
		if(check(player,posx,posy)) {
			int sum1=0;
			char tmp2=player->mat[posx][posy];
			if(player->mat[posx][posy]=='o'||player->mat[posx][posy]=='O') {
				if(player->mat[posx][posy]=='o') {
					sum1=sum1+10;
				} else if(player->mat[posx][posy]=='O') {
					sum1=sum1+100+500*6;
					max=sum1;
					nextpath=i;
					flag=1;
					break;
				}
				player->mat[posx][posy]='.';
			}
			int maxtmp=dfs(player,posx,posy,0,sum1);
			if(maxtmp==max&&max!=0){
				flag=2;
			}
			if(maxtmp>max) {
				max=maxtmp;
				flag=1;
				nextpath=i;
			}
			player->mat[posx][posy]=tmp2;
		}
	}
	if(flag==1) {
		ret.X=player->your_posx+dx[nextpath];
		ret.Y=player->your_posy+dy[nextpath];
		step++;
		path[step][0]=dx[nextpath];
		path[step][1]=dy[nextpath];
		return ret;
	} 
	else if(flag==2) {
		for(int t=0;t<4;t++){
					int posx=player->your_posx+dx[t];
					int posy=player->your_posy+dy[t];
					if(check(player,posx,posy)){
						if(player->mat[posx][posy]=='o'||player->mat[posx][posy]=='O'){
							ret.X=posx;
							ret.Y=posy;
							return ret;
						}
					}      
		}
		for(int i=0; i<4; i++) {
			int posx=player->your_posx+dx[i];
			int posy=player->your_posy+dy[i];
			if(check(player,posx,posy)) {
				if(posx<player->row_cnt/2&&i==1){
					ret.X=posx;
					ret.Y=posy;
					step++;
					path[step][0]=dx[i];
					path[step][1]=dy[i];
					return ret;
				}
				else if(posx>player->row_cnt/2&&i==0){
					ret.X=posx;
					ret.Y=posy;
					step++;
					path[step][0]=dx[i];
					path[step][1]=dy[i];
					return ret;
				}
				else if(posy>player->col_cnt/2&&i==2){
					ret.X=posx;
					ret.Y=posy;
					step++;
					path[step][0]=dx[i];
					path[step][1]=dy[i];
					return ret;
				}
				else if(posy<player->row_cnt/2&&i==3){
					ret.X=posx;
					ret.Y=posy;
					step++;
					path[step][0]=dx[i];
					path[step][1]=dy[i];
					return ret;
				}
			}
		}
		
	}
	else {
		for(int i=0; i<4; i++) {
			int posx=player->your_posx+dx[i];
			int posy=player->your_posy+dy[i];
			if(check(player,posx,posy)) {
				if(posx<player->row_cnt/2&&i==1){
					ret.X=posx;
					ret.Y=posy;
					step++;
					path[step][0]=dx[i];
					path[step][1]=dy[i];
					return ret;
				}
				else if(posx>player->row_cnt/2&&i==0){
					ret.X=posx;
					ret.Y=posy;
					step++;
					path[step][0]=dx[i];
					path[step][1]=dy[i];
					return ret;
				}
				else if(posy>player->col_cnt/2&&i==2){
					ret.X=posx;
					ret.Y=posy;
					step++;
					path[step][0]=dx[i];
					path[step][1]=dy[i];
					return ret;
				}
				else if(posy<player->row_cnt/2&&i==3){
					ret.X=posx;
					ret.Y=posy;
					step++;
					path[step][0]=dx[i];
					path[step][1]=dy[i];
					return ret;
				}
			}
		}
	}

	for(int i=0; i<4; i++) {
		int posx=player->your_posx+dx[i];
		int posy=player->your_posy+dy[i];
		if(check(player,posx,posy)) {
			ret.X=posx;
			ret.Y=posy;
			step++;
			path[step][0]=dx[i];
			path[step][1]=dy[i];
			return ret;
		}
	}

}
//

int main()
{
	Player* player;
	player=(Player*)malloc(sizeof(Player));
	player->mat=(char**)malloc(100*sizeof(char*));
	char *tmp;
	for(int i=0; i<100; i++) {
		tmp=(char*)malloc(100*sizeof(char));
		player->mat[i]=tmp;
	}
	player->col_cnt=15;
	player->row_cnt=13;
	for(int i=0; i<player->row_cnt; i++) {
		for(int j=0; j<player->col_cnt; j++) {
			player->mat[i][j]=start[i][j];
		}
	}
	player->ghost_posx[0]=3;
	player->ghost_posy[0]=3;
	player->ghost_posx[1]=3;
	player->ghost_posy[1]=11;
	player->your_posx=1;
	player->your_posy=9;
	Point p=walk(player);
	pprint(p);
	for(int i=0; i<100; i++) {
		tmp=player->mat[i];
		free(tmp);
	}
	free(player->mat);
	free(player);
	pprint(p);
	return 0;
}

