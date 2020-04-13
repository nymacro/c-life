#include <stdio.h>
#include <string.h>
#include <SDL2/SDL.h>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define BLOCK_WIDTH 8
#define BLOCK_HEIGHT 8
#define ARENA_WIDTH (WINDOW_WIDTH / BLOCK_WIDTH)
#define ARENA_HEIGHT (WINDOW_HEIGHT / BLOCK_HEIGHT)

typedef struct {
  uint8_t cells[ARENA_HEIGHT][ARENA_WIDTH];
} arena_t;

typedef struct {
  uint8_t i;
  arena_t arenas[2];
} life_t;

arena_t *make_arena() {
  arena_t *ptr = (arena_t*)malloc(sizeof(arena_t));
  return ptr;
}

life_t *make_life() {
  life_t *ptr = (life_t*)malloc(sizeof(life_t));
  ptr->i = 0;
  return ptr;
}

#define ARENA_IDX(a, x, y)                              \
  (a).cells[(y) % ARENA_HEIGHT][(x) % ARENA_WIDTH]

#define LIFE_ALIVE_VALUE 255
#define LIFE_IDX(l, x, y)                                       \
  ARENA_IDX((l)->arenas[(l)->i], (x), (y))
#define LIFE_ALIVE(v) ((v) == LIFE_ALIVE_VALUE)

uint8_t life_neighbours(life_t *l, uint8_t x, uint8_t y) {
  uint8_t neighbours = 0;
  for (uint8_t y_ = 0; y_ < 3; ++y_) {
    for (uint8_t x_ = 0; x_ < 3; ++x_) {
      if (x_ == 1 && y_ == 1) continue; /* don't count yourself */

      uint8_t xx = (x + x_ + (ARENA_WIDTH-1)) % ARENA_WIDTH;
      uint8_t yy = (y + y_ + (ARENA_HEIGHT-1)) % ARENA_HEIGHT;
      if (LIFE_ALIVE(LIFE_IDX(l, xx, yy))) {
        ++neighbours;
      }
    }
  }
  return neighbours;
}

#define LIFE_DRAIN_AMOUNT 16
#define LIFE_DRAIN(i) (((i) > 0) ? ((LIFE_DRAIN_AMOUNT < (i)) ? (i)-LIFE_DRAIN_AMOUNT : 0) : 0)

void life_tick(life_t *l) {
  arena_t *next = &l->arenas[(l->i+1) % 2];
  for (uint8_t y = 0; y < ARENA_HEIGHT; ++y) {
    for (uint8_t x = 0; x < ARENA_WIDTH; ++x) {
      uint8_t neighbours = life_neighbours(l, x, y);
      uint8_t alive = LIFE_IDX(l, x, y);
      uint8_t alivep = LIFE_ALIVE(alive);

      /* get cell for next state */
      uint8_t *cell = &ARENA_IDX(*next, x, y);

      if (alivep && neighbours < 2) {
        *cell = LIFE_DRAIN(alive);
      } else if (alivep && neighbours > 3) {
        *cell = LIFE_DRAIN(alive);
      } else if (!alivep && neighbours == 3) {
          *cell = LIFE_ALIVE_VALUE;
      } else {
        if (alivep) {
          *cell = alive;
        } else {
          *cell = LIFE_DRAIN(alive);
        }
      }
    }
  }

  l->i = (l->i+1) % 2;
}

void life_randomize(life_t *l) {
  for (uint8_t y = 0; y < ARENA_HEIGHT; ++y) {
    for (uint8_t x = 0; x < ARENA_WIDTH; ++x) {
      LIFE_IDX(l, x, y) = (rand() % 2) ? 255 : 0;
    }
  }
}

void life_render(life_t *l, SDL_Renderer *r) {
  SDL_Rect rect;
  for (uint8_t y = 0; y < ARENA_HEIGHT; ++y) {
    for (uint8_t x = 0; x < ARENA_WIDTH; ++x) {
      uint8_t i = LIFE_IDX(l, x, y);
      SDL_SetRenderDrawColor(r, i, 0, 0, 255);
      rect = (SDL_Rect){ x * BLOCK_WIDTH, y * BLOCK_HEIGHT, BLOCK_WIDTH, BLOCK_HEIGHT };
      SDL_RenderFillRect(r, &rect);
    }
  }
}

void life(life_t *l) {
  uint8_t pause = 0, fullscreen = 0;
  SDL_Window *window = SDL_CreateWindow("Game of Life",
                                        0, 0,
                                        WINDOW_WIDTH, WINDOW_HEIGHT,
                                        0);
  if (!window) abort();
  SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
  if (!renderer) abort();

  do {
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
      switch (e.type) {
      case SDL_QUIT:
        printf("quitting\n");
        goto done;
      case SDL_KEYDOWN:
        printf("keydown\n");
        switch (e.key.keysym.sym) {
        case SDLK_ESCAPE: goto done;
        case SDLK_f:
          fullscreen = !fullscreen;
          SDL_SetWindowFullscreen(window, fullscreen ? SDL_TRUE : SDL_FALSE);
          break;
        case SDLK_RETURN:
          life_randomize(l);
          break;
        case SDLK_SPACE:
          pause = !pause;
          break;
        default: break;
        }
      default: break;
      }
    }

    life_render(l, renderer);
    SDL_RenderPresent(renderer);

    if (!pause)
      life_tick(l);

    //SDL_Delay(30);
  } while(1);

 done:
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
}

int main(int argc, char *argv[]) {
  SDL_Init(SDL_INIT_VIDEO);
  life_t *l = make_life();
  life_randomize(l);
  life(l);
  return 0;
}
