#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_scancode.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_keyboard.h>
#include <math.h>
#define _USE_MATH_DEFINES
#define WINDOW_TITLE "02 Close Window"
#define SCREEN_WIDTH 1600 
#define SCREEN_HEIGHT 600 
#define PLAYER_SIZE 10
#define CELL_WIDTH 100 
#define CELL_HEIGHT 75
#define DRAW_CELL_WIDTH 50 
#define DRAW_CELL_HEIGHT 37.5
#define MAP_HEIGHT 8 
#define MAP_WIDTH 8 
#define IMAGE_FLAGS IMG_INIT_PNG // This represents the bit pattern of a PNG
#define TEXT_SIZE 80 // PT size not pixel size
struct game_header 
{
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Texture* background;   
    TTF_Font *text_font;
    SDL_Color text_color;
    SDL_Rect text_rect;
    SDL_Texture *text_image;

};
typedef struct{
    float x;
    float y;
    float angle;
    float dirX;
    float dirY;
    float planeX;
    float planeY;
} player;

typedef struct game_header Game;

void game_free(Game* game, int exit_status); // Returns the exits status when quiting 
bool load_media(Game* game);
bool sdl_initialize(Game* game); // Initalizes Game Pointer
float castRay(player* player, int map[8][8], float angle);
void drawPlayer(player *player, SDL_Renderer * renderer, int map[8][8]); 
bool isColliding(player* player, int map[8][8]);
void movePlayer(player* player, int map[8][8]);
void renderView(player* player, SDL_Renderer* renderer, int map[8][8]);




int main(int argc, char *argv[])
{
    Game game;
    game.window = NULL;
    game.renderer = NULL;
    game.background = NULL;
    game.text_font = NULL;
    game.text_color = (SDL_Color){255,255,255,255};
    game.text_rect = (SDL_Rect){0,0,0,0};
    game.text_image = NULL;
    int map[8][8] = {
    {1, 1, 1, 1, 1, 1, 1, 1},
    {1, 0, 1, 0, 0, 0, 0, 1},
    {1, 0, 1, 0, 0, 0, 0, 1},
    {1, 0, 1, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 1, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 1},
    {1, 1, 1, 1, 1, 1, 1, 1},
    };

    player p1;
    p1.x = 150;
    p1.y = 150;
    p1.angle = 0;
    p1.dirX = cos(p1.angle); //




    if(sdl_initialize(&game)) // If there is an error we want to return an exit failure 
    {
        game_free(&game,EXIT_FAILURE);
        exit(1);
    }
    if(load_media(&game)) // If there is an error we want to return an exit failure 
    {
        game_free(&game,EXIT_FAILURE);
        exit(1);
    }

    while(true)
    {
        SDL_Event event;
        while(SDL_PollEvent(&event))
        {
            
            switch(event.type)
            {
                case SDL_QUIT:
                    game_free(&game,EXIT_SUCCESS); 
                    break;
                case SDL_KEYDOWN: 
                    switch(event.key.keysym.scancode)
                    {
                        case SDL_SCANCODE_ESCAPE:
                            game_free(&game,EXIT_SUCCESS);
                            break;
                        default:
                            break;
                    }
                default:
                    break;
            }
        }
    float oldPlayerX = p1.x;
    float oldPlayerY = p1.y;
    movePlayer(&p1,map);

    int newPlayerRow = (p1.y)/CELL_HEIGHT;
    int newPlayerCol = (p1.x)/CELL_WIDTH;

    if(newPlayerRow >=0 && newPlayerRow < 8 && newPlayerCol >=0 && newPlayerCol < 8)
    {
        if(map[newPlayerRow][newPlayerCol] ==1 )
        {
            
            p1.x= oldPlayerX;
            p1.y= oldPlayerY;
        }
    }

    SDL_SetRenderDrawColor(game.renderer, 0, 0, 0, 255);
    SDL_RenderClear(game.renderer);

    for(int i = 0; i < MAP_HEIGHT;i++)
    {
        for(int j = 0; j < MAP_WIDTH;j++)
        {
            SDL_Rect temp;
            temp.x = j*CELL_WIDTH;
            temp.y = i*CELL_HEIGHT;
            temp.w = CELL_WIDTH;
            temp.h = CELL_HEIGHT;
            SDL_RenderDrawRect(game.renderer,&temp);
            if(map[i][j] ==1)
            {
                SDL_SetRenderDrawColor(game.renderer,0,0,0,0);
                SDL_RenderFillRect(game.renderer,&temp);
                
            }
            else 
            {
                SDL_SetRenderDrawColor(game.renderer,255,200,255,255);
                SDL_RenderFillRect(game.renderer,&temp);
            }
            SDL_SetRenderDrawColor(game.renderer, 255, 255, 255, 255);
            SDL_RenderDrawRect(game.renderer, &temp);
        }

    }
        drawPlayer(&p1,game.renderer,map);
        renderView(&p1, game.renderer,map);
        SDL_RenderPresent(game.renderer);
        SDL_Delay(16);
    }   

    

    game_free(&game, EXIT_SUCCESS);
    printf("Passed all Testcases!!");
    return 0;
}   

void game_free(Game* game,int exit_status)
{   
    SDL_DestroyTexture(game->text_image);
    TTF_CloseFont(game->text_font); // Frees font
    SDL_DestroyRenderer(game->renderer); // Free the renderer 
    SDL_DestroyWindow(game->window); // Free the window
    SDL_DestroyTexture(game->background); // Free the background/Texture
    TTF_Quit();
    IMG_Quit(); // Quits the image
    SDL_Quit(); // quit SDL
    exit(exit_status);

}

bool sdl_initialize(Game* game) // Iniatlizes a Game renderer and game Window. It returns true if theres an error and false if there is no error
{
    if(SDL_Init(SDL_INIT_EVERYTHING))
    {
        fprintf(stderr,"Error initalizing SDL: %s \n",SDL_GetError()); 
        return true;
    }

    int img_init = IMG_Init(IMAGE_FLAGS);
    if((img_init & IMAGE_FLAGS) != IMAGE_FLAGS)
    {
        fprintf(stderr,"Error initalizing SDL: %s \n",IMG_GetError()); 
        return true; 
    }
    if(TTF_Init()== -1)
    {
        fprintf(stderr,"Error initalizing SDL_TTF: %s \n",SDL_GetError()); 
        return true; 
    }

    game->window = SDL_CreateWindow(WINDOW_TITLE,SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,SCREEN_WIDTH,SCREEN_HEIGHT,0); // Creates Window with Centered size and screen width/height
    if(game->window == NULL)
    {
        fprintf(stderr,"Error creating window: %s \n",SDL_GetError());
        return true;
    }
    game->renderer = SDL_CreateRenderer(game->window, -1,0);
    if(game->renderer == NULL)
    {
        fprintf(stderr,"Error creating renderer: %s \n",SDL_GetError());
        return true;
    }
    return false;

}
bool load_media(Game* game)
{
    game->background = IMG_LoadTexture(game->renderer,"../images/SDL2_Background2.png");
    if(game->background == NULL)
    {
        fprintf(stderr,"Error creating Texture: %s \n",IMG_GetError());
        return true;  
    }
    game->text_font=TTF_OpenFont("../fonts/free-sans-bold.ttf", TEXT_SIZE);
    if(game->text_font == NULL)
    {
        fprintf(stderr,"Error creating fonts: %s \n",TTF_GetError());
        return true;  
    }
    SDL_Surface *surface= TTF_RenderText_Blended(game->text_font,"The New Age", game->text_color);
    if(surface == NULL)
    {
        fprintf(stderr,"Error creating surface: %s \n",SDL_GetError());
        return true;  
    }
    game->text_rect.w = surface->w;
    game->text_rect.h = surface->h;
    game->text_image = SDL_CreateTextureFromSurface(game->renderer,surface);
    SDL_FreeSurface(surface);   
    if(game->text_image == NULL)
    {
        fprintf(stderr,"Error creating texture: %s \n",SDL_GetError());
        return true; 
    }
    

    return false;
}   

float castRay(player* player,int map[8][8],float angle)
{
        float lineX = cos(angle); // This the amount of pixels the ray moves in X for every one unit of ray travel
        float lineY = sin(angle); // This gets direction of Players Y 
        int playerCol = (player->x+5) / CELL_WIDTH; // This show current Column of player 
        int playerRow = (player->y+5) / CELL_HEIGHT; // This shows current Row of Player
        float deltaDirX = fabs(1/lineX); // How far the ray travels to cross one pixel
        float deltaDirY = fabs(1/lineY);
        int stepX;
        int stepY;
        float sideDistanceX,sideDistanceY;
        int side;

        if(lineX > 0)
        {
            stepX=1;  
            sideDistanceX = deltaDirX *((playerCol + 1) * CELL_WIDTH - (player->x+5));

        }
        else
        {
            stepX= -1;
            sideDistanceX = deltaDirX * ((player->x+5) - playerCol * CELL_WIDTH); 
        }
        if(lineY > 0)
        {
            stepY=1; 
            sideDistanceY = deltaDirY *((playerRow + 1 ) * CELL_HEIGHT - (player->y+5));
            
        }
        else
        {
            stepY=-1;
            sideDistanceY = deltaDirY * ((player->y+5) - playerRow * CELL_HEIGHT);
        }
    
        bool hit = false;
        while(!hit)
        {
            if(sideDistanceX < sideDistanceY)
            {
                playerCol+=stepX;
                sideDistanceX += deltaDirX*CELL_WIDTH;
                side = 0;
                if(map[playerRow][playerCol] ==1) 
                {
                    hit = true;
                }
            }
            else
            {
                playerRow+=stepY;
                sideDistanceY += deltaDirY*75;
                side = 1;
                if(map[playerRow][playerCol] ==1) 
                {
                    hit = true;
                }
            }
        }
        float wallX,wallY;
        if(side == 0) 
        {
            if(lineX > 0 ) 
            {
                wallX = (playerCol)*CELL_WIDTH;
            }
            else
            {
                wallX = (playerCol+1)*CELL_WIDTH;
            }
             wallY = (player->y+5) + ((wallX - (player->x+5)) / lineX) * lineY;
        }
        else
        {
            if(lineY > 0 )
            {
                wallY = playerRow * CELL_HEIGHT;
            }
            else
            {
                wallY = (playerRow+1)*CELL_HEIGHT;
            }
            wallX = (player->x+5) + ((wallY- (player->y+5)) / lineY) * lineX;
            
        }
        float playerWallDist = sqrt((wallX - (player->x+5))*(wallX - (player->x+5)) + (wallY-(player->y+5))*(wallY-(player->y+5)));
        return playerWallDist;

}

void drawPlayer(player *player, SDL_Renderer * renderer, int map[8][8])
{
    SDL_Rect temp;
    temp.x = player->x;
    temp.y = player->y;
    temp.w =  PLAYER_SIZE;
    temp.h = PLAYER_SIZE;
    SDL_RenderDrawRect(renderer, &temp);
    SDL_SetRenderDrawColor(renderer, 144,238,144,0);
    SDL_RenderFillRect(renderer,&temp);

    // Draw line 
    float dist = castRay(player,map, player->angle);
    int startX = temp.x+temp.w/2;
    int startY = temp.h/2 + temp.y;
    int endX = startX + cos(player->angle) * dist;
    int endY = startY + sin(player->angle) * dist;
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_RenderDrawLine(renderer,startX,startY,endX,endY);
}

void movePlayer(player* player, int map[8][8])
{
    float oldX = player->x;
    float oldY = player->y; 
    const Uint8* keys = SDL_GetKeyboardState(NULL);

    if(keys[SDL_SCANCODE_W])
    {
        player->x += 3 * cos(player->angle);
        player->y += 3 * sin(player->angle);
    }
    if(keys[SDL_SCANCODE_S])
    {
        player->x -= 3 * cos(player->angle);
        player->y -= 3 * sin(player->angle);
    }
    if(keys[SDL_SCANCODE_A])
    {
        player->x += 3 * sin(player->angle);
        player->y -= 3 * cos(player->angle);
    }
    if(keys[SDL_SCANCODE_D])
    {
        player->x -= 3 * sin(player->angle);
        player->y += 3 * cos(player->angle);
    }
    if(keys[SDL_SCANCODE_Q])
    {
        player->angle+=0.05;
    }
    if(keys[SDL_SCANCODE_E])
    {
        player->angle-=0.05;
    }


    if(isColliding(player, map))
    {
        player->x = oldX;
        player->y = oldY;
    }
}

bool isColliding(player* player, int map[8][8])
{
    int topRow = player->y/CELL_HEIGHT;
    int botRow = (player->y+PLAYER_SIZE) / CELL_HEIGHT;
    int leftCol = player->x / CELL_WIDTH;
    int rightCol = (player->x+PLAYER_SIZE) / CELL_WIDTH;

    if(map[topRow][leftCol] == 1 || map[topRow][rightCol] ==1  || map[botRow][leftCol] ==1  || map[botRow][rightCol] ==1) 
    {
        return true;
    }
    return false;
}

void renderView(player* player,  SDL_Renderer* renderer, int map[8][8])
{
    float fov = M_PI /3;
    for(int col = 0; col < SCREEN_WIDTH/2;col++) 
    {
        float angleOffset = ((float)col / (SCREEN_WIDTH/2))* fov;
        float rayAngle = (player->angle - fov/2) + angleOffset;
        float dist = castRay(player, map,rayAngle);
        float wallHeight = (SCREEN_HEIGHT*CELL_WIDTH)/dist;
        float drawStart = (SCREEN_HEIGHT/2) - (wallHeight/2);
        float drawEnd = (SCREEN_HEIGHT/2) + (wallHeight/2);
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderDrawLine(renderer,col+ SCREEN_WIDTH/2,drawStart,col+ SCREEN_WIDTH/2,drawEnd);

    }
}




  


