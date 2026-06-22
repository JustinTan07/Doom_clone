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
#define SCREEN_WIDTH 800 
#define SCREEN_HEIGHT 600 
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
float castRay(player* player, int map[8][8]);
void drawPlayer(player *player, SDL_Renderer * renderer, int map[8][8]); 




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
    int oldPlayerX = p1.x;
    int oldPlayerY = p1.y;
    SDL_RenderClear(game.renderer);
    const Uint8 *keys = SDL_GetKeyboardState(NULL);
    if(keys[SDL_SCANCODE_Q])
    {
        p1.angle+=0.05;
    }
    if(keys[SDL_SCANCODE_E])
    {
        p1.angle-=0.05;
    }
    if (keys[SDL_SCANCODE_W]) // Changed movement so that its dependent on players camera angle basically
    {
        p1.y += 3*sin(p1.angle);
        p1.x+= 3*cos(p1.angle);
        int newPlayerRow = (p1.y - 10) / 75;  // check top edge
        int newPlayerCol = p1.x / 100;
        if (map[newPlayerRow][newPlayerCol] == 1)
        {
            p1.y = oldPlayerY;  // restore if wall
            p1.x = oldPlayerX;
        }
    }
    if (keys[SDL_SCANCODE_S])
    {
        p1.y -= 3*sin(p1.angle);
        p1.x-= 3*cos(p1.angle);

        int newPlayerRow = (p1.y + 10) / 75;  // check bottom edge
        int newPlayerCol = p1.x / 100;
    
        if (map[newPlayerRow][newPlayerCol] == 1)
        {
            p1.y = oldPlayerY;
            p1.x = oldPlayerX;
        }
    }
    if (keys[SDL_SCANCODE_A])
    {
        p1.x += 3*sin(p1.angle);
        p1.y -=3*cos(p1.angle);
        int newPlayerRow = p1.y / 75;
        int newPlayerCol = (p1.x -10) / 100;  // check left edge
        if (map[newPlayerRow][newPlayerCol] == 1)
        {
            p1.x = oldPlayerX;
            p1.y = oldPlayerY;
        }

    }
    if (keys[SDL_SCANCODE_D])
    {
        p1.x -= 3*sin(p1.angle);
        p1.y += 3*cos(p1.angle);
        int newPlayerRow = p1.y / 75;
        int newPlayerCol = (p1.x + 10) / 100;  // check right edge
        if (map[newPlayerRow][newPlayerCol] == 1)
        {
            p1.x = oldPlayerX;
            p1.y= oldPlayerY;
        }
    }

    int newPlayerRow = (p1.y+10)/75;
    int newPlayerCol = (p1.x+10)/100;

    if(newPlayerRow >=0 && newPlayerRow < 8 && newPlayerCol >=0 && newPlayerCol < 8)
    {
        if(map[newPlayerRow][newPlayerCol] ==1 )
        {
            
            p1.x= oldPlayerX;
            p1.y= oldPlayerY;
        }
    }



    for(int i = 0; i < 8;i++)
    {
        for(int j = 0; j < 8;j++)
        {
            SDL_Rect temp;
            temp.x = j*100;
            temp.y = i*75;
            temp.w = 100;
            temp.h = 75;
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

float castRay(player* player,int map[8][8])
    {
        float lineX = cos(player->angle);
        float lineY = sin(player->angle);
        int playerCol = player->x / 100;
        int playerRow = player->y / 75;
        float deltaDirX = fabs(1/lineX);
        float deltaDirY = fabs(1/lineY);
        int stepX;
        int stepY;
        float sideDistanceX,sideDistanceY;
        int side;

        if(lineX > 0)
        {
            stepX=1;  
            sideDistanceX = deltaDirX *((playerCol + 1) * 100 - player->x);

        }
        else
        {
            stepX= -1;
            sideDistanceX = deltaDirX * (player->x - playerCol * 100); 
        }
        if(lineY > 0)
        {
            stepY=1; 
            sideDistanceY = deltaDirY *((playerRow + 1 ) * 75 - player->y);
            
        }
        else
        {
            stepY=-1;
            sideDistanceY = deltaDirY * (player->y - playerRow * 75);
        }
    
        bool hit = false;
        while(!hit)
        {
            if(sideDistanceX < sideDistanceY)
            {
                playerCol+=stepX;
                sideDistanceX += deltaDirX;
                side = 0;
                if(map[playerRow][playerCol] ==1) 
                {
                    hit = true;
                }
            }
            else
            {
                playerRow+=stepY;
                sideDistanceY += deltaDirY;
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
                wallX = (playerCol)*100;
            }
            else
            {
                wallX = (playerCol+1)*100;
            }
             wallY = player->y + ((wallX - player->x) / lineX) * lineY;
        }
        else
        {
            if(lineY > 0 )
            {
                wallY = playerRow * 75;
            }
            else
            {
                wallY = (playerRow+1)*75;
            }
            wallX = player->x + ((wallY- player->y) / lineY) * lineX;
            
        }
        float playerWallDist = sqrt((wallX - player->x)*(wallX - player->x) + (wallY-player->y)*(wallY-player->y));
        printf("side=%d lineX=%.2f lineY=%.2f playerCol=%d playerRow=%d wallX=%.2f wallY=%.2f dist=%.2f\n",
    side, lineX, lineY, playerCol, playerRow, wallX, wallY, playerWallDist);
        return playerWallDist;

}

void drawPlayer(player *player, SDL_Renderer * renderer, int map[8][8])
{
    SDL_Rect temp;
    temp.x = player->x;
    temp.y = player->y;
    temp.w =  10;
    temp.h = 10;
    SDL_RenderDrawRect(renderer, &temp);
    SDL_SetRenderDrawColor(renderer, 144,238,144,0);
    SDL_RenderFillRect(renderer,&temp);

    // Draw line 
    float dist = castRay(player,map);
    int startX = temp.x+temp.w/2;
    int startY = temp.h/2 + temp.y;
    int endX = startX + cos(player->angle) * dist;
    int endY = startY + sin(player->angle) * dist;
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_RenderDrawLine(renderer,startX,startY,endX,endY);
}


  


