#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <queue>
char start[100][100]={
{"oo#o#oo#oo"},
{"oo#o#oo#oo"},
{"oo##oo##oo"},
{"oo##oo##oo"},
{"oooooooooo"},
{"oooooooooo"},
{"oo#oooo#oo"},
{"oo#oooo#oo"},
{"oo######oo"},
{"oooO##Oooo"},
};
int dx[4]={-1,1,0,0};
int dy[4]={0,0,-1,1};
struct Point {
    int X, Y;
};
int length[100][100];
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
int check(struct Player *player,int x,int y){
    if(x<0||y<0||x>=player->row_cnt||y>=player->col_cnt)
        return 0;
    if(player->mat[x][y]=='#')
        return 0;
    if(x==player->ghost_posx[0]&&y==player->ghost_posy[0])
        return 0;
    if(x==player->ghost_posx[1]&&y==player->ghost_posy[1])
        return 0;
    else
        return 1;
}
void bfs1(struct Player *player,int x,int y){
    int queue[100][2];
    int front=0,rear=0;
    queue[rear][0]=x;
    queue[rear][1]=y;
    rear++;
    length[x][y]=0;
    while(front!=rear){
        int nowx=queue[front][0];
        int nowy=queue[front][1];
        front++;
        for(int i=0;i<4;i++){
            int posx=nowx+dx[i];
            int posy=nowy+dy[i];
            if(check(player,posx,posy)==1){
                queue[rear][0]=posx;
                queue[rear][1]=posy;
                rear++;
                if(length[posx][posy]>length[nowx][nowy]+1){
                    length[posx][posy]=length[nowx][nowy]+1;
                }
            }
        }
        if(length[nowx][nowy]==4){
            break;
        }
    }
        
}



static const Point MOV[4] = {{-1, 0}, {1, 0}, {0, 1}, {0, -1}};
struct qNode
    {
        Point pos;
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
static void bfs(Point pos, Player *player, int step[100][100])
{
    
    std::queue<qNode> q;
    q.push({pos, 0});
    bool flag[61][61];
    memset(flag, false, sizeof(flag));
    flag[pos.X][pos.Y] = true;
    while (!q.empty())
    {
        qNode tmp = q.front();
        q.pop();
        for (int i = 0; i < 4; i++)
        {
            Point next = {tmp.pos.X + MOV[i].X, tmp.pos.Y + MOV[i].Y};
            if (check(next, player) && !flag[next.X][next.Y])
            {
                if (tmp.step + 1 <= 5)
                    q.push({next, tmp.step + 1});
                flag[next.X][next.Y] = true;
                step[next.X][next.Y]= tmp.step + 1;
            }
        }
    }
}











struct Point walk(struct Player *player) {
	// This function will be executed in each round.
	struct Point ret = {player->your_posx, player->your_posy};
	for(int i=0;i<100;i++){
        for(int j=0;j<100;j++){
            length[i][j]=190000;
        }
    }
    Point p={player->ghost_posx[0],player->ghost_posy[0]};
    bfs(p,player,length);
    for(int i=0;i<4;i++){
        int posx=player->your_posx+dx[i];
        int posy=player->your_posy+dy[i];
        if((posx==player->ghost_posx[0]&&posy==player->ghost_posy[0])||(posx==player->ghost_posx[1]&&posy==player->ghost_posy[1])){
            for(int j=0;j<4;j++){
                posx=player->your_posx+dx[j];
                posy=player->your_posy+dy[j];
                if(check(player,posx,posy)&&length[posx][posy]>1){
                    ret.X=posx;
                    ret.Y=posy;
                    return ret;
                }
           }
            
        }      
    }
}
void pprint(Point p){
	printf("(%d,%d)",p.X,p.Y);
	return;
}

int main(){
	Player* player;
	player=(Player*)malloc(sizeof(Player));
	player->mat=(char**)malloc(100*sizeof(char*));
	char *tmp;
	for(int i=0;i<100;i++){
		tmp=(char*)malloc(100*sizeof(char));
		player->mat[i]=tmp;
	}
	player->col_cnt=10;
	player->row_cnt=10;
	for(int i=0;i<player->row_cnt;i++){
		for(int j=0;j<player->col_cnt;j++){
			player->mat[i][j]=start[i][j];
		}
	}
	player->ghost_posx[0]=6;
	player->ghost_posy[0]=6;
	player->your_posx=7;
	player->your_posy=6;
	player->your_posx=7;
	player->your_posy=6;
	Point p=walk(player);
	pprint(p);
	return 0;
}
