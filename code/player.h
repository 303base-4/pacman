#include "../include/playerbase.h"
#include <math.h>
#include <queue>
#include <stdlib.h>
#include <string.h>
static const int MAXN = 61;
static const Point MOV[4] = {{-1, 0}, {0, 1}, {1, 0}, {0, -1}};
static int run[MAXN][MAXN];      // 逃跑距离
static int choice[MAXN][MAXN];   // 可选方向数
static int ghostx[2], ghosty[2]; // 幽灵的初始位置
static int ed[MAXN][MAXN];       // 敌人的距离
static int run_x, run_y;         // 逃跑方向

static int intmin(int a, int b)
{
    return a < b ? a : b;
}
static int intmax(int a, int b)
{
    return a > b ? a : b;
}
static bool check(Point pos, Player *player) // 检查位置pos是否合法
{
    int x1 = pos.X, y1 = pos.Y;
    if (x1 >= 0 && x1 < player->row_cnt && y1 >= 0 && y1 < player->col_cnt && player->mat[x1][y1] != '#')
        return true;
    else
        return false;
}

static int init_run(Point pos, Player *player, int choice[MAXN][MAXN])
// 计算某个位置的逃跑距离。逃跑距离是指该位置到安全区（四个方向都可以走的点）的最短路径长度
// 采用广度优先搜索
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

void init(struct Player *player) // 初始化，包括一个点可走方向数的计算、逃跑距离的计算
{
    // This function will be executed at the begin of each game, only once.
    // 计算每个点可以走的方向数
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
    // 可选方向数小于2的点往往是死角或者死胡同入口，不是可选方向。所以将这些点周围点的可选方向-1.
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
    // 初始化每个点的逃跑距离
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
    // 记录幽灵的初始位置，方便吃幽灵后的计算
    ghostx[0] = player->ghost_posx[0];
    ghostx[1] = player->ghost_posx[1];
    ghosty[0] = player->ghost_posy[0];
    ghosty[1] = player->ghost_posy[1];
}
static void update_run(Player *player)
// 更新逃跑距离。如果敌人到达了安全区或安全区附近，那么该安全区不再安全，与此安全区有关的逃跑距离需要更新
{
    // 更新敌人附近点的可选方向数
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
static void enemy_dist(Player *player, int step[MAXN][MAXN]) // 计算地图上所有点到离它最近的敌人的最短距离
{
    // 广度优先搜索
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

static int calcscore(Player *tmp) // 计算tmp中玩家位置的分数
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
static bool unsafe(Player *now) // 判断敌人是否接近
{
    int run_dist = run[now->your_posx][now->your_posy];
    // 先用曼哈顿距离简单判断
    if (abs(now->ghost_posx[0] - now->your_posx) + abs(now->ghost_posy[0] - now->your_posy) <= run_dist * 2 + 1 ||
        abs(now->ghost_posx[1] - now->your_posx) + abs(now->ghost_posy[1] - now->your_posy) <= run_dist * 2 + 1 ||
        (now->opponent_status > 0 &&
         abs(now->opponent_posx - now->your_posx) + abs(now->opponent_posy - now->your_posy) <= run_dist * 2 + 1))
    {
        // 进行一次逃跑尝试
        memset(ed, 0x3f3f3f3f, sizeof(ed));
        enemy_dist(now, ed);
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
            run_x = now->your_posx + MOV[mini].X, run_y = now->your_posy + MOV[mini].Y;
        }
        else
        {
            run_x = now->your_posx, run_y = now->your_posy;
        }
        struct qNode
        {
            Point pos;
            int step;
            int direction;
            int mind; // 记录该逃跑路线中与敌人最近的距离
        };
        std::queue<qNode> q;
        bool flag[MAXN][MAXN];
        memset(flag, false, sizeof(flag));
        q.push({now->your_posx, now->your_posy, 0, -1, ed[now->your_posx][now->your_posy]});
        flag[now->your_posx][now->your_posy] = true;
        int mind = 0, ndx = 3;
        while (!q.empty())
        {
            qNode tmp = q.front();
            for (int i = 0; i < 4; i++)
            {
                qNode next = tmp;
                next.pos.X += MOV[i].X, next.pos.Y += MOV[i].Y;
                next.step++;
                if (next.direction == -1)
                    next.direction = i;
                next.mind = intmin(next.mind, ed[next.pos.X][next.pos.Y] - next.step);
                if (check(next.pos, now) && ed[next.pos.X][next.pos.Y] > next.step && !flag[next.pos.X][next.pos.Y])
                {
                    if (choice[next.pos.X][next.pos.Y] >= ndx && next.step < ed[next.pos.X][next.pos.Y])
                    {
                        if (choice[next.pos.X][next.pos.Y] > ndx)
                        {
                            ndx = choice[next.pos.X][next.pos.Y];
                            mind = next.mind;
                        }
                        if (next.mind >= mind)
                        {
                            mind = next.mind;
                            run_x = now->your_posx + MOV[next.direction].X;
                            run_y = now->your_posy + MOV[next.direction].Y;
                        }
                    }
                    // 吃到超级星且强化状态有效也算逃跑成功
                    if (now->mat[next.pos.X][next.pos.Y] == 'O' && ed[next.pos.X][next.pos.Y] < 19)
                    {
                        run_x = now->your_posx + MOV[next.direction].X;
                        run_y = now->your_posy + MOV[next.direction].Y;
                        return true;
                    }
                    q.push(next);
                    flag[next.pos.X][next.pos.Y] = true;
                }
            }
            q.pop();
        }
        // 如果逃跑过程中与敌人最近距离<=2，则视为危险
        if (mind <= 2)
            return true;
    }
    return false;
}

static Point hunt(Player *now) // 在拿到超级星状态下，追击离自己最近的敌人
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

static Point get_star(Player *now) // 广度优先搜索找到最近的一颗星拿分
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
    int maxc = 0, maxd = -1;
    for (int i = 0; maxd == -1; i++)
    {
        while (q.front().step == i)
        {
            qNode tmp = q.front();
            q.pop();
            for (int j = 0; j < 4; j++)
            {
                qNode next = tmp;
                next.pos.X += MOV[j].X, next.pos.Y += MOV[j].Y;
                if (next.direction == -1)
                    next.direction = j;
                next.step++;
                if (!check({next.pos.X, next.pos.Y}, now) || flag[next.pos.X][next.pos.Y])
                    continue;
                Player ptmp = *now;
                ptmp.your_posx = next.pos.X, ptmp.your_posy = next.pos.Y;
                if (calcscore(&ptmp) > 0)
                {
                    if (choice[next.pos.X][next.pos.Y] > maxc)
                    {
                        maxc = choice[next.pos.X][next.pos.Y];
                        maxd = next.direction;
                    }
                }
                q.push(next);
                flag[next.pos.X][next.pos.Y] = true;
            }
        }
    }
    return {now->your_posx + MOV[maxd].X, now->your_posy + MOV[maxd].Y};
}
struct Point walk(struct Player *player) // 操作函数
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
            ret = {run_x, run_y}; // 逃跑方向在unsafe中已经计算出来
        }
    }
    else
    {
        int ostmp = start->opponent_status;
        if (start->opponent_status == 0)
            start->opponent_status = 1;
        else
            start->opponent_status = 0;
        memset(ed, 0x3f3f3f3f, sizeof(ed));
        enemy_dist(start, ed);
        start->opponent_status = ostmp;
        if (ed[start->your_posx][start->your_posy] < start->your_status - 1)
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
