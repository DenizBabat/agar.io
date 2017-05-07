#include <iostream>
#include <ctime>
#include <allegro5\allegro.h>
#include <allegro5\allegro_primitives.h>
#include <allegro5\allegro_font.h>
#include <allegro5\allegro_ttf.h>
#include "object.h"


//GLOBALS-------------------------------------------------------------
const int WIDTH = 1366;
const int HEIGHT = 768;
const int NUM_FOODS = 200;
const int NUM_ENEMY = 5;
const int FPS = 30;
//FUNC-----------------------------------------------------------------
void InitAgar(MyAgar &agar);
void DrawAgar(MyAgar &agar);
void UpdateAgar(MyAgar &agar);

void InitFood(MyFood food[], int size);
void DrawFood(MyFood food[], int size);
void UpdateFood(MyFood food[], int size);
void EatFood(MyFood food[], int size, MyAgar &agar, BadEnemy enemy[], int size_enemy);
void EatEnemy_or_EatMe(MyAgar &agar, BadEnemy enemy[], int size_enemy);

void InitEnemy(BadEnemy enemy[], int size);
void DrawEnemy(BadEnemy enemy[], int size);
void StartMoveEnemy(BadEnemy enemy[], int size);
void MoveEnemy(BadEnemy enemy[], int size);

void DropedSpeedAgarAndEnemy(BadEnemy enemy[], MyAgar &agar, int size);

int main()
{
    bool done = false;
    bool redraw = true;


    // object variables
    MyFood foods[NUM_FOODS];
    MyAgar agar;
    BadEnemy enemy[NUM_ENEMY];

    ALLEGRO_DISPLAY *display = NULL;
    ALLEGRO_EVENT_QUEUE *event_queue=NULL;
    ALLEGRO_TIMER *timer=NULL;


    if(!al_init())
        return -1;

    display = al_create_display(WIDTH,HEIGHT);

    if(!display)
        return -1;

    al_init_primitives_addon();
    al_install_mouse();
    al_init_font_addon();
	al_init_ttf_addon();
    ALLEGRO_FONT *font18 = al_load_font("arial.ttf", 18, 0);

    event_queue = al_create_event_queue();
    timer = al_create_timer(1.0/FPS);

    al_register_event_source(event_queue, al_get_mouse_event_source());
    al_register_event_source(event_queue, al_get_timer_event_source(timer));
    al_register_event_source(event_queue, al_get_display_event_source(display));

    srand(time(NULL));
    InitFood(foods,NUM_FOODS);
    InitAgar(agar);
    InitEnemy(enemy,NUM_ENEMY);

    al_start_timer(timer);
    while(!done)
    {
        ALLEGRO_EVENT ev;
        al_wait_for_event(event_queue,&ev);

      //  al_draw_textf(font18, al_map_rgb(255, 0, 255), 400, 230, ALLEGRO_ALIGN_CENTRE,"X==> %i; Y==> %i",pos_x,pos_y);
        if(ev.type == ALLEGRO_EVENT_TIMER)
        {
            redraw = true;
            UpdateAgar(agar);
            UpdateFood(foods, NUM_FOODS);
            MoveEnemy(enemy, NUM_ENEMY);

        }

        if( ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE ){
            done = true;
        }
        else if(ev.type == ALLEGRO_EVENT_MOUSE_AXES){
            /*agar.x = ev.mouse.x ;
            agar.y = ev.mouse.y*/
            if(ev.mouse.x > agar.x)
            {
                agar.x += agar.speed;

            }
            else if(ev.mouse.x < agar.x)
            {
                agar.x -= agar.speed;

            }
            if(ev.mouse.y >agar.y)
            {
                agar.y += agar.speed;

            }
            else if(ev.mouse.y < agar.y)
            {
                agar.y -= agar.speed;

            }
            al_draw_textf(font18, al_map_rgb(255, 0, 255), 300, 230, ALLEGRO_ALIGN_CENTRE," X==> %i; Y==> %i;",agar.x,agar.y);
        }
        else if (ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN){
            if(ev.mouse.button & 2)
                done = true;
        }
        DrawAgar(agar);
        DrawFood(foods,NUM_FOODS);
        EatFood(foods,NUM_FOODS,agar,enemy,NUM_ENEMY);
        DrawEnemy(enemy, NUM_ENEMY);
        StartMoveEnemy(enemy,NUM_ENEMY);
        EatEnemy_or_EatMe(agar, enemy, NUM_ENEMY);
        DropedSpeedAgarAndEnemy(enemy, agar, NUM_ENEMY );
        al_draw_textf(font18, al_map_rgb(255, 0, 255), 300, 430, ALLEGRO_ALIGN_CENTRE," speed==> %f;",agar.speed);

        al_flip_display();
        al_clear_to_color(al_map_rgb(250,250,250));

    }

    al_destroy_display(display);

    return 0;
}
void InitAgar(MyAgar &agar){

     agar.x =  WIDTH/2;
     agar.y = HEIGHT/2;
     agar.ID = PLAYER;
     agar.lives = true;
     agar.diameter = 10;
     agar.score = 0;
     agar.speed = 10.0;
     agar.ForSpeedAgar = 1; // this element reduce speed of agar

 }
 void DrawAgar(MyAgar &agar){
    if(agar.lives == true)
        al_draw_filled_circle(agar.x, agar.y, agar.diameter, al_map_rgb(rand(),250,rand()%50));
}
void UpdateAgar(MyAgar &agar)
{
    if(agar.lives == false)
    {
        if(rand()%100==0)
        {
            agar.lives = true;
        }
    }
}
void InitFood(MyFood food[], int size)
{
    for(int i=0; i<size; ++i)
    {
        food[i].ID = FOOD;
        food[i].live = false;
    }

}
void DrawFood(MyFood food[], int size)
{
    for(int k=0; k<size; ++k)
    {
        if(food[k].live == true)
        {
            al_draw_filled_circle(food[k].x,food[k].y,2,al_map_rgb(rand()%250,rand()%250,rand()%250));

        }
    }
}
void UpdateFood(MyFood food[], int size)
{
    for(int i=0; i<size; ++i)
    {
        if(!food[i].live)
        {
            if(rand()%10 == 0)
            {
                food[i].live = true;
                food[i].x = rand()%WIDTH;
                food[i].y = rand()%HEIGHT;break;
            }
        }
    }
}
void EatFood(MyFood food[], int size, MyAgar &agar, BadEnemy enemy[], int size_enemy){
    for(int i=0; i<size; i++)
    {
        if(agar.x+agar.diameter > food[i].x &&
            agar.x-agar.diameter < food[i].x &&
            agar.y+agar.diameter > food[i].y &&
            agar.y-agar.diameter < food[i].y &&
            food[i].live == true)
        {

            if(agar.diameter<50)
            {
                 agar.diameter += 0.1;
            }
            else if(agar.diameter > 50 && agar.diameter <120)
            {
                agar.diameter +=0.001;
            }
            else
            {
                agar.diameter = 0.0001;
            }

            food[i].live = false;
        }
        for(int k=0; k<size_enemy; ++k)
        if(enemy[k].x+enemy[k].Ediameter > food[i].x &&
            enemy[k].x-enemy[k].Ediameter < food[i].x &&
            enemy[k].y+enemy[k].Ediameter > food[i].y &&
            enemy[k].y-enemy[k].Ediameter < food[i].y &&
            food[i].live == true)
        {

            if(enemy[k].Ediameter<50)
            {
                 enemy[k].Ediameter += 0.1;
            }
            else if(enemy[k].Ediameter > 50 && enemy[k].Ediameter <120)
            {
                enemy[k].Ediameter +=0.001;
            }
            else if(enemy[k].Ediameter >120)
            {
                enemy[k].Ediameter = 0.0001;
            }

            food[i].live = false;
        }
    }
}

void EatEnemy_or_EatMe(MyAgar &agar, BadEnemy enemy[], int size_enemy){
    int i=0;
    for(int k=0; k<size_enemy; ++k)
    {
        if(enemy[k].x+enemy[k].Ediameter > agar.x-agar.diameter &&
            enemy[k].x-enemy[k].Ediameter < agar.x+agar.diameter &&
            enemy[k].y+enemy[k].Ediameter > agar.y-agar.diameter &&
            enemy[k].y-enemy[k].Ediameter < agar.y+agar.diameter &&
            enemy[k].live == true && agar.lives == true)
        {
            if(agar.diameter-enemy[k].Ediameter > 1)            {
                agar.diameter += enemy[k].Ediameter/5;
                enemy[k].live = false;
            }
            else if(enemy[k].Ediameter-agar.diameter > 1)
            {
                enemy[k].Ediameter += agar.diameter/5;
                agar.lives = false;
            }
        }
         while(i<size_enemy)
        {
            if(i==k)
            {
                i++;
            }
            if(enemy[k].x+enemy[k].Ediameter > enemy[i].x-enemy[i].Ediameter &&
            enemy[k].x-enemy[k].Ediameter < enemy[i].x+enemy[i].Ediameter &&
            enemy[k].y+enemy[k].Ediameter > enemy[i].y-enemy[i].Ediameter &&
            enemy[k].y-enemy[k].Ediameter < enemy[i].y+enemy[i].Ediameter &&
            enemy[k].live == true && enemy[i].live == true)
            {
                if(enemy[i].Ediameter-enemy[k].Ediameter > 1)            {
                    enemy[i].Ediameter += enemy[k].Ediameter/5;
                    enemy[k].live = false;
                }
                else if(enemy[k].Ediameter-enemy[i].Ediameter > 1)
                {
                    enemy[k].Ediameter += enemy[i].Ediameter/5;
                    enemy[i].live = false;
                }
            }
            i+=1;
        }

    }
}
void InitEnemy(BadEnemy enemy[], int size){
    for(int i=0; i<size; ++i)
    {
        enemy[i].ID = ENEMY;
        enemy[i].live = false;
        enemy[i].x = 0;
        enemy[i].y = 0;
        enemy[i].speed = 10;
        enemy[i].Ediameter = 10;
        for(int i=0; i<size; ++i)
        {
            enemy[i].ForSpeedEnemy = 1; // this element reduce speed of enemy
        }
    }
}
void DrawEnemy(BadEnemy enemy[], int size){

    for(int i=0; i<size; ++i)
    {
        if(enemy[i].live==true)
            al_draw_filled_circle(enemy[i].x, enemy[i].y,enemy[i].Ediameter,al_map_rgb(50,100,85));
    }
}

void StartMoveEnemy(BadEnemy enemy[], int size)
{
    for(int i=0; i<size; ++i)
    {
        if(!enemy[i].live)
        {
            if(rand()%100 == 0 ){
                enemy[i].live =true;
                enemy[i].x = rand()%WIDTH;
                enemy[i].y = rand()%HEIGHT;
                enemy[i].targetX =  enemy[i].x;
                enemy[i].targetY =  enemy[i].y;
                break;}
            }
        }
}
void MoveEnemy(BadEnemy enemy[], int size)
{
    for(int i=0; i<size; ++i)
    {
        if(rand()%50 == 0)
        {
            enemy[i].targetX = rand()%WIDTH;
            enemy[i].targetY = rand()%HEIGHT;
        }
         else if( enemy[i].targetX == enemy[i].x && enemy[i].targetY == enemy[i].y ){
            enemy[i].targetX = rand()%WIDTH;
            enemy[i].targetY = rand()%HEIGHT;
         }
         else
        {
                if(enemy[i].targetX > enemy[i].x)
            {
                enemy[i].x += enemy[i].speed;
            }
            else if(enemy[i].targetX < enemy[i].x)
            {
                enemy[i].x -= enemy[i].speed;
            }
            if(enemy[i].targetY > enemy[i].y)
            {
                enemy[i].y += enemy[i].speed;
            }
            else if(enemy[i].targetY < enemy[i].y)
            {
                enemy[i].y -= enemy[i].speed;
            }
        }
    }
}
void DropedSpeedAgarAndEnemy(BadEnemy enemy[], MyAgar &agar, int size)
{


    if(agar.ForSpeedAgar ==1 && agar.diameter>15 && agar.speed-2 > 0)
    {
          agar.speed -= 2;
          agar.ForSpeedAgar +=1;
    }
    else if(agar.ForSpeedAgar ==2 && agar.diameter>20 && agar.speed-2 > 0)
    {
        agar.speed -= 2;
        agar.ForSpeedAgar +=1;
    }
    else if(agar.ForSpeedAgar ==3 && agar.diameter>30 && agar.speed-1 > 0)
    {
        agar.speed -= 1;
    }
    for(int i=0; i<size; ++i)
    {

        if(enemy[i].ForSpeedEnemy==1 && enemy[i].Ediameter>15 && enemy[i].speed-2 > 0)
        {
            enemy[i].speed -=2;
            enemy[i].ForSpeedEnemy+=1;
        }
        else if(enemy[i].ForSpeedEnemy==2 && enemy[i].Ediameter>20 && enemy[i].speed-2 > 0)
        {
            enemy[i].speed -= 2;
            enemy[i].ForSpeedEnemy+=1;
        }
        else if(enemy[i].ForSpeedEnemy==3 && enemy[i].Ediameter>30 && enemy[i].speed-1 > 0)
        {
            enemy[i].speed -= 1;
            enemy[i].ForSpeedEnemy+=1;
        }
    }
}


