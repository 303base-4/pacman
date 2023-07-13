#include "../include/playerbase.h"
#include <math.h>
#include <queue>
#include <stdlib.h>
#include <string.h>
static const int MAXN = 61;
static const Point MOV[4] = {{-1, 0}, {0, 1}, {1, 0}, {0, -1}};
static const int DEPTH = 5;
static int run[MAXN][MAXN];
static int choice[MAXN][MAXN];
static int ghostx[2], ghosty[2];
static int ed[MAXN][MAXN];
static bool walked[MAXN][MAXN];

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

static int init_run(Point pos, Player *player, int choice[MAXN][MAXN])
{
    struct qNode
    {
        int x, y;
        int step;
    };
    std::queue<qNode> q;
    q.push({pos.X, pos.Y, 0});
    bool flag[MAXN][MAXN];
    memset(flag, false, sizeof(flag));
    flag[pos.X][pos.Y] = true;
    while (!q.empty())
    {
        qNode tmp = q.front();
        q.pop();
        for (int i = 0; i < 4; i++)
        {
            int x1 = tmp.x + MOV[i].X, y1 = tmp.y + MOV[i].Y;
            if (check({x1, y1}, player) && !flag[x1][y1])
            {
                if (choice[x1][y1] >= 4)
                    return tmp.step + 1;
                q.push({x1, y1, tmp.step + 1});
                flag[x1][y1] = true;
            }
        }
    }
    return 0x3f3f3f3f;
}

void init(struct Player *player)
{
    // This function will be executed at the begin of each game, only once.
    memset(walked, false, sizeof(walked));
    for (int i = 0; i < player->row_cnt; i++)
    {
        for (int j = 0; j < player->col_cnt; j++)
        {
            if (!check({i, j}, player))
                continue;
            for (int k = 0; k < 4; k++)
            {
                int x1 = i + MOV[k].X, y1 = j + MOV[k].Y;
                if (check({x1, y1}, player))
                {
                    choice[i][j]++;
                }
            }
        }
    }
    int choice_tmp[MAXN][MAXN];
    memcpy(choice_tmp, choice, sizeof(choice_tmp));
    for (int i = 0; i < player->row_cnt; i++)
    {
        for (int j = 0; j < player->col_cnt; j++)
        {
            if (!check({i, j}, player) || choice_tmp[i][j] >= 3)
                continue;
            for (int k = 0; k < 4; k++)
            {
                int x1 = i + MOV[k].X, y1 = j + MOV[k].Y;
                if (!check({x1, y1}, player) || choice_tmp[x1][y1] < 3)
                    continue;
                choice[x1][y1]--;
            }
        }
    }
    for (int i = 0; i < player->row_cnt; i++)
    {
        for (int j = 0; j < player->col_cnt; j++)
        {
            if (!check({i, j}, player))
                continue;
            if (choice[i][j] >= 4)
                run[i][j] = 0;
            else
                run[i][j] = init_run({i, j}, player, choice);
        }
    }
    ghostx[0] = player->ghost_posx[0];
    ghostx[1] = player->ghost_posx[1];
    ghosty[0] = player->ghost_posy[0];
    ghosty[1] = player->ghost_posy[1];
}
static void update_run(Player *player)
{
    for (int i = 0; i < 2; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            int x1 = player->ghost_posx[i] + MOV[j].X, y1 = player->ghost_posy[i] + MOV[j].Y;
            if (player->ghost_posx[1] == player->ghost_posx[0] && player->ghost_posy[1] == player->ghost_posy[0] &&
                i == 1)
                break;
            if (!check({x1, y1}, player))
                continue;
            else
            {
                choice[x1][y1]--;
            }
        }
    }
    for (int i = 0; i < 2; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            int x1 = player->ghost_posx[i] + MOV[j].X, y1 = player->ghost_posy[i] + MOV[j].Y;
            if (player->ghost_posx[1] == player->ghost_posx[0] && player->ghost_posy[1] == player->ghost_posy[0] &&
                i == 1)
                break;
            if (!check({x1, y1}, player))
                continue;
            else
            {
                if (choice[x1][y1] <= 3)
                {
                    for (int k = 0; k < 4; k++)
                    {
                        int x2 = x1 + MOV[k].X, y2 = y1 + MOV[k].Y;
                        if (!check({x2, y2}, player))
                            continue;
                        run[x2][y2] = init_run({x2, y2}, player, choice);
                    }
                }
            }
        }
    }
    if (player->opponent_status > 0)
    {
        for (int j = 0; j < 4; j++)
        {
            int x1 = player->opponent_posx + MOV[j].X, y1 = player->opponent_posy + MOV[j].Y;
            if (!check({x1, y1}, player))
                continue;
            else
            {
                choice[x1][y1]--;
            }
        }
        for (int j = 0; j < 4; j++)
        {
            int x1 = player->opponent_posx + MOV[j].X, y1 = player->opponent_posy + MOV[j].Y;
            if (!check({x1, y1}, player))
                continue;
            else
            {
                if (choice[x1][y1] <= 3)
                {
                    for (int k = 0; k < 4; k++)
                    {
                        int x2 = x1 + MOV[k].X, y2 = y1 + MOV[k].Y;
                        if (!check({x2, y2}, player))
                            continue;
                        run[x2][y2] = init_run({x2, y2}, player, choice);
                    }
                }
            }
        }
    }
}
static void enemy_dist(Player *player, int step[MAXN][MAXN])
{
    struct qNode
    {
        int x, y;
        int step;
    };
    std::queue<qNode> q;
    q.push({player->ghost_posx[0], player->ghost_posy[0], 0});
    q.push({player->ghost_posx[1], player->ghost_posy[1], 0});
    bool flag[61][61];
    memset(flag, false, sizeof(flag));
    flag[player->ghost_posx[0]][player->ghost_posy[0]] = true;
    flag[player->ghost_posx[1]][player->ghost_posy[1]] = true;
    step[player->ghost_posx[0]][player->ghost_posy[0]] = 0;
    step[player->ghost_posx[1]][player->ghost_posy[1]] = 0;
    if (player->opponent_status > 0)
    {
        q.push({player->opponent_posx, player->opponent_posy, 0});
        flag[player->opponent_posx][player->opponent_posy] = true;
        step[player->opponent_posx][player->opponent_posy] = 0;
    }
    while (!q.empty())
    {
        qNode tmp = q.front();
        q.pop();
        for (int i = 0; i < 4; i++)
        {
            int x1 = tmp.x + MOV[i].X, y1 = tmp.y + MOV[i].Y;
            if (check({x1, y1}, player) && !flag[x1][y1])
            {
                q.push({x1, y1, tmp.step + 1});
                flag[x1][y1] = true;
                step[x1][y1] = tmp.step + 1;
            }
        }
    }
}
/*
static int put(Player *tmp)
{
    int score = 0;
    if (tmp->mat[tmp->your_posx][tmp->your_posy] == 'o' || tmp->mat[tmp->your_posx][tmp->your_posy] == 'O')
    {
        score += 10;
        tmp->mat[tmp->your_posx][tmp->your_posy] = '.';
    }
    if (tmp->your_status > 0)
    {
        if (tmp->your_posx == tmp->ghost_posx[0] && tmp->your_posy == tmp->ghost_posy[0])
        {
            score += 200;
            tmp->ghost_posx[0] = ghostx[0], tmp->ghost_posy[0] = tmp->ghost_posy[0];
        }
        if (tmp->your_posx == tmp->ghost_posx[1] && tmp->your_posy == tmp->ghost_posy[1])
        {
            score += 200;
            tmp->ghost_posx[1] = ghostx[1], tmp->ghost_posy[1] = tmp->ghost_posy[1];
        }
        if (tmp->your_posx == tmp->opponent_posx && tmp->your_posy == tmp->opponent_posy && tmp->opponent_status == 0)
        {
            score += tmp->opponent_score / 2;
            tmp->opponent_status = -1;
        }
    }
    return score;
}
*/
static int calcscore(Player *tmp)
{
    int score = 0;
    if (tmp->mat[tmp->your_posx][tmp->your_posy] == 'o' || tmp->mat[tmp->your_posx][tmp->your_posy] == 'O')
    {
        score += 10;
    }
    if (tmp->your_status > 0)
    {
        if (tmp->your_posx == tmp->ghost_posx[0] && tmp->your_posy == tmp->ghost_posy[0])
        {
            score += 200;
        }
        if (tmp->your_posx == tmp->ghost_posx[1] && tmp->your_posy == tmp->ghost_posy[1])
        {
            score += 200;
        }
        if (tmp->your_posx == tmp->opponent_posx && tmp->your_posy == tmp->opponent_posy && tmp->opponent_status == 0)
        {
            score += tmp->opponent_score / 2;
        }
    }
    return score;
}
static bool unsafe(Player *now)
{
    int run_dist = run[now->your_posx][now->your_posy];
    if (abs(now->ghost_posx[0] - now->your_posx) + abs(now->ghost_posy[0] - now->your_posy) <= run_dist * 2 + 1 ||
        abs(now->ghost_posx[1] - now->your_posx) + abs(now->ghost_posy[1] - now->your_posy) <= run_dist * 2 + 1 ||
        (now->opponent_status > 0 &&
         abs(now->opponent_posx - now->your_posx) + abs(now->opponent_posy - now->your_posy) <= run_dist * 2 + 1))
    {
        memset(ed, 0x3f3f3f3f, sizeof(ed));
        enemy_dist(now, ed);
        if (ed[now->your_posx][now->your_posy] <= run[now->your_posx][now->your_posy] * 2 + 1)
            return true;
    }
    return false;
}
static Point run_away(Player *now)
{
    struct qNode
    {
        Point pos;
        int step;
        int direction;
    };
    std::queue<qNode> q;
    bool flag[MAXN][MAXN];
    memset(flag, false, sizeof(flag));
    q.push({now->your_posx, now->your_posy, 0, -1});
    flag[now->your_posx][now->your_posy] = true;
    int nd = -1, ndx = 3;
    while (!q.empty())
    {
        qNode tmp = q.front();
        q.pop();
        for (int i = 0; i < 4; i++)
        {
            qNode next = tmp;
            next.pos.X += MOV[i].X, next.pos.Y += MOV[i].Y;
            next.step++;
            if (next.direction == -1)
                next.direction = i;
            if (check(next.pos, now) && ed[next.pos.X][next.pos.Y] > next.step && !flag[next.pos.X][next.pos.Y])
            {
                if (choice[next.pos.X][next.pos.Y] >= ndx && next.step < ed[next.pos.X][next.pos.Y])
                {
                    ndx = choice[next.pos.X][next.pos.Y];
                    nd = next.direction;
                }
                if (now->mat[next.pos.X][next.pos.Y] == 'O' && ed[next.pos.X][next.pos.Y] < 19)
                {
                    return {now->your_posx + MOV[next.direction].X, now->your_posy + MOV[next.direction].Y};
                }
                q.push(next);
                flag[next.pos.X][next.pos.Y] = true;
            }
        }
    }
    if (nd != -1)
    {
        return {now->your_posx + MOV[nd].X, now->your_posy + MOV[nd].Y};
    }
    int maxd = -1, mini = -1;
    for (int i = 0; i < 4; i++)
    {
        Point next = {now->your_posx + MOV[i].X, now->your_posy + MOV[i].Y};
        if (!check(next, now))
            continue;
        if (ed[next.X][next.Y] > maxd)
        {
            maxd = ed[next.X][next.Y];
            mini = i;
        }
    }
    if (maxd > ed[now->your_posx][now->your_posy])
    {
        return {now->your_posx + MOV[mini].X, now->your_posy + MOV[mini].Y};
    }
    else
    {
        return {now->your_posx, now->your_posy};
    }
}
static Point hunt(Player *now)
{
    int mind = 0x3f3f3f3f, mini = -1;
    for (int i = 0; i < 4; i++)
    {
        Point next = {now->your_posx + MOV[i].X, now->your_posy + MOV[i].Y};
        if (!check(next, now))
            continue;
        if (ed[next.X][next.Y] < mind)
        {
            mind = ed[next.X][next.Y];
            mini = i;
        }
    }
    return {now->your_posx + MOV[mini].X, now->your_posy + MOV[mini].Y};
}
static struct Point get_star(struct Player *start)
{
    struct Point ret = {start->your_posx, start->your_posy};
    int maxx = 0, maxi = -1;
    for (int i = 0; i < 4; i++)
    {
        Player tmp = *start;
        tmp.your_posx += MOV[i].X;
        tmp.your_posy += MOV[i].Y;
        if (!check({tmp.your_posx, tmp.your_posy}, start))
            continue;
        int h = calcscore(&tmp) + choice[tmp.your_posx][tmp.your_posy];
        if (h > maxx)
        {
            maxx = h;
            maxi = i;
        }
        else
        {
            if (maxx < 10 && walked[tmp.your_posx][tmp.your_posx] == false)
            {
                if (maxi == -1)
                {
                    maxx = h;
                    maxi = i;
                }
                else if (walked[start->your_posx + MOV[maxi].X][start->your_posy + MOV[maxi].Y])
                {
                    maxx = h;
                    maxi = i;
                }
            }
        }
    }
    int x1 = start->your_posx + MOV[maxi].X, y1 = start->your_posy + MOV[maxi].Y;
    walked[x1][y1] = true;
    ret = {start->your_posx + MOV[maxi].X, start->your_posy + MOV[maxi].Y};
    return ret;
}
struct Point walk(struct Player *player)
{
    // This function will be executed in each round.
    struct Point ret = {player->your_posx, player->your_posy};
    Player *start = (Player *)malloc(sizeof(Player));
    *start = *player;
    int choice_tmp[MAXN][MAXN], run_tmp[MAXN][MAXN];
    memcpy(choice_tmp, choice, sizeof(choice_tmp));
    memcpy(run_tmp, run, sizeof(run_tmp));
    update_run(start);
    if (start->your_status == 0)
    {
        if (!unsafe(start))
        {
            ret = get_star(start);
        }
        else
        {
            ret = run_away(start);
        }
    }
    else
    {
        int ostmp = start->opponent_status;
        start->opponent_status = 0;
        memset(ed, 0x3f3f3f3f, sizeof(ed));
        enemy_dist(start, ed);
        start->opponent_status = ostmp;
        if (ed[start->your_posx][start->your_posy] < start->your_status)
        {
            ret = hunt(start);
        }
        else
        {
            ret = get_star(start);
        }
    }
    memcpy(choice, choice_tmp, sizeof(choice));
    memcpy(run, run_tmp, sizeof(run));
    free(start);
    return ret;
}
