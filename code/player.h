

//./run.sh --visible
#include <string.h>
#include "../include/playerbase.h"
#include <queue>
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
				if (tmp.step + 1 <= 15)
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
	if(num==9) {
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
    for(int i=0; i<100; i++) {
		for(int j=0; j<100; j++) {
			length[i][j]=190000;
		}
	}
    bfs(player,length);
    if(player->your_status>0){
        int min=1000;int nextp=0;
        for(int t=0;t<4;t++){
				int posx=player->your_posx+dx[t];
				int posy=player->your_posy+dy[t];
				if(check(player,posx,posy)){
					if(length[posx][posy]<min){
                        min=length[posx][posy];
                        nextp=t;
					}
				}      
		}
        if(min!=1000){
            ret.X=player->your_posx+dx[nextp];
		    ret.Y=player->your_posy+dy[nextp];
            return ret;
        }
    }
	player->mat[1][4]='#';
	player->mat[1][10]='#';
	player->mat[11][4]='#';
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
