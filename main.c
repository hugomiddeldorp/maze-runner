/************************************************
* 
* Maze-Runner: A simple game where the objective
* is to make it to the other side of the maze.
* Each level is randomly generated.
* 
* Author: Hugo Middeldorp
* 
************************************************/

#include <stdio.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <time.h>
#include <unistd.h>

/* Algorithm starts from the middle, so best to have odd width and height */
#define GRID_WIDTH 11
#define GRID_HEIGHT 11

#define CELL_WIDTH 48
#define CELL_BORDER 8
#define STEP 14 // This needs to be a multiple of CELL_WIDTH+CELL_BORDER
#define PLAYER_WIDTH CELL_WIDTH

/* Window size is defined by the size of the grid */
#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720
#define XOFFSET 332
#define YOFFSET 52

/* COLOURS */
int C1[3] = { 13, 43, 69 };
int C2[3] =  { 32, 60, 86 };
int C3[3] =  { 84, 78, 104 };
int C4[3] =  { 141, 105, 122 };
int C5[3] =  { 208, 129, 89 };
int C6[3] =  { 255, 170, 94 };
int C7[3] =  { 255, 212, 163 };
int C8[3] =  { 255, 236, 214 };


typedef struct {
    int current_i, current_j, next_i, next_j, x, y;
} Player;


typedef struct {
    int x, y;
    char walls[4]; // TOP, RIGHT, BOTTOM, LEFT
    bool visited;
} Cell;


int getRandom(int range) {
    return rand() % range;
}


/* processEvents takes the *pointer* of the window structure, and the plater
 * and returns if the game is done
 * If the event is closing the window, done is set to 1 */
int processEvents(SDL_Window *window, Cell grid[][GRID_WIDTH],
        Player *player, char game_state) {
    SDL_Event event;

    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_WINDOWEVENT_CLOSE:
                if (window) {
                    SDL_DestroyWindow(window);
                    game_state = 'D';
                }
                break;
            case SDL_QUIT:
                game_state = 'D';
                break;
            case SDL_KEYDOWN:
                if (event.key.repeat == 0) {
                    switch (event.key.keysym.sym) {
                        if (game_state == 'G') {
                           case SDLK_UP:
                              movePlayer(grid, player, 'T');
                               break;
                           case SDLK_RIGHT:
                               movePlayer(grid, player, 'R');
                               break;
                           case SDLK_DOWN:
                               movePlayer(grid, player, 'B');
                               break;
                           case SDLK_LEFT:
                               movePlayer(grid, player, 'L');
                               break;
                       }
                       case SDLK_s:
                           if (game_state == 'S') { game_state = 'G'; }
                           break;
                       case SDLK_r:
                           if (game_state == 'W') { game_state = 'G'; }
                           break;
                    }
                }
               
        }
    }
    
    return game_state;
}


/* Takes the grid, the player and the direction and changes the next_i or
 * next_j based on the direction, the grid limits and the presence of walls */
void movePlayer(Cell grid[][GRID_WIDTH], Player *player, char direction) {
    if (direction == 'T' && player->next_j > 0
            && !grid[player->next_j][player->next_i].walls[0]) {
        --player->next_j;
    }
    else if (direction == 'R' && player->next_i < GRID_WIDTH
            && !grid[player->next_j][player->next_i].walls[1]) {
        ++player->next_i;
    }
    else if (direction == 'B' && player->next_j < GRID_HEIGHT
            && !grid[player->next_j][player->next_i].walls[2]) {
        ++player->next_j;
    }
    else if (direction == 'L' && player->next_i > 0
            && !grid[player->next_j][player->next_i].walls[3]) {
        --player->next_i;
    }
}


/* Take the player and move its x and y if those coordinates don't match
 * the coordinates of the next cell. Once they match, set the next cell to
 * the current cell. */
void playerMovement(Player *player) {
    int next_x = XOFFSET+(CELL_WIDTH+CELL_BORDER)*player->next_i;
    int next_y = YOFFSET+(CELL_WIDTH+CELL_BORDER)*player->next_j;
    
    if (player->x == XOFFSET+(CELL_WIDTH+CELL_BORDER)*player->next_i) {
        player->current_i = player->next_i;
    }
    else if ( player->x < next_x) {
        player->x += STEP; // Be careful of how this lines up with CELL_WIDTH
    }
    else if ( player->x > next_x) {
        player->x -= STEP; 
    }
    if (player->y == YOFFSET+(CELL_WIDTH+CELL_BORDER)*player->next_j) {
        player->current_j = player->next_j;
    }
    else if ( player->y < next_y) {
        player->y += STEP;
    }
    else if ( player->y > next_y) {
        player->y -= STEP;
    }
}


/* Take the renderer, a cell, the x and y positions, the width and the border width
 * and draw the walls of the square */
void drawSquare(SDL_Renderer *renderer, Cell cell) {
    int x = XOFFSET+(CELL_WIDTH+CELL_BORDER)*cell.x;
    int y = YOFFSET+(CELL_WIDTH+CELL_BORDER)*cell.y;
    int width = CELL_WIDTH;
    int border = CELL_BORDER;

    SDL_SetRenderDrawColor(renderer, C8[0], C8[1], C8[2], 255);

    /* Check all walls and only draw the ones that exist */
    if (cell.walls[0]){
        SDL_Rect top = { x-border, y-border, width+border*2, border };
        SDL_RenderFillRect(renderer, &top);
    }
    if (cell.walls[1]) {
        SDL_Rect right = { x+width, y-border, border, width+border*2 };
        SDL_RenderFillRect(renderer, &right);
    }
    if (cell.walls[2]) {
        SDL_Rect bottom = { x-border, y+width, width+border*2, border };
        SDL_RenderFillRect(renderer, &bottom);
    }
    if (cell.walls[3]) {
        SDL_Rect left = { x-border, y-border, border, width+border*2 };
        SDL_RenderFillRect(renderer, &left);
    }

}


/* Print the grid in the console */
void printGrid(Cell grid[][GRID_WIDTH]) {
    int i, j;

    for (j = 0; j < GRID_HEIGHT; ++j) {
        for (i = 0; i < GRID_WIDTH; ++i) {
            if (grid[j][i].visited) {
                printf("1 ");
            }
            else {
                printf("0 ");
            }
        }
        printf("\n");
    }
}


/* Fill the grid with cell entities */
void fillGrid(Cell grid[][GRID_WIDTH]) {
    int j, i;

    for (j = 0; j < GRID_HEIGHT; ++j) {
        for (i = 0; i < GRID_WIDTH; ++i) {
            Cell cell = { i, j, { 1, 1, 1, 1 }, false };
            grid[j][i] = cell;
        }
    }
}


void drawTexture(SDL_Renderer *renderer, SDL_Texture *texture, int x, int y,
        int width, int height) {
    SDL_Rect texture_rect = { x, y, width, height };
    SDL_RenderCopy(renderer, texture, NULL, &texture_rect);
}


/* x and y positions will be different anchors based on text_align */
void drawText(SDL_Renderer *renderer, int size, char text[],
        char text_align, int x, int y, int colour[3]) {
    
    TTF_Font *font;
    font = TTF_OpenFont("resources/PressStart2P-Regular.ttf", size);
    if (!font) {
        printf("Failed to load the font:\n%s\n\n", TTF_GetError());
    }
    
    SDL_Surface *text_surface;
    SDL_Color text_colour = { colour[0], colour[1], colour[2]};
    text_surface = TTF_RenderText_Solid(font, text, text_colour);
    if (!text_surface) {
        printf("Failed to render text:\n%s\n\n", TTF_GetError());
    }
    SDL_Texture *text_texture;
    text_texture = SDL_CreateTextureFromSurface(renderer, text_surface);
    SDL_Rect text_rect;

    if (text_align == 'L') {
        SDL_Rect text_rect = { x, y, text_surface->w, text_surface->h };
    }
    else if (text_align == 'C') {
        SDL_Rect text_rect = { x-(text_surface->w/2), y, text_surface->w,
            text_surface->h };
    }
    else {
        SDL_Rect text_rect = { x-(text_surface->w), y, text_surface->w,
            text_surface->h };
    }

    if (SDL_RenderCopy(renderer, text_texture, NULL, &text_rect) != 0) {
        printf("Error! %s\n", SDL_GetError());
    }

    SDL_DestroyTexture(text_texture);
    SDL_FreeSurface(text_surface);
    TTF_CloseFont(font);
}


/* Go through each item in the grid and call drawSquare for it */
void drawGrid(SDL_Renderer *renderer, Cell grid[][GRID_WIDTH]) {
    int i, j;

    for (j = 0; j < GRID_HEIGHT; ++j) {
        for (i = 0; i < GRID_WIDTH; ++i) {
            drawSquare(renderer, grid[j][i]);
        }
    }
}


/* Using iterative depth-first search to generate the maze */
void makeMaze(Cell grid[][GRID_WIDTH]) {
    
    fillGrid(grid);

    Cell stack[GRID_HEIGHT*GRID_WIDTH];

    int index = 0;
    char directions[4];
    int valid_directions;

    // initial cell
    grid[GRID_HEIGHT/2][GRID_WIDTH/2].visited = true;
    stack[0] = grid[GRID_HEIGHT/2][GRID_WIDTH/2];
    Cell current = stack[index];

    /* Check for valid next cell */
    while (index >= 0) {
        current = stack[index];
        valid_directions = 0;
        /* If the cell is within bounds and hasn't been visited
         * Then add the direction as valid and add to the count of directions */
        /* TOP-RIGHT-BOTTOM-LEFT */
        if (current.y-1 >= 0 && !grid[current.y-1][current.x].visited) {
            directions[valid_directions] = 'T';
            ++valid_directions;
        }
        if (current.x+1 < GRID_WIDTH && !grid[current.y][current.x+1].visited) {
            directions[valid_directions] = 'R';
            ++valid_directions;
        }
        if (current.y+1 < GRID_HEIGHT && !grid[current.y+1][current.x].visited) {
            directions[valid_directions] = 'B';
            ++valid_directions;
        }
        if (current.x-1 >= 0 && !grid[current.y][current.x-1].visited) {
            directions[valid_directions] = 'L';
            ++valid_directions;
        }
        
        /* If there is a valid neighbour, pick a random direction, otherwise
         * -1 from index. */
        if (valid_directions > 0) {
            char random_direction = directions[getRandom(valid_directions)];

            /* Break the walls, mark as current and mark as visited */
            if (random_direction == 'T') {
                grid[current.y][current.x].walls[0] = 0;
                current = grid[current.y-1][current.x];
                grid[current.y][current.x].walls[2] = 0;
                grid[current.y][current.x].visited = true;
            }
            if (random_direction == 'R') {
                grid[current.y][current.x].walls[1] = 0;
                current = grid[current.y][current.x+1];
                grid[current.y][current.x].walls[3] = 0;
                grid[current.y][current.x].visited = true;
            }
            if (random_direction == 'B') {
                grid[current.y][current.x].walls[2] = 0;
                current = grid[current.y+1][current.x];
                grid[current.y][current.x].walls[0] = 0;
                grid[current.y][current.x].visited = true;
            }
            if (random_direction == 'L') {
                grid[current.y][current.x].walls[3] = 0;
                current = grid[current.y][current.x-1];
                grid[current.y][current.x].walls[1] = 0;
                grid[current.y][current.x].visited = true;
            }

            ++index;
            stack[index] = current;

        }
        else {
            --index;
        }

    }
}


/* Initialising SDL and game variables */
SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;
SDL_Surface *key_surface = NULL;
SDL_Surface *bg_surface = NULL;
SDL_Surface *player_surface = NULL;
SDL_Surface *title_surface = NULL;

Cell grid[GRID_HEIGHT][GRID_WIDTH];


/* Gives the values to all the game variables */
int gameStart() {

    /* Set the seed for random */
    int seed = time(NULL);
    printf("Seed: %d\n", seed);
    srand(seed);

    SDL_Init(SDL_INIT_VIDEO);
    TTF_Init();

    window = SDL_CreateWindow("Maze Runner", 
            SDL_WINDOWPOS_UNDEFINED,
            SDL_WINDOWPOS_UNDEFINED,
            SCREEN_WIDTH,
            SCREEN_HEIGHT,
            0);

    renderer = SDL_CreateRenderer(window, -1,
            SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    key_surface = IMG_Load("resources/key.png");
    bg_surface = IMG_Load("resources/background.png");
    player_surface = IMG_Load("resources/char.png");
    title_surface = IMG_Load("resources/title.png");

    // TODO ADD MORE ERROR MESSAGES
    if (!key_surface) {
        printf("Cannot find key.png\n\n");
        SDL_Quit();
        return 1;
    }
    
    return 0;
}


int main() {

    /* Game setup */
    gameStart();

    /* Load surfaces into textures */
    SDL_Texture *key = SDL_CreateTextureFromSurface(renderer, key_surface);
    SDL_FreeSurface(key_surface);

    SDL_Texture *bg = SDL_CreateTextureFromSurface(renderer, bg_surface);
    SDL_FreeSurface(bg_surface);

    SDL_Texture *player_texture = SDL_CreateTextureFromSurface(renderer,
                                    player_surface);
    SDL_FreeSurface(player_surface);

    SDL_Texture *title = SDL_CreateTextureFromSurface(renderer, title_surface);
    SDL_FreeSurface(title_surface);

    /* Set player location in maze */
    Player player = { 0, 0, 0, 0, XOFFSET, YOFFSET };
    
    // Main game loop
    char game_state = 'S'; /* Start, Game, Win, Done */
    float timer_start;
    char timer_string[16];


    while(game_state != 'D') {

        /* Clear Window */
        SDL_SetRenderDrawColor(renderer, C1[0], C1[1], C1[2], 255);
        SDL_RenderClear(renderer);

        /* Events */
        game_state = processEvents(window, grid, &player, game_state);

        /* Start Screen */
        if (game_state == 'S') {
            drawTexture(renderer, bg, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
            drawTexture(renderer, title, (SCREEN_WIDTH/2-696/2), 200, 696, 64);
            drawText(renderer, 24, "Press 'S' to start", 'C', SCREEN_WIDTH/2,
                    SCREEN_HEIGHT/2, C6);
            
            timer_start = SDL_GetTicks();
        }

        /* Game */
        else if (game_state == 'G') {

            /* Timer */
            float timer_end = SDL_GetTicks();
            float timer = (timer_end - timer_start) / 1000;
            if (timer < 0.1) {
                player.current_i = player.current_j = player.next_i = player.next_j = 0;
                player.x = XOFFSET;
                player.y = YOFFSET;
                makeMaze(grid);
            }
            sprintf(timer_string, "%g", timer);

            /* Logic */

            if (player.current_j != player.next_j || player.current_i != player.next_i ) {
                playerMovement(&player);
            }

            if (player.current_j == GRID_HEIGHT-1
                    && player.current_i == GRID_WIDTH-1) {
                printf("Success!\n");
                game_state = 'W';
            }
            

            /* Update */
            drawText(renderer, 16, timer_string, 'L', 10, 10, C7);
            drawGrid(renderer, grid);
            drawTexture(renderer, player_texture, player.x, player.y,
                    CELL_WIDTH, CELL_WIDTH);
            drawTexture(renderer, key, XOFFSET+(GRID_WIDTH-1)*(CELL_WIDTH+CELL_BORDER),
                    YOFFSET+(GRID_HEIGHT-1)*(CELL_WIDTH+CELL_BORDER),
                    CELL_WIDTH, CELL_WIDTH);

        }

        /* Win */
        else if (game_state == 'W') {
            drawTexture(renderer, bg, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
            drawText(renderer, 48, "You Win!", 'C', SCREEN_WIDTH/2, 100, C6);
            drawText(renderer, 64, timer_string, 'C',   SCREEN_WIDTH/2, SCREEN_HEIGHT/2, C5);
            drawText(renderer, 24, "Press 'R' to restart", 'C', SCREEN_WIDTH/2, SCREEN_HEIGHT-200, C3);
            
            timer_start = SDL_GetTicks();
        }

        /* Present the updates */
        SDL_RenderPresent(renderer);

    }

    /* Cleanup */
    SDL_DestroyTexture(key);
    SDL_DestroyTexture(player_texture);
    SDL_DestroyTexture(title);
    SDL_DestroyTexture(bg);
    SDL_DestroyWindow(window);

    SDL_Quit();
    
	return 0;
}

