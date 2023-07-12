#include <string.h>
#include "../include/playerbase.h"
#include <iostream>
#include <queue>

using namespace std;

const int N = 62, INF = 0x3f3f3f3f;
const int xz[4] = {-1, 1, 0, 0};
const int yz[4] = {0, 0, -1, 1};

int d[N][N][N][N], n, m;
char ma[N][N];
struct POINT
{
    int x, y;
    POINT (int x = -1, int y = -1) : x(x), y(y) {}
};
bool ava(int x, int y, struct Player *player)
{
    if (x >= n || y >= m || x < 0 || y < 0) return 0;
    if (player->mat[x][y] == '#') return 0;
    return 1;
}
void bfs(int x, int y, struct Player *player)
{
    queue<pair<int, int> > q;
    q.push(make_pair(x, y));
    d[x][y][x][y] = 0;
    while (!q.empty())
    {
        pair<int, int> p = q.front(); q.pop();
        //if (x == 7 && y == 6)
        //printf("work %d %d\n", p.first, p.second);
        for (int di = 0; di < 4; ++ di)
        {
            int xx = p.first + xz[di];
            int yy = p.second + yz[di];
            if (ava(xx, yy, player) && d[x][y][xx][yy] == INF)
            {
                d[x][y][xx][yy] = d[x][y][p.first][p.second] + 1;
                q.push(make_pair(xx, yy));
            }
        }
    }
}
Point chocho;
int step;
void init(struct Player *player)
{
    memset(d, 0x3f, sizeof(d));
    n = player->row_cnt, m = player->col_cnt;
    for (int i = 0; i < n; ++ i)
    {
        for (int j = 0; j < m; ++ j)
        {
            if (player->mat[i][j] != '#')
            {
                //printf("bfs %d %d\n", i, j);
                bfs(i, j, player);
            }
        }
    }
    step = 0;
}
bool v[N][N];
struct Point makepoint(int x, int y)
{
    struct Point temp;
    temp.X = x;
    temp.Y = y;
    return temp;
}
struct Point walk(struct Player *player)
{
    POINT me = POINT(player->your_posx, player->your_posy);
    POINT op = POINT(player->opponent_posx, player->opponent_posy);
    POINT g0 = POINT(player->ghost_posx[0], player->ghost_posy[0]);
    POINT g1 = POINT(player->ghost_posx[1], player->ghost_posy[1]);
    int stat = player->your_status, op_stat = player->opponent_status;
    step ++;
    if (player->opponent_status == -1 && player->your_score - 400 - 1 > player->opponent_score) return makepoint(-1, -1);
    vector<POINT> stars;
    int base = 0;
    int sc[2] = {10, 30};
    //printf("g0 %d %d\n", g0.x, g0.y);
    //printf("g1 %d %d\n", g1.x, g1.y);
    if (stat > 1)
    {
        for (int di = 0; di < 4; ++ di)
        {
            POINT mn = POINT(me.x + xz[di], me.y + yz[di]);
            if (!ava(mn.x, mn.y, player)) continue;
            if (g1.x == mn.x && g1.y == mn.y) return makepoint(mn.x, mn.y);
            if (g0.x == mn.x && g0.y == mn.y) return makepoint(mn.x, mn.y);
        }
    }
    for (int i = 0; i < n; ++ i)
    {
        for (int j = 0; j < m; ++ j)
        {
            int d1 = min(d[i][j][g0.x][g0.y], d[i][j][g1.x][g1.y]);
            if (max(d[i][j][op.x][op.y], 1) < op_stat)
            {
                d1 = min(max(d[i][j][op.x][op.y], 1), d1);
            }
            int d2 = max(1, d[i][j][me.x][me.y]);
            if (d1 <= d2 && d2 >= stat) player->mat[i][j] = '#';
        }
    }
    /*for (int i = 0; i < n; ++ i)
    {
        cout << mat[i] << endl;
    }*/
    for (int i = 0; i < n; ++ i)
    {
        for (int j = 0; j < m; ++ j)
        {
            if (player->mat[i][j] == '#') continue;
            if (player->mat[i][j] != '.')
            {
                stars.push_back(POINT(i, j));
                if (d[i][i][me.x][me.y] < INF)
                    base += d[i][j][me.x][me.y] * sc[player->mat[i][j] == 'O'];
            }
        }
    }
    POINT cho = POINT(me.x, me.y);
    int danger = min(d[me.x][me.y][g0.x][g0.y], d[me.x][me.y][g1.x][g1.y]);
    if (op_stat && player->your_score > 0) danger = min(danger, d[me.x][me.y][op.x][op.y]);
    //if (step < 5)
    if ((danger > 7 && step % 100 < 5) || step < 5)
    {
        for (int di = 0; di < 4; ++ di)
        {
            POINT mn = POINT(me.x + xz[di], me.y + yz[di]);
            if (!ava(mn.x, mn.y, player)) continue;
            int add = 0;
            for (POINT s : stars)
            {
                if (d[mn.x][mn.y][s.x][s.y] < INF)
                    add += d[mn.x][mn.y][s.x][s.y];
            }
            //printf("%d %d : base = %d add = %d\n", mn.x, mn.y, base, add);
            if (add < base || player->mat[cho.x][cho.y] == '#')
            {
                base = add;
                cho = mn;
            }
        }
    }
    else
    {
        if (stars.empty())
        {
            base = min(d[me.x][me.y][g0.x][g0.y], d[me.x][me.y][g1.x][g1.y]);
            for (int di = 0; di < 4; ++ di)
            {
                POINT mn = POINT(me.x + xz[di], me.y + yz[di]);
                if (!ava(mn.x, mn.y, player)) continue;
                int tmp = min(d[mn.x][mn.y][g0.x][g0.y], d[mn.x][mn.y][g1.x][g1.y]);
                if (player->mat[cho.x][cho.y] == '#' || tmp < base)
                {
                    base = tmp;
                    cho = mn;
                }
            }

        }
        else
        {
            base = INF;
            POINT chos;
            for (POINT s : stars)
            {
                if (d[s.x][s.y][me.x][me.y] < base || player->mat[s.x][s.y] == 'O')
                {
                    base = d[s.x][s.y][me.x][me.y];
                    chos = s;
                }
            }
            //printf("chos = %d %d\n", chos.x, chos.y);
            for (int di = 0; di < 4; ++ di)
            {
                POINT mn = POINT(me.x + xz[di], me.y + yz[di]);
                if (!ava(mn.x, mn.y, player)) continue;
                if (player->mat[cho.x][cho.y] == '#' || d[mn.x][mn.y][chos.x][chos.y] < base)
                {
                    base = d[mn.x][mn.y][chos.x][chos.y];
                    cho = mn;
                }
            }
        }
    }
    return makepoint(cho.x, cho.y);
}