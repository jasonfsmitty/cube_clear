#ifndef GAME_EVENT_H
#define GAME_EVENT_H

namespace event {

	enum Type
	{
		EVT_BOARD,
	};

	struct Board
	{
		enum Input { UP, DOWN, LEFT, RIGHT, ENTER };

		Input input;
	};
}

#endif /* GAME_EVENT_H */

