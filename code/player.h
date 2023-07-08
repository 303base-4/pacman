#include <string.h>
#include "../include/playerbase.h"

void init(struct Player *player) {
	// This function will be executed at the begin of each game, only once.
}


struct Point walk(struct Player *player) {
	// This function will be executed in each round.
	struct Point ret = {player->your_posx, player->your_posy};
    return ret;
}
