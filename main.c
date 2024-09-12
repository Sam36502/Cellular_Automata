#include <SDL2/SDL.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_ttf.h>

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define PIXEL_SIZE 4
#define COLOUR_COUNT 8
#define BOARD_WIDTH 200
#define BOARD_HEIGHT 200
#define TIMER_INTERVAL 30 // ms
#define SCREEN_WIDTH BOARD_WIDTH * PIXEL_SIZE
#define SCREEN_HEIGHT BOARD_HEIGHT * PIXEL_SIZE

#define CLR_RGBA(clr) clr.r, clr.g, clr.b, clr.a
#define CLR_WHITE (struct SDL_Colour){ 0xFF, 0xFF, 0xFF, 0xFF }
#define CLR_BLACK (struct SDL_Colour){ 0x00, 0x00, 0x00, 0xFF }
#define CLR_RED (struct SDL_Colour){ 0xFF, 0x41, 0x36, 0xFF }
#define CLR_GREEN (struct SDL_Colour){ 0x40, 0xFF, 0x80, 0xFF }
#define CLR_BLUE (struct SDL_Colour){ 0x00, 0x74, 0xD9, 0xFF }
#define CLR_LIGHT_GREY (struct SDL_Colour){ 0xDD, 0xDD, 0xDD, 0xFF }
#define CLR_DARK_GREY (struct SDL_Colour){ 0xAA, 0xAA, 0xAA, 0xFF }

void err_msg(const char *msg);
void draw_text(int x, int y, char *str, SDL_Color clr);
void cls();

#define brd_set(x, y, n) *(g_board + (y*BOARD_WIDTH) + x) = n
#define brd_get(x, y) *(g_board + (y*BOARD_WIDTH) + x)
void brd_randomise();
void brd_paint(int x, int y, int radius);
void brd_process();

Uint32 timer_callback(Uint32 interval, void* name);

SDL_Window *g_window = NULL;
SDL_Renderer *g_renderer = NULL;

Uint8 *g_board;
Uint8 g_cell_colours[COLOUR_COUNT][4] = {
	{ 0xFF, 0x41, 0x36, 0xFF },	// Red
	{ 0xFF, 0x85, 0x1B, 0xFF },	// Orange
	{ 0xFF, 0xDC, 0x00, 0xFF },	// Yellow
	{ 0x2E, 0xCC, 0x40, 0xFF },	// Green
	{ 0x39, 0xCC, 0xCC, 0xFF },	// Cyan
	{ 0x00, 0x74, 0xD9, 0xFF },	// Blue
	{ 0xB1, 0x0D, 0xC9, 0xFF },	// Purple
	{ 0xF0, 0x12, 0xBE, 0xFF },	// Fuchsia
};
int g_temperature = 2;

int main(int argc, char* args[]) {
	srand(time(NULL));
	g_board = malloc(sizeof(Uint8) * BOARD_WIDTH * BOARD_HEIGHT);

	// Initialisation
	if (SDL_Init(SDL_INIT_VIDEO
		| SDL_INIT_TIMER
	) < 0) err_msg("Failed to initialise SDL");
	g_window = SDL_CreateWindow(
		"Automaton",
		SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
		SCREEN_WIDTH, SCREEN_HEIGHT,
		SDL_WINDOW_SHOWN
		//| SDL_WINDOW_FULLSCREEN
	);
	if (g_window == NULL) err_msg("Failed to create Window");
	g_renderer = SDL_CreateRenderer(g_window, -1, SDL_RENDERER_ACCELERATED);
	if (g_renderer == NULL) err_msg("Failed to create Renderer");
	SDL_TimerID timer_id = SDL_AddTimer(TIMER_INTERVAL, timer_callback, NULL);
	if (timer_id == 0) err_msg("Failed to create timer");

	// Init Board
	brd_randomise();
	int brush_size = 0;

	// Main Loop
	bool isRunning = true;
	bool redraw = true;
	bool processing = false;
	bool mouse_held = false;
	SDL_Event curr_event;
	while (isRunning) {

		// Handle events
		if (SDL_WaitEvent(&curr_event) != 0) {
			switch (curr_event.type) {
				case SDL_QUIT:
					isRunning = false;
				continue;

				case SDL_KEYUP: {
					SDL_KeyCode kc = curr_event.key.keysym.sym;
					switch (kc) {
						case SDLK_BACKSPACE: brd_randomise(); break;
						case SDLK_RETURN: processing = !processing; break;
						case SDLK_KP_PLUS: { if (g_temperature < COLOUR_COUNT) g_temperature++; } break;
						case SDLK_KP_MINUS: { if (g_temperature > 0) g_temperature--; } break;
						default: break;
					};
					fflush(stdout);
					redraw = true;
				} break;

				case SDL_MOUSEBUTTONDOWN: {
					int x = curr_event.motion.x / PIXEL_SIZE;
					int y = curr_event.motion.y / PIXEL_SIZE;
					brd_paint(x, y, brush_size);
					mouse_held = true;
					redraw = true;
				} break;
				case SDL_MOUSEBUTTONUP: mouse_held = false; break;

				case SDL_MOUSEMOTION: {
					if (mouse_held) {
						int x = curr_event.motion.x / PIXEL_SIZE;
						int y = curr_event.motion.y / PIXEL_SIZE;
						brd_paint(x, y, brush_size);
						redraw = true;
					}
				} break;

				case SDL_MOUSEWHEEL: {
					int scroll = curr_event.wheel.y;
					if ((brush_size > 0 && scroll < 0)
						|| (brush_size < 100 && scroll > 0)
					) brush_size += scroll;
				} break;

				case SDL_USEREVENT: {
					if (processing) {
						brd_process();
						redraw = true;
					}
				}
			}

			if (redraw) {
				cls();

				// Render the board
				for (int y=0; y<BOARD_HEIGHT; y++) {
					for (int x=0; x<BOARD_WIDTH; x++) {

						int curr = brd_get(x, y);
						SDL_SetRenderDrawColor(g_renderer,
							g_cell_colours[curr][0],
							g_cell_colours[curr][1],
							g_cell_colours[curr][2],
							g_cell_colours[curr][3]
						);

						#if PIXEL_SIZE == 1
							SDL_RenderDrawPoint(g_renderer, x, y);
						#else
							SDL_RenderFillRect(g_renderer, &(struct SDL_Rect){
								x * PIXEL_SIZE,
								y * PIXEL_SIZE,
								PIXEL_SIZE, PIXEL_SIZE
							});
						#endif

					}
				}
			
				SDL_RenderPresent(g_renderer);
				redraw = false;
			}
		}

	}

	// Termination
	SDL_DestroyRenderer(g_renderer);
	SDL_DestroyWindow(g_window);
	SDL_Quit();
	return 0;
}

void err_msg(const char *msg) {
	printf("[ERROR] %s: %s\n", msg, SDL_GetError());
	exit(1);
}

void cls() {
	SDL_SetRenderDrawColor(g_renderer, CLR_RGBA(CLR_WHITE));
	SDL_RenderClear(g_renderer);
}

void brd_randomise() {
	for (int y=0; y<BOARD_HEIGHT; y++) {
		for (int x=0; x<BOARD_WIDTH; x++) {
			brd_set(x, y, rand() % COLOUR_COUNT);
		}
	}
}

void brd_paint(int x, int y, int radius) {
	int xmin = (x - radius > 0) ? x - radius : 0;
	int xmax = (x + radius < BOARD_WIDTH) ? x + radius : BOARD_WIDTH;
	int ymin = (y - radius > 0) ? y - radius : 0;
	int ymax = (y + radius < BOARD_HEIGHT) ? y + radius : BOARD_HEIGHT;

	for (int v=ymin; v<ymax; v++) {
		for (int u=xmin; u<xmax; u++) {
			brd_set(u, v, 0);
		}
	}
}

void brd_process() {
	Uint8 *new_board = calloc(sizeof(Uint8), BOARD_WIDTH * BOARD_HEIGHT);

	for (int y=0; y<BOARD_HEIGHT; y++) {
		for (int x=0; x<BOARD_WIDTH; x++) {

			// Get current cell
			Uint8 curr = brd_get(x, y);

			// Psychadelic Algo
			// Check random neighbour
			int u = x;
			int v = y;
			while (
				(u == x && v == y)
				|| u < 0 || u > BOARD_WIDTH-1
				|| v < 0 || v > BOARD_HEIGHT-1
			) {
				u = x + ((rand() % 3) - 1);
				v = y + ((rand() % 3) - 1);
			}

			int dom = curr;
			for (int i=1; i<=g_temperature; i++) {
				int next = (curr + i) % COLOUR_COUNT;
				if (brd_get(u, v) == next) dom = (curr + 1) % COLOUR_COUNT;
			}

			*(new_board + (y*BOARD_WIDTH) + x) = dom;
		}
	}

	Uint8 *p = g_board;
	g_board = new_board;
	free(p);
}

Uint32 timer_callback(Uint32 interval, void* name) {
	SDL_Event event;
	SDL_UserEvent userevent;

	userevent.type = SDL_USEREVENT;
	userevent.code = 0x00;
	userevent.data1 = NULL;
	userevent.data2 = NULL;

	event.type = SDL_USEREVENT;
	event.user = userevent;

	SDL_PushEvent(&event);
	return(TIMER_INTERVAL);
}