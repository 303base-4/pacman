#include <string.h>
#include "../include/playerbase.h"
#define MAX 1000
int dx[4]={1,-1,0,0};
int dy[4]={0,0,-1,1};


void init(struct Player *player) {
	// This function will be executed at the begin of each game, only once.
}

typedef struct step{
    int x;
    int y;
    int dir;//记录方向
}Step;
int front,rear;
Step queue[MAX];
void enqueue(Step x){
    queue[rear]=x;
    rear=(rear+1)%MAX;
}
void dequeue(Step *x){
    *x=queue[front];
    front=(front+1+MAX)%MAX;
}

int in(int x,int y,struct Player *player){
    return x>=0 && x<player->row_cnt && y>=0 && y<player->col_cnt;
}
int check(int x,int y,struct Player *player){
    return in(x,y,player)&&player->mat[x][y]!='#';
}//别出界和别撞墙

int ghostcheck(int x,int y,struct Player *player){
    int gx0=player->ghost_posx[0];
    int gy0=player->ghost_posy[0];
    int gx1=player->ghost_posx[1];
    int gy1=player->ghost_posy[1];
    
    if(player->your_status>0){
        return 1;
    }
    if(gx0==x&&gy0==y||gx1==x&&gy1==y){
        return 0;
    }else{
        return 1;
    }
    
}//别上幽灵
int bfs(struct Player *player){
    front=0;rear=0;
    int vis[100][100]={0};
    int nowx=player->your_posx;
    int nowy=player->your_posy;
    vis[nowx][nowy]=1;
    for(int i=0;i<4;i++){
        int posx=nowx+dx[i];
        int posy=nowy+dy[i];
        if(in(posx,posy,player)&&ghostcheck(posx,posy,player)){
            if(player->mat[posx][posy]=='o'||player->mat[posx][posy]=='O'){
            return i;//return最近的方向
           }else{
          if(check(posx,posy,player)&&ghostcheck(posx,posy,player)&&vis[posx][posy]!=1){
             Step q={posx,posy,i};
            vis[posx][posy]=1;
            enqueue(q);
            }
            
        }
        }
        
    }//找周围 周围有的话就走，没有就入队用bfs找最近的
    while(front!=rear){
        Step now;
        dequeue(&now);
        for(int i=0;i<4;i++){
            int posx=now.x+dx[i];
            int posy=now.y+dy[i];
            int id=now.dir;
            
            if(in(posx,posy,player)){
                if(player->mat[posx][posy]=='o'||player->mat[posx][posy]=='O'){
                return id;
            }else{
                if(check(posx,posy,player)&&vis[posx][posy]!=1){
                    Step q={posx,posy,id};
                    vis[posx][posy]=1;
                    enqueue(q);
                }
            }
            }
            
        }
    }    
}
struct Point walk(struct Player *player) {
	// This function will be executed in each round.
	struct Point ret = {player->your_posx, player->your_posy};
    int i=bfs(player);
    ret.X = player->your_posx+dx[i];
    ret.Y = player->your_posy+dy[i];
    return ret;
}