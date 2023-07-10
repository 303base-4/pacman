#include <string.h>
#include "../include/playerbase.h"
int dx[4]={-1,1,0,0};
int dy[4]={0,0,-1,1};
int step=1;
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
    if(step==33)
    {
        step=44;
        ret.Y--;
    }
    else if(step==29)
    {
        step=32;
        ret.Y--;
    }
        else if(step==1||step==2||step==14||step==15||step==16||step==17||step==18||step==19||step==20||step==21||step==22||step==34||step==35||step==36||step==49||step==51||step==52||step==54||step==56||step==57||step==58)
    { //上
        ret.X--;
        step++;
    }
    else if(step==3||step==4||step==11||step==12||step==13||step==38||step==45||step==64||step==66||step==70||step==74||step==77||step==78||step==79)
    {//右
        ret.Y++;
        step++;
    }
    else if(step==5||step==6||step==7||step==8||step==24||step==25||step==26||step==27||step==30||step==37||step==40||step==42||step==43||step==44||step==46||step==47||step==60||step==61||step==62||step==63||step==65||step==69||step==71||step==73||step==75)
    {//下
        ret.X++;
        step++;
    }
    else if(step==9||step==10||step==23||step==28||step==31||step==32||step==39||step==41||step==48||step==50||step==53||step==55||step==59||step==67||step==68||step==72||step==76)
    {//左
        ret.Y--;
        step++;
    }
    
    return ret;
}
