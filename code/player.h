#include <string.h>
#include "../include/playerbase.h"
int dx[4]={-1,1,0,0};
int dy[4]={0,0,-1,1};
int step=0;
int path[100][2];

int check(struct Player *player,int x,int y){
    if(x<0||y<0||x>=player->row_cnt||y>=player->col_cnt)
        return 0;
    if(player->mat[x][y]=='#')
        return 0;
    else
        return 1;
}

int dfs(struct Player *player,int x,int y,int num,int sum){
    int max=0;
    if(num==3){
        return sum;
    }
    else{
        for(int i=0;i<4;i++){
            int posx=x+dx[i];
            int posy=y+dy[i];
            if(check(player,posx,posy)==1){
                char tmp1=player->mat[posx][posy];
                if(player->mat[posx][posy]=='o'||player->mat[posx][posy]=='O'){
                    sum=sum+10;
                    player->mat[posx][posy]=='.';
                }
                int maxtmp=dfs(player,posx,posy,num+1,sum);
                if(maxtmp>max){
                    max=maxtmp;
                }
                player->mat[posx][posy]=tmp1;
            }
        }
        return max;
    }
}

void init(struct Player *player) {
	// This function will be executed at the begin of each game, only once.
}


struct Point walk(struct Player *player) {
	// This function will be executed in each round.
	struct Point ret = {player->your_posx, player->your_posy};
    for(int i=0;i<4;i++){
        int posx=player->your_posx+dx[i];
        int posy=player->your_posy+dy[i];
        if(check(player,posx,posy)){
            if(player->mat[posx][posy]=='o'||player->mat[posx][posy]=='O'){
                ret.X=posx;
                ret.Y=posy;
                step++;
                path[step][0]=dx[i];
                path[step][1]=dy[i];
                return ret;
            }
        }      
    }
    int flag=0,max=0,nextpath=0;
    for(int i=0;i<4;i++){
        int posx=player->your_posx+dx[i];
        int posy=player->your_posy+dy[i];
        if(check(player,posx,posy)){
                ret.X=posx;
                ret.Y=posy;
                int maxtmp=dfs(player,posx,posy,0,0);
                if(maxtmp>max){
                    max=maxtmp;
                    flag=1;
                    nextpath=i;
                }
        }  
    }
    if(flag==1){
        ret.X=player->your_posx+dx[nextpath];
        ret.Y=player->your_posy+dy[nextpath];
        step++;
        path[step][0]=dx[nextpath];
        path[step][1]=dy[nextpath];
        return ret;
    }
    else{
        if(check(player,player->your_posx,player->your_posy+1)==1)
        ret.Y=player->your_posy+1;
        return ret; 
    }
}
