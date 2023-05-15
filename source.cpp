#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_image.h>
#include <iostream>
#include <math.h>
#include <vector>
#include <bits/stdc++.h>

// width and height and some other stats of the game
#define WIDTH1 620
#define HEIGHT1 720
#define WIDTH 800
#define HEIGHT 800
#define SPEED 9
#define FONT_SIZE 32
#define BALLSPEED 12
#define SIZE 16
#define COL 11
#define ROW 10
#define PI 3.14
#define SPACING 16

// width and height and some other stats of the multi player game
#define WIDTH3 1400
#define HEIGHT3 800
#define WIDTH_L 700
#define WIDTH_R 700
#define COL3 11
#define ROW3 7
#define SPACING3 12

// width and height and some other stats of the 2v2 mode
#define COL8 11
#define ROW8 7
#define SPACING8 12
#define SIZE8 9

// cnt to reset the score and aim
int cnt = 0;
int cnt2 = 0;

// width and height of the font
int titleW=0, titleH=0;

// FPS stats
int frameCount, timerFPS, lastFrame, fps;

// window for rush and break mode and some function
SDL_Window* window9;
SDL_Renderer* renderer9;

// texture of heart score and time
SDL_Texture* heartNum9 = NULL;
int heart9 = 2;
SDL_Texture* scoreNum9 = NULL;
int score9 = 0;
SDL_Texture* timerText9 = NULL;
int timer9 = 80;
SDL_Texture* highScoreNum = NULL;
int highScore = 0;
bool running9;

// position of the paddle ball and bricks
SDL_Rect paddle9, ball9, brick9;
bool bricks9[ROW*COL];
float vel9X, vel9Y;

// font and the color
TTF_Font* font13;
SDL_Color color13;

bool isLost;
SDL_Texture* restart = NULL;
int checkPoint10Y = 0;
int checkPoint9Y = 10000;
int cnt1 = 0;
int restartX = -3000;


// function to reset the brick
void resetBrick9()
{
    for(int i=0; i<ROW*COL; i++) bricks9[i]=1;
    paddle9.x = WIDTH/2-paddle9.w/2;
    ball9.y = paddle9.y-paddle9.h*4;
    vel9X=0;
    vel9Y = BALLSPEED/2;
    ball9.x = paddle9.x+paddle9.w/2;
}

// function to set up the brick
void setupBrick9(int i)
{
    brick9.x = (((i%COL)+1)*SPACING)+((i%COL)*brick9.w)-(SPACING/2);
    brick9.y = brick9.h*3+(((i%ROW)+1)*SPACING)+((i%ROW)*brick9.h)-(SPACING/2);
}

// function to update the game
void update9()
{
    if(SDL_HasIntersection(&ball9, &paddle9)) {
        double rel = (paddle9.x+(paddle9.w/2))-(ball9.x+(SIZE/2));
        double norm = rel/(paddle9.w/2);
        double bounceAngle = norm*(5*PI/12);
        vel9X = -BALLSPEED*sin(bounceAngle);
        vel9Y = -BALLSPEED*cos(bounceAngle);
    }
    if(ball9.y<0) vel9Y = -vel9Y;
    if(ball9.y+SIZE>HEIGHT) {
        // decrease the heart and reset the ball
        heart9--;
        paddle9.x = WIDTH/2-paddle9.w/2;
        ball9.y = paddle9.y-paddle9.h*4;
        vel9X=0;
        vel9Y = BALLSPEED/2;
        ball9.x = paddle9.x+paddle9.w/2;
    }
    if(ball9.x<0 || ball9.x+SIZE>WIDTH) vel9X = -vel9X;
    ball9.x+=vel9X;
    ball9.y+=vel9Y;
    if(paddle9.x<0) paddle9.x = 0;
    if(paddle9.x+paddle9.w>WIDTH) paddle9.x = WIDTH-paddle9.w;
    bool reset = 1;
    for(int i=0; i<ROW*COL; i++) {
        setupBrick9(i);
        if(SDL_HasIntersection(&ball9, &brick9) && bricks9[i]) {
            score9+=65;
            timer9++;
            bricks9[i] = 0;
            if(ball9.x>=brick9.x) {vel9X=-vel9X; ball9.x-=20;}
            if(ball9.x<=brick9.x) {vel9X=-vel9X; ball9.x+=20;}
            if(ball9.y<=brick9.y) {vel9Y=-vel9Y; ball9.y-=20;}
            if(ball9.y>=brick9.y) {vel9Y=-vel9Y; ball9.y+=20;}
        }
        if(bricks9[i]) reset = 0;
    }
    // restart announce render
    TTF_Font* font20 = TTF_OpenFont("BruceForeverRegular-X3jd2.ttf", 25);
    SDL_Color color20 = {255, 255, 255};

    highScore = std::max(highScore, score9);
    if(heart9==0 || timer9<=0) isLost = true;
    else isLost = false;
    if(isLost) {
       vel9X = 0;
       vel9Y = 0;
       SDL_Surface* restartSurface = TTF_RenderText_Solid(font20, "TRY AGAIN TO BREAK THE HIGH SCORE!", color20);
       restart = SDL_CreateTextureFromSurface(renderer9, restartSurface);
       SDL_FreeSurface(restartSurface);
       SDL_QueryTexture(restart, NULL, NULL, &titleW, &titleH);
       checkPoint10Y+=20;
       if(checkPoint10Y%2500==0) {
            score9 = 0;
            heart9 = 2;
            timer9 = 60;
            resetBrick9();
            restart = NULL;
       }
    }
    if(reset) resetBrick9();

}

// function to handle the input
void handleInput9()
{
    SDL_Event e9;
    const Uint8 *keystates = SDL_GetKeyboardState(NULL);
    while(SDL_PollEvent(&e9)) {
        if(e9.type==SDL_QUIT) running9 = false;
    }
    if(keystates[SDL_SCANCODE_ESCAPE]) running9 = false;
    if(keystates[SDL_SCANCODE_RIGHT]) paddle9.x+=SPEED;
    if(keystates[SDL_SCANCODE_LEFT]) paddle9.x-=SPEED;
}

// function to render on the screen
void render9()
{
    SDL_SetRenderDrawColor(renderer9, 0, 0, 0, 255);
    SDL_RenderClear(renderer9);// limiting the frame

    // limiting the frame
    frameCount++;
    timerFPS=SDL_GetTicks()-lastFrame;
    if(timerFPS<(1000/60)) {
     SDL_Delay((1000/60)-timerFPS);
    }

    restartX+=50;
    if(restartX>50) restartX = 50;
    SDL_Rect restartPos = {restartX, 600, titleW, titleH};



    // define the font and the color
    font13 = TTF_OpenFont("GameOfSquids-1GMVL.ttf", 20);
    color13 = {255, 255, 255};


    // render the heart on the screen
    std::string heartNum9Line = "LIFE: "+std::to_string(heart9);
    SDL_Surface* heartNum9Surface = TTF_RenderText_Solid(font13, heartNum9Line.c_str(), color13);
    heartNum9 = SDL_CreateTextureFromSurface(renderer9, heartNum9Surface);
    SDL_FreeSurface(heartNum9Surface);
    SDL_QueryTexture(heartNum9, NULL, NULL, &titleW, &titleH);
    SDL_Rect heartNum9Pos = {20, 20, titleW, titleH};
    SDL_RenderCopy(renderer9, heartNum9, NULL, &heartNum9Pos);
    SDL_DestroyTexture(heartNum9);

    // render the score on the screen
    std::string scoreNum9Line = "SCORE: "+std::to_string(score9);
    SDL_Surface* scoreNum9Surface = TTF_RenderText_Solid(font13, scoreNum9Line.c_str(), color13);
    scoreNum9 = SDL_CreateTextureFromSurface(renderer9, scoreNum9Surface);
    SDL_FreeSurface(scoreNum9Surface);
    SDL_QueryTexture(scoreNum9, NULL, NULL, &titleW, &titleH);
    SDL_Rect scoreNum9Pos = {200, 20, titleW, titleH};
    SDL_RenderCopy(renderer9, scoreNum9, NULL, &scoreNum9Pos);
    SDL_DestroyTexture(scoreNum9);

    // render the timer on the screen
    std::string timerText9Line = "TIME: "+std::to_string(timer9);
    SDL_Surface* timerText9Surface = TTF_RenderText_Solid(font13, timerText9Line.c_str(), color13);
    timerText9 = SDL_CreateTextureFromSurface(renderer9, timerText9Surface);
    SDL_FreeSurface(timerText9Surface);
    SDL_QueryTexture(timerText9, NULL, NULL, &titleW, &titleH);
    SDL_Rect timerText9Pos = {380, 20, titleW, titleH};
    SDL_RenderCopy(renderer9, timerText9, NULL, &timerText9Pos);
    SDL_DestroyTexture(timerText9);

    // render the high score on the screen
    std::string highScoreNumLine = "HIGHSCORE: "+std::to_string(highScore);
    SDL_Surface* highScoreNumSurface = TTF_RenderText_Solid(font13, highScoreNumLine.c_str(), color13);
    highScoreNum = SDL_CreateTextureFromSurface(renderer9, highScoreNumSurface);
    SDL_FreeSurface(highScoreNumSurface);
    SDL_QueryTexture(highScoreNum, NULL, NULL, &titleW, &titleH);
    SDL_Rect highScoreNumPos = {560, 20, titleW, titleH};
    SDL_RenderCopy(renderer9, highScoreNum, NULL, &highScoreNumPos);
    SDL_DestroyTexture(highScoreNum);

    // countdown
    checkPoint9Y-=20;
    if(checkPoint9Y%1000==0) timer9--;

    // draw the paddle
    SDL_SetRenderDrawColor(renderer9, 255, 255, 255, 255);
    SDL_RenderFillRect(renderer9, &paddle9);

    // draw the ball
    SDL_RenderFillRect(renderer9, &ball9);

    // draw the brick
    for(int i=0; i<ROW*COL; i++) {
        SDL_SetRenderDrawColor(renderer9, 255, 0, 0, 255);
        if(i%4==1) SDL_SetRenderDrawColor(renderer9, 0, 0, 255, 255);
        if(i%4==2) SDL_SetRenderDrawColor(renderer9, 0, 255, 0, 255);
        if(i%4==3) SDL_SetRenderDrawColor(renderer9, 255, 255, 0, 255);
        if(bricks9[i]) {
            setupBrick9(i);
            SDL_RenderFillRect(renderer9, &brick9);
        }
    }

    // render copy the restart announce
    SDL_RenderCopy(renderer9, restart, NULL, &restartPos);
    SDL_DestroyTexture(restart);

    SDL_RenderPresent(renderer9);
}




// window for the 2v2 mode and some function
SDL_Window* window8;
SDL_Renderer* renderer8;
bool running8;
SDL_Texture* playerTeamHeart = NULL;
int heart5 = 2;
SDL_Texture* AITeamHeart = NULL;
int heart6 = 2;
SDL_Texture* playerTeamScore = NULL;
int score5 = 0;
SDL_Texture* AITeamScore = NULL;
int score6 = 0;
SDL_Texture* timerText5 = NULL;
SDL_Texture* timerText6 = NULL;
int timer5 = 100;
int timer6 = 100;
int checkPoint8Y = 10000;
TTF_Font* font12;
SDL_Color color12;
SDL_Rect paddle5, paddle6, paddle7, paddle8;
SDL_Rect ball5, ball6, ball7, ball8;
SDL_Rect brick5, brick6;
bool bricks5[ROW8*COL8], bricks6[ROW8*COL8];
SDL_Rect barrier5 = {WIDTH_L-50, 0, 10, HEIGHT};
SDL_Rect barrier6 = {WIDTH_L+52, 0, 10, HEIGHT};
float vel5X, vel5Y, vel6X, vel6Y, vel7X, vel7Y, vel8X, vel8Y;

// window for the result of the 2v2 mode
SDL_Window* window10 = NULL;
SDL_Renderer* renderer10 = NULL;
SDL_Texture* title10 = NULL;
SDL_Texture* tryAgain10 = NULL;
SDL_Texture* quit10 = NULL;
bool isPressed10;

// function to reset the brick for the player team
void resetBrickForPlayerTeam()
{
   for(int i=0; i<ROW8*COL8; i++) bricks5[i]=1;
   // for the player 1
   paddle5.x = (WIDTH_L/2)-(paddle5.w/2)-200;
   ball5.y = paddle5.y-(paddle5.h*4);
   vel5Y = BALLSPEED/2;
   vel5X = 0;
   ball5.x = WIDTH_L/2-SIZE8/2-200;

   // for the player 2
   paddle6.x = (WIDTH_L/2)+(paddle5.w/2);
   ball6.y = paddle6.y-(paddle6.h*4);
   vel6Y = BALLSPEED/2;
   vel6X = 0;
   ball6.x = WIDTH_L/2+SIZE8/2+80;
}

// function to reset the brick for the AI team
void resetBrickForAITeam()
{
   for(int i=0; i<ROW8*COL8; i++) bricks6[i]=1;
   // for the AI 1
   paddle7.x = WIDTH3 - paddle5.x - 100;
   ball7.y = paddle7.y-(paddle7.h*4);
   vel7Y = -BALLSPEED;
   vel7X = 0;
   ball7.x = WIDTH3 - ball5.x - 16;

   // for the AI 2
   paddle8.x = WIDTH3-paddle6.x-100;
   ball8.y = paddle8.y-(paddle8.h*4);
   vel8Y=-BALLSPEED;
   vel8X=0;
   ball8.x=WIDTH3-ball6.x-16;
}

// function to set up bricks for player team
void setupBrickForPlayerTeam(int i)
{
    brick5.x = (((i%COL8)+1)*SPACING8)+((i%COL8)*brick5.w)-(SPACING8/2);
    brick5.y = brick5.h*3+(((i%ROW8)+1)*SPACING8)+((i%ROW8)*brick5.h)-(SPACING8/2);
}

// function to set up bricks for AI team
void setupBrickForAITeam(int i)
{
    brick6.x = WIDTH3-((((i%COL8)+1)*SPACING8)+((i%COL8)*brick6.w)-(SPACING8/2))-32;
    brick6.y = brick6.h*3+(((i%ROW8)+1)*SPACING8)+((i%ROW8)*brick6.h)-(SPACING8/2);
}

// function to update for player team
void updateForThePlayerTeam()
{
    // intersection of player 1
    if(SDL_HasIntersection(&ball5, &paddle5)) {
        double rel = (paddle5.x+(paddle5.w/2))-(ball5.x+(SIZE8/2));
        double norm = rel/(paddle5.w/2);
        double bounceAngle = norm*(5*PI/12);
        vel5X = -BALLSPEED*sin(bounceAngle);
        vel5Y = -BALLSPEED*cos(bounceAngle);
    }
    if(ball5.y<0) vel5Y = -vel5Y;
    if(ball5.y+SIZE8>HEIGHT) {
        vel5Y = -vel5Y;
    }
    if(ball5.x<0 || ball5.x+SIZE8>WIDTH_L-50) vel5X = -vel5X;
    ball5.x+=vel5X;
    ball5.y+=vel5Y;
    if(paddle5.x<0) paddle5.x = 0;
    if(paddle5.x+paddle5.w>WIDTH_L-50) paddle5.x = WIDTH_L-paddle5.w-50;

    // intersection of player 2
    if(SDL_HasIntersection(&ball6, &paddle6)) {
        double rel = (paddle6.x+(paddle6.w/2))-(ball6.x+(SIZE8/2));
        double norm = rel/(paddle6.w/2);
        double bounceAngle = norm*(5*PI/12);
        vel6X = -BALLSPEED*sin(bounceAngle);
        vel6Y = -BALLSPEED*cos(bounceAngle);
    }
    if(ball6.y<0) vel6Y = -vel6Y;
    if(ball6.y+SIZE8>HEIGHT) {
        vel6Y = -vel6Y;
    }
    if(ball6.x<0 || ball6.x+SIZE>WIDTH_L-50) vel6X = -vel6X;
    ball6.x+=vel6X;
    ball6.y+=vel6Y;
    if(paddle6.x<0) paddle6.x = 0;
    if(paddle6.x+paddle6.w>WIDTH_L-50) paddle6.x = WIDTH_L-paddle6.w-50;

    // intersection with bricks
    bool reset = 1;
    for(int i=0; i<ROW8*COL8; i++) {
        setupBrickForPlayerTeam(i);
        if(SDL_HasIntersection(&ball5, &brick5) && bricks5[i]) {
            score5+=25;
            bricks5[i] = 0;
            if(ball5.x>=brick5.x) {vel5X=-vel5X; ball5.x-=20;}
            if(ball5.x<=brick5.x) {vel5X=-vel5X; ball5.x+=20;}
            if(ball5.y<=brick5.y) {vel5Y=-vel5Y; ball5.y-=20;}
            if(ball5.y>=brick5.y) {vel5Y=-vel5Y; ball5.y+=20;}
        }
        if(SDL_HasIntersection(&ball6, &brick5) && bricks5[i]) {
            score5+=25;
            bricks5[i] = 0;
            if(ball6.x>=brick5.x) {vel6X=-vel6X; ball6.x-=20;}
            if(ball6.x<=brick5.x) {vel6X=-vel6X; ball6.x+=20;}
            if(ball6.y<=brick5.y) {vel6Y=-vel6Y; ball6.y-=20;}
            if(ball6.y>=brick5.y) {vel6Y=-vel6Y; ball6.y+=20;}
        }
        if(bricks5[i]) reset = 0;
    }
    if(reset) resetBrickForPlayerTeam();
}

// function to update for the AI team
void updateForTheAITeam()
{
    // intersection of AI 1
    if(SDL_HasIntersection(&ball7, &paddle7)) {
        double rel = (paddle7.x+(paddle7.w/2))-(ball7.x+(SIZE8/2));
        double norm = rel/(paddle7.w/2);
        double bounceAngle = norm*(5*PI/12);
        vel7X = -BALLSPEED*sin(bounceAngle);
        vel7Y = -BALLSPEED*cos(bounceAngle);
    }
    if(ball7.x>(paddle7.x+paddle7.w/2)+10) paddle7.x+=SPEED;
    if(ball7.x<=(paddle7.x+paddle7.w/2)-10) paddle7.x-=SPEED;
    if(ball7.y<0) vel7Y = -vel7Y;
    if(ball7.y+SIZE8>HEIGHT) {
        vel7Y = -vel7Y;
    }
    if(ball7.x+SIZE8>WIDTH3) vel7X = -vel7X;
    if(ball7.x<WIDTH_L+57) {
        ball7.x = WIDTH_R+57;
        vel7X = -vel7X;
    }
    ball7.x+=vel7X;
    ball7.y+=vel7Y;
    if(paddle7.x<WIDTH_L+52) paddle7.x = WIDTH_L+52;
    if(paddle7.x+paddle7.w>WIDTH3) paddle7.x = WIDTH3-paddle7.w;

    // intersection of AI 2
    if(SDL_HasIntersection(&ball8, &paddle8)) {
        double rel = (paddle8.x+(paddle8.w/2))-(ball8.x+(SIZE8/2));
        double norm = rel/(paddle8.w/2);
        double bounceAngle = norm*(5*PI/12);
        vel8X = -BALLSPEED*sin(bounceAngle);
        vel8Y = -BALLSPEED*cos(bounceAngle);
    }
    if(ball8.x>(paddle8.x+paddle8.w/2)) paddle8.x+=SPEED;
    if(ball8.x<=(paddle8.x+paddle8.w/2)) paddle8.x-=SPEED;
    if(ball8.y<0) vel8Y = -vel8Y;
    if(ball8.y+SIZE8>HEIGHT) {
        vel8Y = -vel8Y;
    }
    if(ball8.x+SIZE8>WIDTH3) vel8X = -vel8X;
    if(ball8.x<WIDTH_L+57) {
        ball8.x = WIDTH_R+57;
        vel8X = -vel8X;
    }
    ball8.x+=vel8X;
    ball8.y+=vel8Y;
    if(paddle8.x<WIDTH_L+52) paddle8.x = WIDTH_L+52;
    if(paddle8.x+paddle8.w>WIDTH3) paddle8.x = WIDTH3-paddle8.w;

    // intersection of bricks
    bool reset = 1;
    for(int i=0; i<ROW8*COL8; i++) {
        setupBrickForAITeam(i);
        if(SDL_HasIntersection(&ball7, &brick6) && bricks6[i]) {
            score6+=25;
            bricks6[i] = 0;
            if(ball7.x>=brick6.x) {vel7X=-vel7X; ball7.x-=20;}
            if(ball7.x<=brick6.x) {vel7X=-vel7X; ball7.x+=20;}
            if(ball7.y<=brick6.y) {vel7Y=-vel7Y; ball7.y-=20;}
            if(ball7.y>=brick6.y) {vel7Y=-vel7Y; ball7.y+=20;}
        }
        if(SDL_HasIntersection(&ball8, &brick6) && bricks6[i]) {
            score6+=25;
            bricks6[i] = 0;
            if(ball8.x>=brick6.x) {vel8X=-vel8X; ball8.x-=20;}
            if(ball8.x<=brick6.x) {vel8X=-vel8X; ball8.x+=20;}
            if(ball8.y<=brick6.y) {vel8Y=-vel8Y; ball8.y-=20;}
            if(ball8.y>=brick6.y) {vel8Y=-vel8Y; ball8.y+=20;}
        }
        if(bricks6[i]) reset = 0;
    }
    if(reset) resetBrickForAITeam();
}

// function to handle input of the player team
void handleInput8()
{
    SDL_Event e8;
    while(SDL_PollEvent(&e8)) {
        if(e8.type==SDL_QUIT) running8=false;
    }
    const Uint8 *keystates = SDL_GetKeyboardState(NULL);
    if(keystates[SDL_SCANCODE_RIGHT]) paddle5.x+=SPEED;
    if(keystates[SDL_SCANCODE_LEFT]) paddle5.x-=SPEED;
    if(keystates[SDL_SCANCODE_D]) paddle6.x+=SPEED;
    if(keystates[SDL_SCANCODE_A]) paddle6.x-=SPEED;
}

// function to render on the screen
void render8()
{
    SDL_SetRenderDrawColor(renderer8, 0, 0, 0, 255);
    SDL_RenderClear(renderer8);

    // limiting the frame
    frameCount++;
    timerFPS=SDL_GetTicks()-lastFrame;
    if(timerFPS<(1000/60)) {
     SDL_Delay((1000/60)-timerFPS);
    }

    // render the heart on the screen
    font12 = TTF_OpenFont("GameOfSquids-1GMVL.ttf", 20);
    color12 = {255, 255, 255};
    std::string playerTeamHeartLine = "LIFE: "+std::to_string(heart5);
    SDL_Surface* playerTeamHeartSurface = TTF_RenderText_Solid(font12, playerTeamHeartLine.c_str(), color12);
    playerTeamHeart = SDL_CreateTextureFromSurface(renderer8, playerTeamHeartSurface);
    SDL_FreeSurface(playerTeamHeartSurface);
    SDL_QueryTexture(playerTeamHeart, NULL, NULL, &titleW, &titleH);
    SDL_Rect playerTeamHeartPos = {20, 20, titleW, titleH};
    SDL_RenderCopy(renderer8, playerTeamHeart, NULL, &playerTeamHeartPos);
    SDL_DestroyTexture(playerTeamHeart);

    std::string AITeamHeartLine = "LIFE: "+std::to_string(heart6);
    SDL_Surface* AITeamHeartSurface = TTF_RenderText_Solid(font12, AITeamHeartLine.c_str(), color12);
    AITeamHeart = SDL_CreateTextureFromSurface(renderer8, AITeamHeartSurface);
    SDL_FreeSurface(AITeamHeartSurface);
    SDL_QueryTexture(AITeamHeart, NULL, NULL, &titleW, &titleH);
    SDL_Rect AITeamHeartPos = {800, 20, titleW, titleH};
    SDL_RenderCopy(renderer8, AITeamHeart, NULL, &AITeamHeartPos);
    SDL_DestroyTexture(AITeamHeart);

    // render the score on the screen
    std::string playerTeamScoreLine = "SCORE: "+std::to_string(score5);
    SDL_Surface* playerTeamScoreSurface = TTF_RenderText_Solid(font12, playerTeamScoreLine.c_str(), color12);
    playerTeamScore = SDL_CreateTextureFromSurface(renderer8, playerTeamScoreSurface);
    SDL_FreeSurface(playerTeamScoreSurface);
    SDL_QueryTexture(playerTeamScore, NULL, NULL, &titleW, &titleH);
    SDL_Rect playerTeamScorePos = {230, 20, titleW, titleH};
    SDL_RenderCopy(renderer8, playerTeamScore, NULL, &playerTeamScorePos);
    SDL_DestroyTexture(playerTeamScore);

    std::string AITeamScoreLine = "SCORE: "+std::to_string(score6);
    SDL_Surface* AITeamScoreSurface = TTF_RenderText_Solid(font12, AITeamScoreLine.c_str(), color12);
    AITeamScore = SDL_CreateTextureFromSurface(renderer8, AITeamScoreSurface);
    SDL_FreeSurface(AITeamScoreSurface);
    SDL_QueryTexture(AITeamScore, NULL, NULL, &titleW, &titleH);
    SDL_Rect AITeamScorePos = {1010, 20, titleW, titleH};
    SDL_RenderCopy(renderer8, AITeamScore, NULL, &AITeamScorePos);
    SDL_DestroyTexture(AITeamScore);

    // render the timer on the screen
    std::string timerText5Line = "TIME: "+std::to_string(timer5);
    SDL_Surface* timerText5Surface = TTF_RenderText_Solid(font12, timerText5Line.c_str(), color12);
    timerText5 = SDL_CreateTextureFromSurface(renderer8, timerText5Surface);
    SDL_FreeSurface(timerText5Surface);
    SDL_QueryTexture(timerText5, NULL, NULL, &titleW, &titleH);
    SDL_Rect timerText5Pos = {450, 20, titleW, titleH};
    SDL_RenderCopy(renderer8, timerText5, NULL, &timerText5Pos);
    SDL_DestroyTexture(timerText5);

    std::string timerText6Line = "TIME: "+std::to_string(timer6);
    SDL_Surface* timerText6Surface = TTF_RenderText_Solid(font12, timerText6Line.c_str(), color12);
    timerText6 = SDL_CreateTextureFromSurface(renderer8, timerText6Surface);
    SDL_FreeSurface(timerText6Surface);
    SDL_QueryTexture(timerText6, NULL, NULL, &titleW, &titleH);
    SDL_Rect timerText6Pos = {1230, 20, titleW, titleH};
    SDL_RenderCopy(renderer8, timerText6, NULL, &timerText6Pos);
    SDL_DestroyTexture(timerText6);

    // countdown
    checkPoint8Y-=25;
    if(checkPoint8Y%1000==0) {
        timer5--;
        timer6--;
    }

    // draw the paddle and the ball on the screen
    SDL_SetRenderDrawColor(renderer8, 255, 255, 255, 255);
    SDL_RenderFillRect(renderer8, &paddle5);
    SDL_RenderFillRect(renderer8, &paddle7);
    SDL_RenderFillRect(renderer8, &ball5);
    SDL_RenderFillRect(renderer8, &ball7);
    SDL_SetRenderDrawColor(renderer8, 255, 102, 102, 255);
    SDL_RenderFillRect(renderer8, &paddle6);
    SDL_RenderFillRect(renderer8, &paddle8);
    SDL_RenderFillRect(renderer8, &ball6);
    SDL_RenderFillRect(renderer8, &ball8);

    // draw the barrier
    SDL_SetRenderDrawColor(renderer8, 51, 255, 255, 255);
    SDL_RenderFillRect(renderer8, &barrier5);
    SDL_RenderFillRect(renderer8, &barrier6);

    // draw the bricks
    for(int i=0; i<ROW8*COL8; i++) {
        SDL_SetRenderDrawColor(renderer8, 255, 0, 0, 255);
        if(i%4==1) SDL_SetRenderDrawColor(renderer8, 255, 255, 0, 255);
        if(i%4==2) SDL_SetRenderDrawColor(renderer8, 0, 0, 255, 255);
        if(i%4==3) SDL_SetRenderDrawColor(renderer8, 0, 255, 0, 255);
        if(bricks5[i]) {
            setupBrickForPlayerTeam(i);
            SDL_RenderFillRect(renderer8, &brick5);
        }
        if(bricks6[i]) {
            setupBrickForAITeam(i);
            SDL_RenderFillRect(renderer8, &brick6);
        }
    }

    SDL_RenderPresent(renderer8);
}


// window for the challenge modes
SDL_Window* window5 = NULL;
SDL_Renderer* renderer5 = NULL;
SDL_Texture* title5 = NULL;
SDL_Texture* mode1 = NULL;
SDL_Texture* mode2 = NULL;
SDL_Texture* mode3 = NULL;
SDL_Texture* back5 = NULL;
bool isPressed5;

// window for the 1v1 AI and some function
SDL_Window* window6;
SDL_Renderer* renderer6;
SDL_Texture* heartPlayer3 = NULL;
int heart3 = 2;
SDL_Texture* heartPlayer4 = NULL;
int heart4 = 2;
SDL_Texture* scoreText3 = NULL;
int score3 = 0;
SDL_Texture* scoreText4 = NULL;
int score4 = 0;
SDL_Texture* timerText3 = NULL;
SDL_Texture* timerText4 = NULL;
int timer3 = 100;
int timer4 = 100;
SDL_Texture* checkPoint6 = NULL;
int checkPoint6Y = 10000;
bool running6;
TTF_Font* font7;
SDL_Color color7;
SDL_Rect paddle3, paddle4, ball3, ball4, brick3, brick4;
float vel3X, vel3Y, vel4X, vel4Y;
SDL_Rect barrier3 = {WIDTH_L-50, 0, 10, HEIGHT};
SDL_Rect barrier4 = {WIDTH_L+52, 0, 10, HEIGHT};
bool bricks3[ROW3*COL3], bricks4[ROW3*COL3];

// window for the result of the 1v1 mode
SDL_Window* window7=NULL;
SDL_Renderer* renderer7=NULL;
SDL_Texture* res7=NULL;
SDL_Texture* tryAgain7=NULL;
SDL_Texture* quit7=NULL;
bool isPressed7;

// function to reset the brick for the player and the AI
void resetBrickForThePlayer()
{
   for(int i=0; i<ROW3*COL3; i++) bricks3[i]=1;
   paddle3.x = (WIDTH_L/2)-(paddle3.w/2);
   ball3.y = paddle3.y-(paddle3.h*4);
   vel3Y = BALLSPEED/2;
   vel3X = 0;
   ball3.x = WIDTH_L/2-SIZE/2;
}

void resetBrickForTheAI()
{
   for(int i=0; i<ROW3*COL3; i++) bricks4[i]=1;
   paddle4.x = WIDTH3 - paddle3.x - 32;
   ball4.y = paddle4.y-(paddle4.h*4);
   vel4Y = -BALLSPEED;
   vel4X = 0;
   ball4.x = WIDTH3 - ball4.x - 16;
}

// function to set up bricks for the player and the AI
void setupBrickForThePlayer(int i)
{
    brick3.x = (((i%COL3)+1)*SPACING3)+((i%COL3)*brick3.w)-(SPACING3/2);
    brick3.y = brick3.h*3+(((i%ROW3)+1)*SPACING3)+((i%ROW3)*brick3.h)-(SPACING3/2);
}

void setupBrickForTheAI(int i)
{
    brick4.x = WIDTH3-((((i%COL3)+1)*SPACING3)+((i%COL3)*brick4.w)-(SPACING3/2))-32;
    brick4.y = brick4.h*3+(((i%ROW3)+1)*SPACING3)+((i%ROW3)*brick4.h)-(SPACING3/2);
}

// function to update the game for the player and the AI
void updateForThePlayer()
{
    if(SDL_HasIntersection(&ball3, &paddle3)) {
        double rel = (paddle3.x+(paddle3.w/2))-(ball3.x+(SIZE/2));
        double norm = rel/(paddle3.w/2);
        double bounceAngle = norm*(5*PI/12);
        vel3X = -BALLSPEED*sin(bounceAngle);
        vel3Y = -BALLSPEED*cos(bounceAngle);
    }
    if(ball3.y<0) vel3Y = -vel3Y;
    if(ball3.y+SIZE>HEIGHT) {
        // reset the ball and decrease the heart
        heart3--;
        paddle3.x = (WIDTH_L/2)-(paddle3.w/2);
        ball3.y = paddle3.y-(paddle3.h*4);
        vel3Y = BALLSPEED/2;
        vel3X = 0;
        ball3.x = WIDTH_L/2-SIZE/2;
    }
    if(ball3.x<0 || ball3.x+SIZE>WIDTH_L-50) vel3X = -vel3X;
    ball3.x+=vel3X;
    ball3.y+=vel3Y;
    if(paddle3.x<0) paddle3.x = 0;
    if(paddle3.x+paddle3.w>WIDTH_L-50) paddle3.x = WIDTH_L-paddle3.w-50;
    bool reset = 1;
    for(int i=0; i<ROW3*COL3; i++) {
        setupBrickForThePlayer(i);
        if(SDL_HasIntersection(&ball3, &brick3) && bricks3[i]) {
            score3+=30;
            bricks3[i] = 0;
            if(ball3.x>=brick3.x) {vel3X=-vel3X; ball3.x-=20;}
            if(ball3.x<=brick3.x) {vel3X=-vel3X; ball3.x+=20;}
            if(ball3.y<=brick3.y) {vel3Y=-vel3Y; ball3.y-=20;}
            if(ball3.y>=brick3.y) {vel3Y=-vel3Y; ball3.y+=20;}
        }
        if(bricks3[i]) reset = 0;
    }
    if(reset) resetBrickForThePlayer();
}

void updateForTheAI()
{
    if(SDL_HasIntersection(&ball4, &paddle4)) {
        double rel = (paddle4.x+(paddle4.w/2))-(ball4.x+(SIZE/2));
        double norm = rel/(paddle4.w/2);
        double bounceAngle = norm*(5*PI/12);
        vel4X = -BALLSPEED*sin(bounceAngle);
        vel4Y = -BALLSPEED*cos(bounceAngle);
    }
    if((ball4.x>914 && ball4.x<=1076) || (ball4.x>1238 && ball4.x<WIDTH3)) {
        if(ball4.x>(paddle4.x+paddle4.w/3)) paddle4.x+=SPEED;
        if(ball4.x<=(paddle4.x+paddle4.w/3)) paddle4.x-=SPEED;
    } else if((ball4.x>WIDTH_L+52 && ball4.x<=914) || (ball4.x>1076 && ball4.x<=1238)) {
        if(ball4.x>(paddle4.x+2*paddle4.w/3)) paddle4.x+=SPEED;
        if(ball4.x<=(paddle4.x+2*paddle4.w/3)) paddle4.x-=SPEED;
    }
    if(ball4.y<0) vel4Y = -vel4Y;
    if(ball4.y+SIZE>HEIGHT) {
        heart4--;
        vel4Y = -vel4Y;
    }
    if(ball4.x+SIZE>WIDTH3) vel4X = -vel4X;
    if(ball4.x<WIDTH_L+57) {
        ball4.x = WIDTH_R+57;
        vel4X = -vel4X;
    }
    ball4.x+=vel4X;
    ball4.y+=vel4Y;
    if(paddle4.x<WIDTH_L+52) paddle4.x = WIDTH_L+52;
    if(paddle4.x+paddle4.w>WIDTH3) paddle4.x = WIDTH3-paddle4.w;
    bool reset = 1;
    for(int i=0; i<ROW3*COL3; i++) {
        setupBrickForTheAI(i);
        if(SDL_HasIntersection(&ball4, &brick4) && bricks4[i]) {
            score4+=30;
            bricks4[i] = 0;
            if(ball4.x>=brick4.x) {vel4X=-vel4X; ball4.x-=20;}
            if(ball4.x<=brick4.x) {vel4X=-vel4X; ball4.x+=20;}
            if(ball4.y<=brick4.y) {vel4Y=-vel4Y; ball4.y-=20;}
            if(ball4.y>=brick4.y) {vel4Y=-vel4Y; ball4.y+=20;}
        }
        if(bricks4[i]) reset = 0;
    }
    if(reset) resetBrickForTheAI();
}

// function to handle the input for the player
void handleInput6()
{
    SDL_Event e6;
    const Uint8 *keystates = SDL_GetKeyboardState(NULL);
    while(SDL_PollEvent(&e6)) {
        if(e6.type==SDL_QUIT) running6 = false;
    }
    if(keystates[SDL_SCANCODE_RIGHT]) paddle3.x+=SPEED;
    if(keystates[SDL_SCANCODE_LEFT]) paddle3.x-=SPEED;
}

// function to render on the screen
void render6()
{
    SDL_SetRenderDrawColor(renderer6, 0, 0, 0, 255);
    SDL_RenderClear(renderer6);

    // limiting the frame
    frameCount++;
    timerFPS=SDL_GetTicks()-lastFrame;
    if(timerFPS<(1000/60)) {
     SDL_Delay((1000/60)-timerFPS);
    }
    // render the heart on the screen
    font7 = TTF_OpenFont("GameOfSquids-1GMVL.ttf", 20);
    SDL_Color color7 = {255, 255, 255};
    std::string heartPlayer3Line = "LIFE: "+std::to_string(heart3);
    SDL_Surface* heartPlayer3Surface = TTF_RenderText_Solid(font7, heartPlayer3Line.c_str(), color7);
    heartPlayer3 = SDL_CreateTextureFromSurface(renderer6, heartPlayer3Surface);
    SDL_FreeSurface(heartPlayer3Surface);
    SDL_QueryTexture(heartPlayer3, NULL, NULL, &titleW, &titleH);
    SDL_Rect heartPlayer3Pos = {20, 20, titleW, titleH};
    SDL_RenderCopy(renderer6, heartPlayer3, NULL, &heartPlayer3Pos);
    SDL_DestroyTexture(heartPlayer3);

    std::string heartPlayer4Line = "LIFE: "+std::to_string(heart4);
    SDL_Surface* heartPlayer4Surface = TTF_RenderText_Solid(font7, heartPlayer4Line.c_str(), color7);
    heartPlayer4 = SDL_CreateTextureFromSurface(renderer6, heartPlayer4Surface);
    SDL_FreeSurface(heartPlayer4Surface);
    SDL_QueryTexture(heartPlayer4, NULL, NULL, &titleW, &titleH);
    SDL_Rect heartPlayer4Pos = {800, 20, titleW, titleH};
    SDL_RenderCopy(renderer6, heartPlayer4, NULL, &heartPlayer4Pos);
    SDL_DestroyTexture(heartPlayer4);

    // render the score on the screen
    std::string scoreText3Line = "SCORE: "+std::to_string(score3);
    SDL_Surface* scoreText3Surface = TTF_RenderText_Solid(font7, scoreText3Line.c_str(), color7);
    scoreText3 = SDL_CreateTextureFromSurface(renderer6, scoreText3Surface);
    SDL_FreeSurface(scoreText3Surface);
    SDL_QueryTexture(scoreText3, NULL, NULL, &titleW, &titleH);
    SDL_Rect scoreText3Pos = {230, 20, titleW, titleH};
    SDL_RenderCopy(renderer6, scoreText3, NULL, &scoreText3Pos);
    SDL_DestroyTexture(scoreText3);

    std::string scoreText4Line = "SCORE: "+std::to_string(score4);
    SDL_Surface* scoreText4Surface = TTF_RenderText_Solid(font7, scoreText4Line.c_str(), color7);
    scoreText4 = SDL_CreateTextureFromSurface(renderer6, scoreText4Surface);
    SDL_FreeSurface(scoreText4Surface);
    SDL_QueryTexture(scoreText4, NULL, NULL, &titleW, &titleH);
    SDL_Rect scoreText4Pos = {1010, 20, titleW, titleH};
    SDL_RenderCopy(renderer6, scoreText4, NULL, &scoreText4Pos);
    SDL_DestroyTexture(scoreText4);

    // render the timer on the screen
    std::string timerText3Line = "TIME: "+std::to_string(timer3);
    SDL_Surface* timerText3Surface = TTF_RenderText_Solid(font7, timerText3Line.c_str(), color7);
    timerText3 = SDL_CreateTextureFromSurface(renderer6, timerText3Surface);
    SDL_FreeSurface(timerText3Surface);
    SDL_QueryTexture(timerText3, NULL, NULL, &titleW, &titleH);
    SDL_Rect timerText3Pos = {450, 20, titleW, titleH};
    SDL_RenderCopy(renderer6, timerText3, NULL, &timerText3Pos);
    SDL_DestroyTexture(timerText3);

    std::string timerText4Line = "TIME: "+std::to_string(timer4);
    SDL_Surface* timerText4Surface = TTF_RenderText_Solid(font7, timerText4Line.c_str(), color7);
    timerText4 = SDL_CreateTextureFromSurface(renderer6, timerText4Surface);
    SDL_FreeSurface(timerText4Surface);
    SDL_QueryTexture(timerText4, NULL, NULL, &titleW, &titleH);
    SDL_Rect timerText4Pos = {1230, 20, titleW, titleH};
    SDL_RenderCopy(renderer6, timerText4, NULL, &timerText4Pos);
    SDL_DestroyTexture(timerText4);

    // checkpoint to countdown
    checkPoint6Y-=20;
    if(checkPoint6Y%1000==0) {
        timer3--;
        timer4--;
    }

    // draw the paddle and ball
    SDL_SetRenderDrawColor(renderer6, 255, 255, 255, 255);
    SDL_RenderFillRect(renderer6, &paddle3);
    SDL_RenderFillRect(renderer6, &paddle4);
    SDL_RenderFillRect(renderer6, &ball3);
    SDL_RenderFillRect(renderer6, &ball4);

    // draw the barrier
    SDL_SetRenderDrawColor(renderer6, 51, 255, 255, 255);
    SDL_RenderFillRect(renderer6, &barrier3);
    SDL_RenderFillRect(renderer6, &barrier4);

    // draw the brick
    for(int i=0; i<ROW3*COL3; i++) {
        SDL_SetRenderDrawColor(renderer6, 255, 0, 0, 255);
        if(i%4==1) SDL_SetRenderDrawColor(renderer6, 0, 0, 255, 255);
        if(i%4==2) SDL_SetRenderDrawColor(renderer6, 255, 255, 0, 255);
        if(i%4==3) SDL_SetRenderDrawColor(renderer6, 0, 255, 0, 255);
        if(bricks3[i]) {
            setupBrickForThePlayer(i);
            SDL_RenderFillRect(renderer6, &brick3);
        }
        if(bricks4[i]) {
            setupBrickForTheAI(i);
            SDL_RenderFillRect(renderer6, &brick4);
        }
    }

    SDL_RenderPresent(renderer6);
}



// window for the multi and some function
SDL_Window* window3;
SDL_Renderer* renderer3;
SDL_Texture* heartPlayer1 = NULL;
int heart1 = 3;
SDL_Texture* heartPlayer2 = NULL;
int heart2 = 3;
SDL_Texture* scoreText1 = NULL;
int score1 = 0;
SDL_Texture* scoreText2 = NULL;
int score2 = 0;
SDL_Texture* timerText1 = NULL;
SDL_Texture* timerText2 = NULL;
int timer1 = 100;
int timer2 = 100;
SDL_Texture* checkPoint3 = NULL;
int checkPoint3Y = 10000;
bool running3;
TTF_Font* font10;
SDL_Color color10;
SDL_Rect paddle1, paddle2, ball1, ball2, brick1, brick2;
SDL_Rect barrier = {WIDTH_L-50, 0, 10, HEIGHT};
SDL_Rect barrier2 = {WIDTH_L+52, 0, 10, HEIGHT};
float vel1X, vel1Y, vel2X, vel2Y;
int heartNum2=3, heartNum1=3;
bool bricks1[ROW3*COL3], bricks2[ROW3*COL3];
TTF_Font* font3;

// function to reset the brick for players
void resetBrickForPlayer1()
{
   for(int i=0; i<ROW3*COL3; i++) bricks1[i]=1;
   paddle1.x = (WIDTH_L/2)-(paddle1.w/2);
   ball1.y = paddle1.y-(paddle1.h*4);
   vel1Y = BALLSPEED/2;
   vel1X = 0;
   ball1.x = WIDTH_L/2-SIZE/2;
}

void resetBrickForPlayer2()
{
   for(int i=0; i<ROW3*COL3; i++) bricks2[i]=1;
   paddle2.x = WIDTH3 - paddle1.x - 100;
   ball2.y = paddle2.y-(paddle2.h*4);
   vel2Y = BALLSPEED/2;
   vel2X = 0;
   ball2.x = WIDTH3 - ball1.x - 16;
}

// function to set up bricks for player
void setupBrickForPlayer1(int i)
{
    brick1.x = (((i%COL3)+1)*SPACING3)+((i%COL3)*brick1.w)-(SPACING3/2);
    brick1.y = brick1.h*3+(((i%ROW3)+1)*SPACING3)+((i%ROW3)*brick1.h)-(SPACING3/2);
}

void setupBrickForPlayer2(int i)
{
    brick2.x = WIDTH3-((((i%COL3)+1)*SPACING3)+((i%COL3)*brick1.w)-(SPACING3/2))-32;
    brick2.y = brick1.h*3+(((i%ROW3)+1)*SPACING3)+((i%ROW3)*brick1.h)-(SPACING3/2);
}

// function to update the game for players
void updateForPlayer1()
{
    if(SDL_HasIntersection(&ball1, &paddle1)) {
        double rel = (paddle1.x+(paddle1.w/2))-(ball1.x+(SIZE/2));
        double norm = rel/(paddle1.w/2);
        double bounceAngle = norm*(5*PI/12);
        vel1X = -BALLSPEED*sin(bounceAngle);
        vel1Y = -BALLSPEED*cos(bounceAngle);
    }
    if(ball1.y<0) vel1Y = -vel1Y;
    if(ball1.y+SIZE>HEIGHT) {
        // reset the ball and decrease the heart
        heart1--;
        paddle1.x = (WIDTH_L/2)-(paddle1.w/2);
        ball1.y = paddle1.y-(paddle1.h*4);
        vel1Y = BALLSPEED/2;
        vel1X = 0;
        ball1.x = WIDTH_L/2-SIZE/2;
    }
    if(ball1.x<0 || ball1.x+SIZE>WIDTH_L-50) vel1X = -vel1X;
    ball1.x+=vel1X;
    ball1.y+=vel1Y;
    if(paddle1.x<0) paddle1.x = 0;
    if(paddle1.x+paddle1.w>WIDTH_L-50) paddle1.x = WIDTH_L-paddle1.w-50;
    bool reset = 1;
    for(int i=0; i<ROW3*COL3; i++) {
        setupBrickForPlayer1(i);
        if(SDL_HasIntersection(&ball1, &brick1) && bricks1[i]) {
            score1+=20;
            bricks1[i] = 0;
            if(ball1.x>=brick1.x) {vel1X=-vel1X; ball1.x-=20;}
            if(ball1.x<=brick1.x) {vel1X=-vel1X; ball1.x+=20;}
            if(ball1.y<=brick1.y) {vel1Y=-vel1Y; ball1.y-=20;}
            if(ball1.y>=brick1.y) {vel1Y=-vel1Y; ball1.y+=20;}
        }
        if(bricks1[i]) reset = 0;
    }
    if(reset) resetBrickForPlayer1();
}

void updateForPlayer2()
{
    if(SDL_HasIntersection(&ball2, &paddle2)) {
        double rel = (paddle2.x+(paddle2.w/2))-(ball2.x+(SIZE/2));
        double norm = rel/(paddle2.w/2);
        double bounceAngle = norm*(5*PI/12);
        vel2X = -BALLSPEED*sin(bounceAngle);
        vel2Y = -BALLSPEED*cos(bounceAngle);
    }
    if(ball2.y<0) vel2Y = -vel2Y;
    if(ball2.y+SIZE>HEIGHT) {
        // reset the ball and decrease the heart
        heart2--;
        paddle2.x = WIDTH3 - paddle1.x - 100;
        ball2.y = paddle2.y-(paddle2.h*4);
        vel2Y = BALLSPEED/2;
        vel2X = 0;
        ball2.x = WIDTH3 - ball1.x - 16;

    }
    if(ball2.x+SIZE>WIDTH3) vel2X = -vel2X;
    if(ball2.x<WIDTH_L+57) {
        ball2.x = WIDTH_R+57;
        vel2X = -vel2X;
    }
    ball2.x+=vel2X;
    ball2.y+=vel2Y;
    if(paddle2.x<WIDTH_L+52) paddle2.x = WIDTH_L+52;
    if(paddle2.x+paddle2.w>WIDTH3) paddle2.x = WIDTH3-paddle2.w;
    bool reset = 1;
    for(int i=0; i<ROW3*COL3; i++) {
        setupBrickForPlayer2(i);
        if(SDL_HasIntersection(&ball2, &brick2) && bricks2[i]) {
            score2+=20;
            bricks2[i] = 0;
            if(ball2.x>=brick2.x) {vel2X=-vel2X; ball2.x-=20;}
            if(ball2.x<=brick2.x) {vel2X=-vel2X; ball2.x+=20;}
            if(ball2.y<=brick2.y) {vel2Y=-vel2Y; ball2.y-=20;}
            if(ball2.y>=brick2.y) {vel2Y=-vel2Y; ball2.y+=20;}
        }
        if(bricks2[i]) reset = 0;
    }
    if(reset) resetBrickForPlayer2();
}

// function to handle the input
void handleInput3()
{
    SDL_Event e3;
    const Uint8 *keystates = SDL_GetKeyboardState(NULL);
    while(SDL_PollEvent(&e3)) {
        if(e3.type==SDL_QUIT) running3 = false;
    }
    if(keystates[SDL_SCANCODE_RIGHT]) paddle2.x+=SPEED;
    if(keystates[SDL_SCANCODE_LEFT]) paddle2.x-=SPEED;
    if(keystates[SDL_SCANCODE_D]) paddle1.x+=SPEED;
    if(keystates[SDL_SCANCODE_A]) paddle1.x-=SPEED;
}

// function to render
void render3()
{
    SDL_SetRenderDrawColor(renderer3, 0, 0, 0, 255);
    SDL_RenderClear(renderer3);

    // limiting the frame
    frameCount++;
    timerFPS=SDL_GetTicks()-lastFrame;
    if(timerFPS<(1000/60)) {
     SDL_Delay((1000/60)-timerFPS);
    }

    // render the heart on the screen
    font3 = TTF_OpenFont("GameOfSquids-1GMVL.ttf", 20);
    SDL_Color color3 = {255, 255, 255};
    std::string heartPlayer1Line = "LIFE: "+std::to_string(heart1);
    SDL_Surface* heartPlayer1Surface = TTF_RenderText_Solid(font3, heartPlayer1Line.c_str(), color3);
    heartPlayer1 = SDL_CreateTextureFromSurface(renderer3, heartPlayer1Surface);
    SDL_FreeSurface(heartPlayer1Surface);
    SDL_QueryTexture(heartPlayer1, NULL, NULL, &titleW, &titleH);
    SDL_Rect heartPlayer1Pos = {20, 20, titleW, titleH};
    SDL_RenderCopy(renderer3, heartPlayer1, NULL, &heartPlayer1Pos);
    SDL_DestroyTexture(heartPlayer1);

    std::string heartPlayer2Line = "LIFE: "+std::to_string(heart2);
    SDL_Surface* heartPlayer2Surface = TTF_RenderText_Solid(font3, heartPlayer2Line.c_str(), color3);
    heartPlayer2 = SDL_CreateTextureFromSurface(renderer3, heartPlayer2Surface);
    SDL_FreeSurface(heartPlayer2Surface);
    SDL_QueryTexture(heartPlayer2, NULL, NULL, &titleW, &titleH);
    SDL_Rect heartPlayer2Pos = {800, 20, titleW, titleH};
    SDL_RenderCopy(renderer3, heartPlayer2, NULL, &heartPlayer2Pos);
    SDL_DestroyTexture(heartPlayer2);

    // render the score on the screen
    std::string scoreText1Line = "SCORE: "+std::to_string(score1);
    SDL_Surface* scoreText1Surface = TTF_RenderText_Solid(font3, scoreText1Line.c_str(), color3);
    scoreText1 = SDL_CreateTextureFromSurface(renderer3, scoreText1Surface);
    SDL_FreeSurface(scoreText1Surface);
    SDL_QueryTexture(scoreText1, NULL, NULL, &titleW, &titleH);
    SDL_Rect scoreText1Pos = {230, 20, titleW, titleH};
    SDL_RenderCopy(renderer3, scoreText1, NULL, &scoreText1Pos);
    SDL_DestroyTexture(scoreText1);

    std::string scoreText2Line = "SCORE: "+std::to_string(score2);
    SDL_Surface* scoreText2Surface = TTF_RenderText_Solid(font3, scoreText2Line.c_str(), color3);
    scoreText2 = SDL_CreateTextureFromSurface(renderer3, scoreText2Surface);
    SDL_FreeSurface(scoreText2Surface);
    SDL_QueryTexture(scoreText2, NULL, NULL, &titleW, &titleH);
    SDL_Rect scoreText2Pos = {1010, 20, titleW, titleH};
    SDL_RenderCopy(renderer3, scoreText2, NULL, &scoreText2Pos);
    SDL_DestroyTexture(scoreText2);

    // render the timer on the screen
    std::string timerText1Line = "TIME: "+std::to_string(timer1);
    SDL_Surface* timerText1Surface = TTF_RenderText_Solid(font3, timerText1Line.c_str(), color3);
    timerText1 = SDL_CreateTextureFromSurface(renderer3, timerText1Surface);
    SDL_FreeSurface(timerText1Surface);
    SDL_QueryTexture(timerText1, NULL, NULL, &titleW, &titleH);
    SDL_Rect timerText1Pos = {450, 20, titleW, titleH};
    SDL_RenderCopy(renderer3, timerText1, NULL, &timerText1Pos);
    SDL_DestroyTexture(timerText1);

    std::string timerText2Line = "TIME: "+std::to_string(timer2);
    SDL_Surface* timerText2Surface = TTF_RenderText_Solid(font3, timerText2Line.c_str(), color3);
    timerText2 = SDL_CreateTextureFromSurface(renderer3, timerText2Surface);
    SDL_FreeSurface(timerText2Surface);
    SDL_QueryTexture(timerText2, NULL, NULL, &titleW, &titleH);
    SDL_Rect timerText2Pos = {1230, 20, titleW, titleH};
    SDL_RenderCopy(renderer3, timerText2, NULL, &timerText2Pos);
    SDL_DestroyTexture(timerText2);

    // checkpoint to countdown
    SDL_Rect checkPoint3Pos = {0, checkPoint3Y, 32, 32};
    checkPoint3Y-=20;
    if(checkPoint3Y%1000==0) {
        timer1--;
        timer2--;
    }


    // draw the paddle and ball
    SDL_SetRenderDrawColor(renderer3, 255, 255, 255, 255);
    SDL_RenderFillRect(renderer3, &paddle1);
    SDL_RenderFillRect(renderer3, &paddle2);
    SDL_RenderFillRect(renderer3, &ball1);
    SDL_RenderFillRect(renderer3, &ball2);

    // draw the barrier
    SDL_SetRenderDrawColor(renderer3, 51, 255, 255, 255);
    SDL_RenderFillRect(renderer3, &barrier);
    SDL_RenderFillRect(renderer3, &barrier2);

    // draw the brick
    for(int i=0; i<ROW3*COL3; i++) {
        SDL_SetRenderDrawColor(renderer3, 255, 0, 0, 255);
        if(i%4==1) SDL_SetRenderDrawColor(renderer3, 0, 0, 255, 255);
        if(i%4==2) SDL_SetRenderDrawColor(renderer3, 255, 255, 0, 255);
        if(i%4==3) SDL_SetRenderDrawColor(renderer3, 0, 255, 0, 255);
        if(bricks1[i]) {
            setupBrickForPlayer1(i);
            SDL_RenderFillRect(renderer3, &brick1);
        }
        if(bricks2[i]) {
            setupBrickForPlayer2(i);
            SDL_RenderFillRect(renderer3, &brick2);
        }
    }

    SDL_RenderPresent(renderer3);
}


// icon for the window
SDL_Surface* icon = IMG_Load("brick-breaker.png");

// window and some function of the game
SDL_Window* window;
SDL_Renderer* renderer;
SDL_Texture* background;
SDL_Texture* checkpoint = NULL;
SDL_Texture* timerText = NULL;
SDL_Texture* aim = NULL;
int aimNum = 1500;
int timer = 100;
bool running;
TTF_Font* font;
SDL_Color color;
SDL_Rect paddle, ball, heart, brick;
float velX, velY;
int heartNum=3;
bool bricks[ROW*COL];
SDL_Texture* scoreText = NULL;
SDL_Texture* pointText = NULL;
int point = 0;
int checkpointX = 0;
int checkpointY = -10000;


// window for the result
SDL_Window* window2 = NULL;
SDL_Renderer* renderer2 = NULL;
SDL_Texture* res2 = NULL;
SDL_Texture* res3 = NULL;
SDL_Texture* tryAgain = NULL;
SDL_Texture* quit2 = NULL;
bool isPressed2;
bool isPressed1;
bool isPressed4;
int xm, ym;

// window for the multi player result
SDL_Window* window4 = NULL;
SDL_Renderer* renderer4 = NULL;
SDL_Texture* res4 = NULL;
SDL_Texture* tryAgain4 = NULL;
SDL_Texture* quit4 = NULL;

// function to check the focus
bool checkFocus(int x, int y, SDL_Rect pos) {
    if(x>=pos.x && x<=pos.x+pos.w && y>=pos.y && y<=pos.y+pos.h) return 1;
    return 0;
}

// function the reset the brick of the game
void resetBrick()
{
    for(int i=0; i<ROW*COL; i++) bricks[i]=1;
    paddle.x = WIDTH/2 - (paddle.w/2);
    ball.y = paddle.y-(paddle.h*4);
    velY = BALLSPEED/2;
    velX = 0;
    ball.x = WIDTH/2-SIZE/2;
}

// function to set up the bricks
void setupBrick(int i)
{
    brick.x = (((i%COL)+1)*SPACING)+((i%COL)*brick.w)-(SPACING/2);
    brick.y = brick.h*3+(((i%ROW)+1)*SPACING)+((i%ROW)*brick.h)-(SPACING/2);
}

// function to write the score

// function to update the game
void update()
{
    if(SDL_HasIntersection(&ball, &paddle)) {
        double rel = (paddle.x+(paddle.w/2))-(ball.x+(SIZE/2));
        double norm = rel/(paddle.w/2);
        double bounceAngle = norm*(5*PI/12);
        velX = -BALLSPEED*sin(bounceAngle);
        velY = -BALLSPEED*cos(bounceAngle);
    }
    if(ball.y<0) velY = -velY;
    if(ball.y+SIZE>HEIGHT) {
        heartNum--;
        paddle.x = WIDTH/2 - (paddle.w/2);
        ball.y = paddle.y-(paddle.h*4);
        velY = BALLSPEED/2;
        velX = 0;
        ball.x = WIDTH/2-SIZE/2;
    }
    if(ball.x<0 || ball.x+SIZE>WIDTH) velX = -velX;
    ball.x+=velX;
    ball.y+=velY;
    if(paddle.x<0) paddle.x = 0;
    if(paddle.x+paddle.w>WIDTH) paddle.x = WIDTH-paddle.w;
    bool reset = 1;
    for(int i=0; i<ROW*COL; i++) {
        setupBrick(i);
        if(SDL_HasIntersection(&ball, &brick) && bricks[i]) {
            point+=20;
            bricks[i] = 0;
            if(ball.x>=brick.x) {velX=-velX; ball.x-=20;}
            if(ball.x<=brick.x) {velX=-velX; ball.x+=20;}
            if(ball.y<=brick.y) {velY=-velY; ball.y-=20;}
            if(ball.y>=brick.y) {velY=-velY; ball.y+=20;}
        }
        if(bricks[i]) reset = 0;
    }
    if(reset) resetBrick();
}

// function to handle the input of the game
void handleInput()
{
    SDL_Event e;
    const Uint8 *keystates = SDL_GetKeyboardState(NULL);
    while(SDL_PollEvent(&e)) {
        if(e.type==SDL_QUIT) running=false;
    }
    if(keystates[SDL_SCANCODE_ESCAPE]) running=false;
    if(keystates[SDL_SCANCODE_RIGHT]) paddle.x+=SPEED;
    if(keystates[SDL_SCANCODE_LEFT]) paddle.x-=SPEED;
}

// function to render all stats on the screen of the game
void render()
{
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    // limiting the frame
    frameCount++;
    timerFPS=SDL_GetTicks()-lastFrame;
    if(timerFPS<(1000/60)) {
     SDL_Delay((1000/60)-timerFPS);
    }

    font = TTF_OpenFont("GameOfSquids-1GMVL.ttf", 20);
    std::string _heartNum = "LIFE: "+std::to_string(heartNum);
    SDL_Surface* score = TTF_RenderText_Solid(font, _heartNum.c_str(), color);
    scoreText = SDL_CreateTextureFromSurface(renderer, score);
    SDL_FreeSurface(score);
    SDL_QueryTexture(scoreText, NULL, NULL, &titleW, &titleH);
    SDL_Rect scorePos = {20, 20, titleW, titleH};
    SDL_RenderCopy(renderer, scoreText, NULL, &scorePos);
    SDL_DestroyTexture(scoreText);

    std::string pointLine = "SCORE: "+std::to_string(point);
    SDL_Surface* pointTextSurface = TTF_RenderText_Solid(font, pointLine.c_str(), color);
    pointText = SDL_CreateTextureFromSurface(renderer, pointTextSurface);
    SDL_FreeSurface(pointTextSurface);
    SDL_QueryTexture(pointText, NULL, NULL, &titleW, &titleH);
    SDL_Rect pointPos = {180, 20, titleW, titleH};
    SDL_RenderCopy(renderer, pointText, NULL, &pointPos);
    SDL_DestroyTexture(pointText);

    std::string timerLine = "TIME: "+std::to_string(timer);
    SDL_Surface* timerTextSurface = TTF_RenderText_Solid(font, timerLine.c_str(), color);
    timerText = SDL_CreateTextureFromSurface(renderer, timerTextSurface);
    SDL_FreeSurface(timerTextSurface);
    SDL_QueryTexture(timerText, NULL, NULL, &titleW, &titleH);
    SDL_Rect timerTextPos = {400, 20, titleW, titleH};
    SDL_RenderCopy(renderer, timerText, NULL, &timerTextPos);
    SDL_DestroyTexture(timerText);

    SDL_Rect checkpointPos = {checkpointX, checkpointY, 32, 32};
    checkpointY+=20;
    if(checkpointY%1000==0) timer--;

    std::string aimLine = "AIM: "+std::to_string(aimNum);
    SDL_Surface* aimSurface = TTF_RenderText_Solid(font, aimLine.c_str(), color);
    aim = SDL_CreateTextureFromSurface(renderer, aimSurface);
    SDL_FreeSurface(aimSurface);
    SDL_QueryTexture(aim, NULL, NULL, &titleW, &titleH);
    SDL_Rect aimPos = {650, 20, titleW, titleH};
    SDL_RenderCopy(renderer, aim, NULL, &aimPos);
    SDL_DestroyTexture(aim);

    // draw the paddle and ball
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderFillRect(renderer, &paddle);
    SDL_RenderFillRect(renderer, &ball);
    for(int i=0; i<ROW*COL; i++) {
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        if(i%4==1) SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
        if(i%4==2) SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
        if(i%4==3) SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
        if(bricks[i]) {
            setupBrick(i);
            SDL_RenderFillRect(renderer, &brick);
        }
    }
    SDL_RenderPresent(renderer);
}

// function to close the window
void close()
{
    SDL_DestroyWindow(window);
    window = NULL;
    SDL_DestroyRenderer(renderer);
    renderer = NULL;
}

void close2()
{
    SDL_DestroyWindow(window2);
    window2 = NULL;
    SDL_DestroyRenderer(renderer2);
    renderer2 = NULL;
    isPressed1 = false;
    isPressed2 = false;
    timer = 100;
    heartNum = 3;
    point = 0;
}

void close3()
{
    SDL_DestroyWindow(window3);
    window3 = NULL;
    SDL_DestroyRenderer(renderer3);
    renderer3 = NULL;
}

void close4()
{
    SDL_DestroyWindow(window4);
    window4 = NULL;
    SDL_DestroyRenderer(renderer4);
    renderer4 = NULL;
    isPressed1 = false;
    isPressed2 = false;
    isPressed4 = false;
    heart1 = 3;
    heart2 = 3;
    timer1 = 200;
    timer2 = 200;
    score1 = 0;
    score2 = 0;
}

void close5()
{
    SDL_DestroyWindow(window5);
    window5 = NULL;
    SDL_DestroyRenderer(renderer5);
    renderer5 = NULL;
}

void close6()
{
    SDL_DestroyWindow(window6);
    window6 = NULL;
    SDL_DestroyRenderer(renderer6);
    renderer6 = NULL;
    isPressed1 = false;
    isPressed5 = false;
    timer3=320;
    timer4=320;
    score3=0;
    score4=0;
    heart3=2;
    heart4=2;
}

void close7()
{
    SDL_DestroyWindow(window7);
    window7 = NULL;
    SDL_DestroyRenderer(renderer7);
    renderer7 = NULL;
}

void close8()
{
    SDL_DestroyWindow(window8);
    window8 = NULL;
    SDL_DestroyRenderer(renderer8);
    renderer8 = NULL;
    timer5=320;
    timer6=320;
    heart5=2;
    heart6=2;
    score5=0;
    score6=0;
}

void close10()
{
    SDL_DestroyWindow(window10);
    window10 = NULL;
    SDL_DestroyRenderer(renderer10);
    renderer10 = NULL;
}


// window for the menu
SDL_Window* window1 = NULL;
SDL_Renderer* renderer1 = NULL;
SDL_Texture* title1 = NULL;
SDL_Texture* multi1 = NULL;
SDL_Texture* challenge1 = NULL;
SDL_Texture* enter1 = NULL;
SDL_Texture* exit1 = NULL;

SDL_Texture* loadTexture(std::string path)
{
    SDL_Texture* newTexture = NULL;
    SDL_Surface* loadedSurface = IMG_Load(path.c_str());
    if(loadedSurface==NULL) {
        std::cout << "Failed to load the surface! Error: " << SDL_GetError() << std::endl;
    }
    newTexture = SDL_CreateTextureFromSurface(renderer, loadedSurface);
    SDL_FreeSurface(loadedSurface);
    return newTexture;
}


// function to close the window
void close1()
{
    SDL_DestroyWindow(window1);
    window1 = NULL;
    SDL_DestroyRenderer(renderer1);
    renderer1 = NULL;
}

int main(int arc, char* argv[])
{
    // INIT
    if(SDL_Init(SDL_INIT_EVERYTHING)>0) {
        std::cout << "Failed to init! Error: " << SDL_GetError() << std::endl;
    }
    TTF_Init();

    // create the window
    flag:
    window1 = SDL_CreateWindow("BrickBreaker", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH1, HEIGHT1, SDL_WINDOW_SHOWN);
    if(window1==NULL) {
        std::cout << "Failed to create the window of the menu! Error: " << SDL_GetError() << std::endl;
    }

    // create the renderer
    renderer1 = SDL_CreateRenderer(window1, -1, 0);
    if(renderer1==NULL) {
        std::cout << "Failed to create the renderer of the menu! Error: " << SDL_GetError() << std::endl;
    }

    // set the icon for the window
    SDL_SetWindowIcon(window1, icon);

    // game loop
    bool running1 = true;
    SDL_Event e1;
    while(running1) {
        while(SDL_PollEvent(&e1)) {
            if(e1.type==SDL_QUIT) {
                running1 = false;
            } else if(e1.type==SDL_MOUSEMOTION) {
                xm = e1.motion.x;
                ym = e1.motion.y;
            } else if(e1.type==SDL_MOUSEBUTTONDOWN) {
                isPressed1 = true;
            } else if(e1.type==SDL_MOUSEBUTTONUP) {
                isPressed1 = false;
            }
        }
        SDL_SetRenderDrawColor(renderer1, 0, 0, 0, 255);
        SDL_RenderClear(renderer1);

        // render the title on the screen
        TTF_Font* font1 = TTF_OpenFont("ZilapGamePersonalUse-EaPDg.ttf", 70);
        SDL_Color color1 = {51, 255, 255};
        SDL_Surface* title1Surface = TTF_RenderText_Solid(font1, "BRICK BREAKER", color1);
        title1 = SDL_CreateTextureFromSurface(renderer1, title1Surface);
        SDL_FreeSurface(title1Surface);
        SDL_QueryTexture(title1, NULL, NULL, &titleW, &titleH);
        SDL_Rect title1Pos = {100, 50, titleW, titleH};

        // render the enter on the screen
        TTF_Font* font2 = TTF_OpenFont("ZilapGamePersonalUse-EaPDg.ttf", 45);
        TTF_Font* font4 = TTF_OpenFont("ZilapGamePersonalUse-EaPDg.ttf", 60);
        SDL_Color color2 = {255, 255, 0};
        SDL_Color color3 = {255, 0, 0};
        SDL_Surface* enter1Surface = TTF_RenderText_Solid(font2, "SINGLEPLAYER", color2);
        enter1 = SDL_CreateTextureFromSurface(renderer1, enter1Surface);
        SDL_FreeSurface(enter1Surface);
        SDL_QueryTexture(enter1, NULL, NULL, &titleW, &titleH);
        SDL_Rect enter1Pos = {185, 200, titleW, titleH};

        SDL_Surface* challenge1Surface = TTF_RenderText_Solid(font2, "CHALLENGE", color2);
        challenge1 = SDL_CreateTextureFromSurface(renderer1, challenge1Surface);
        SDL_FreeSurface(challenge1Surface);
        SDL_QueryTexture(challenge1, NULL, NULL, &titleW, &titleH);
        SDL_Rect challenge1Pos = {210, 350, titleW, titleH};

        SDL_Surface* multi1Surface = TTF_RenderText_Solid(font2, "MULTIPLAYER", color2);
        multi1 = SDL_CreateTextureFromSurface(renderer1, multi1Surface);
        SDL_FreeSurface(multi1Surface);
        SDL_QueryTexture(multi1, NULL, NULL, &titleW, &titleH);
        SDL_Rect multi1Pos = {195, 500, titleW, titleH};

        SDL_Surface* exit1Surface = TTF_RenderText_Solid(font2, "EXIT", color2);
        exit1 = SDL_CreateTextureFromSurface(renderer1, exit1Surface);
        SDL_FreeSurface(exit1Surface);
        SDL_QueryTexture(exit1, NULL, NULL, &titleW, &titleH);
        SDL_Rect exit1Pos = {275, 650, titleW, titleH};

        // check focus
        if(checkFocus(xm, ym, enter1Pos)) {
            SDL_Surface* tmp = TTF_RenderText_Solid(font4, "SINGLEPLAYER", color3);
            enter1 = SDL_CreateTextureFromSurface(renderer1, tmp);
            SDL_FreeSurface(tmp);
            if(isPressed1) {
                close1();
                // START OF THE GAME
                // create the window
                window = SDL_CreateWindow("BrickBreaker", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, SDL_WINDOW_SHOWN);
                if(window==NULL) {
                    std::cout << "Failed to create the window of the game! Error: " << SDL_GetError() << std::endl;
                }

                // set the window icon
                SDL_SetWindowIcon(window, icon);

                // create the renderer
                renderer = SDL_CreateRenderer(window, -1, 0);
                if(renderer==NULL) {
                    std::cout << "Failed to create the renderer of the game! Error: " << SDL_GetError() << std::endl;
                }

                // define the font
                font = TTF_OpenFont("G7TwinbeeForArcadeTtf-VqYl.ttf", FONT_SIZE);
                if(font==NULL) {
                    std::cout << "Failed to load the font of the game! Error: " << SDL_GetError() << std::endl;
                }
                running = 1;
                static int lastTime = 0;
                color = {255, 255, 255};
                // some stats of the paddle ball and brick
                paddle.h=12;paddle.w=WIDTH/4;
                paddle.y= HEIGHT-paddle.h-32;
                ball.w=ball.h=SIZE;
                brick.w=(WIDTH-(SPACING*COL))/COL;
                brick.h=22;

                // reset the brick
                resetBrick();

                // game loop
                while(running) {
                    lastFrame=SDL_GetTicks();
                    if(lastFrame>=lastTime+1000) {
                      lastTime=lastFrame;
                      fps=frameCount;
                      frameCount=0;
                    }
                    update();
                    handleInput();
                    render();
                    if(timer==0 && point>=aimNum) {
                        cnt++;
                        point=0;
                        timer = 200+cnt*50;
                        aimNum+=500;
                    } else if((timer==0 && point<aimNum) || heartNum==0) {
                        close();
                        // START OF THE RESULT WINDOW
                        // create the window
                        window2 = SDL_CreateWindow("BrickBreaker", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, SDL_WINDOW_SHOWN);
                        if(window2==NULL) {
                            std::cout << "Failed to create the window of the result! Error: " << SDL_GetError() << std::endl;
                        }

                        // create the renderer
                        renderer2 = SDL_CreateRenderer(window2, -1, 0);
                        if(renderer2==NULL) {
                            std::cout << "Failed to create the renderer of the result! Error: " << SDL_GetError() << std::endl;
                        }

                        // set the icon for the window
                        SDL_SetWindowIcon(window2, icon);

                        // game loop
                        bool running2 = true;
                        SDL_Event e2;
                        while(running2) {
                            while(SDL_PollEvent(&e2)) {
                                if(e2.type==SDL_QUIT) {
                                    running2 = false;
                                } else if(e2.type==SDL_MOUSEMOTION) {
                                    xm = e2.motion.x;
                                    ym = e2.motion.y;
                                } else if(e2.type==SDL_MOUSEBUTTONDOWN) {
                                    isPressed2 = true;
                                } else if(e2.type==SDL_MOUSEBUTTONUP) {
                                    isPressed2 = false;
                                }
                            }
                            SDL_SetRenderDrawColor(renderer2, 0, 0, 0, 255);
                            SDL_RenderClear(renderer2);

                            // render the result
                            TTF_Font* font1 = TTF_OpenFont("GameOfSquids-1GMVL.ttf", 35);
                            SDL_Color color1 = {51, 255, 255};
                            SDL_Surface* res3Surface = TTF_RenderText_Solid(font1, "YOU DON'T PASS THE CHALLENGE!", color1);
                            res3 = SDL_CreateTextureFromSurface(renderer2, res3Surface);
                            SDL_FreeSurface(res3Surface);
                            SDL_QueryTexture(res3, NULL, NULL, &titleW, &titleH);
                            SDL_Rect res3Pos = {80, 150, titleW, titleH};
                            SDL_RenderCopy(renderer2, res3, NULL, &res3Pos);
                            SDL_DestroyTexture(res3);

                            // render the try again
                            TTF_Font* font2 = TTF_OpenFont("GameOfSquids-1GMVL.ttf", 30);
                            SDL_Color color2 = {255, 255, 0};
                            SDL_Color color5 = {255, 0, 0};
                            SDL_Surface* tryAgainSurface = TTF_RenderText_Solid(font2, "TRY AGAIN", color2);
                            tryAgain = SDL_CreateTextureFromSurface(renderer2, tryAgainSurface);
                            SDL_FreeSurface(tryAgainSurface);
                            SDL_QueryTexture(tryAgain, NULL, NULL, &titleW, &titleH);
                            SDL_Rect tryAgainPos = {330, 300, titleW, titleH};

                            // render the exit
                            SDL_Surface* quit2Surface = TTF_RenderText_Solid(font2, "QUIT", color2);
                            quit2 = SDL_CreateTextureFromSurface(renderer2, quit2Surface);
                            SDL_FreeSurface(quit2Surface);
                            SDL_QueryTexture(quit2, NULL, NULL, &titleW, &titleH);
                            SDL_Rect quit2Pos = {380, 450, titleW, titleH};

                            // check focus
                            if(checkFocus(xm, ym, tryAgainPos)) {
                                SDL_Surface* tmp = TTF_RenderText_Solid(font2, "TRY AGAIN", color5);
                                tryAgain = SDL_CreateTextureFromSurface(renderer2, tmp);
                                SDL_FreeSurface(tmp);
                                if(isPressed2) {
                                    close2();
                                    goto flag;
                                }
                            } else {
                                SDL_Surface* tmp = TTF_RenderText_Solid(font2, "TRY AGAIN", color2);
                                tryAgain = SDL_CreateTextureFromSurface(renderer2, tmp);
                                SDL_FreeSurface(tmp);
                            }

                            if(checkFocus(xm, ym, quit2Pos)) {
                                SDL_Surface* tmp = TTF_RenderText_Solid(font2, "QUIT", color5);
                                quit2 = SDL_CreateTextureFromSurface(renderer2, tmp);
                                SDL_FreeSurface(tmp);
                                if(isPressed2) {
                                    exit(0);
                                }
                            } else {
                                SDL_Surface* tmp = TTF_RenderText_Solid(font2, "QUIT", color2);
                                quit2 = SDL_CreateTextureFromSurface(renderer2, tmp);
                                SDL_FreeSurface(tmp);
                            }

                            SDL_RenderCopy(renderer2, tryAgain, NULL, &tryAgainPos);
                            SDL_DestroyTexture(tryAgain);
                            SDL_RenderCopy(renderer2, quit2, NULL, &quit2Pos);
                            SDL_DestroyTexture(quit2);

                            SDL_RenderPresent(renderer2);
                        }
                    }

                }
                TTF_CloseFont(font);
                SDL_DestroyRenderer(renderer);
                SDL_DestroyWindow(window);
                SDL_Quit();
                TTF_Quit();
            }
        } else {
            SDL_Surface* tmp = TTF_RenderText_Solid(font2, "SINGLEPLAYER", color2);
            enter1 = SDL_CreateTextureFromSurface(renderer1, tmp);
            SDL_FreeSurface(tmp);
        }

        if(checkFocus(xm, ym, multi1Pos)) {
            SDL_Surface* tmp = TTF_RenderText_Solid(font4, "MULTIPLAYER", color3);
            multi1 = SDL_CreateTextureFromSurface(renderer1, tmp);
            SDL_FreeSurface(tmp);
            if(isPressed1) {
                close1();
                // START OF THE MULTIPLAYER MODE
                // create the window
                flag1:
                window3 = SDL_CreateWindow("BrickBreaker", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH3, HEIGHT3, SDL_WINDOW_SHOWN);
                if(window3==NULL) {
                    std::cout << "Failed to create the window of the multi player mode! Error: " << SDL_GetError() << std::endl;
                }

                // create the renderer
                renderer3 = SDL_CreateRenderer(window3, -1, 0);
                if(renderer3==NULL) {
                    std::cout << "Failed to create the renderer of the multi player mode! Error: " << SDL_GetError() << std::endl;
                }

                // define the font
                font3 = TTF_OpenFont("G7TwinbeeForArcadeTtf-VqYl.ttf", FONT_SIZE);
                if(font3==NULL) {
                    std::cout << "Failed to load the font of the game! Error: " << SDL_GetError() << std::endl;
                }
                running3 = 1;
                static int lastTime = 0;
                color = {255, 255, 255};

                // set the icon for the window
                SDL_SetWindowIcon(window3, icon);

                // some stats
                paddle1.h=paddle2.h=12;
                paddle1.w=paddle2.w=WIDTH_L/4;
                paddle1.y=paddle2.y=HEIGHT-paddle.h-32;
                ball1.w=ball1.h=ball2.w=ball2.h=SIZE;
                brick1.w=brick2.w=(WIDTH_L-(SPACING*COL))/COL;
                brick1.h=brick2.h=22;

                // reset
                resetBrickForPlayer1();
                resetBrickForPlayer2();

                // game loop
                while(running3) {
                    lastFrame=SDL_GetTicks();
                    if(lastFrame>=lastTime+1000) {
                      lastTime=lastFrame;
                      fps=frameCount;
                      frameCount=0;
                    }
                    updateForPlayer1();
                    updateForPlayer2();
                    handleInput3();

                    // handle the result
                    if((timer1==0 && score1>score2) || heart2==0) {
                        close3();
                        // START OF THE RESULT WINDOW
                        // create the window
                        window4 = SDL_CreateWindow("BrickBreaker", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH1, HEIGHT1, SDL_WINDOW_SHOWN);
                        if(window4==NULL) {
                            std::cout << "Failed to create the window of the multi player result! Error: " << SDL_GetError() << std::endl;
                        }

                        // create the renderer
                        renderer4 = SDL_CreateRenderer(window4, -1, 0);
                        if(renderer4==NULL) {
                            std::cout << "Failed to create the renderer of the multi player result! Error: " << SDL_GetError() << std::endl;
                        }

                        // set the icon for the window
                        SDL_SetWindowIcon(window4, icon);

                        // game loop
                        bool running4 = true;
                        SDL_Event e4;
                        while(running4) {
                            while(SDL_PollEvent(&e4)) {
                                if(e4.type==SDL_QUIT) {
                                    running4 = false;
                                } else if(e4.type==SDL_MOUSEMOTION) {
                                    xm = e4.motion.x;
                                    ym = e4.motion.y;
                                } else if(e4.type==SDL_MOUSEBUTTONDOWN) {
                                    isPressed4 = true;
                                } else if(e4.type==SDL_MOUSEBUTTONUP) {
                                    isPressed4 = false;
                                }
                            }
                            SDL_SetRenderDrawColor(renderer4, 0, 0, 0, 255);
                            SDL_RenderClear(renderer4);

                            // render the result
                            TTF_Font* font5 = TTF_OpenFont("GameOfSquids-1GMVL.ttf", 40);
                            SDL_Color color5 = {51, 255, 255};
                            SDL_Surface* res4Surface = TTF_RenderText_Solid(font5, "PlAYER 1 WIN THE MATCH!", color5);
                            res4 = SDL_CreateTextureFromSurface(renderer4, res4Surface);
                            SDL_FreeSurface(res4Surface);
                            SDL_QueryTexture(res4, NULL, NULL, &titleW, &titleH);
                            SDL_Rect res4Pos = {30, 100, titleW, titleH};
                            SDL_RenderCopy(renderer4, res4, NULL, &res4Pos);
                            SDL_DestroyTexture(res4);

                            // render the try again
                            TTF_Font* font6 = TTF_OpenFont("GameOfSquids-1GMVL.ttf", 30);
                            SDL_Color color6 = {255, 255, 0};
                            SDL_Surface* tryAgain4Surface = TTF_RenderText_Solid(font6, "TRY AGAIN", color6);
                            tryAgain4 = SDL_CreateTextureFromSurface(renderer4, tryAgain4Surface);
                            SDL_FreeSurface(tryAgain4Surface);
                            SDL_QueryTexture(tryAgain4, NULL, NULL, &titleW, &titleH);
                            SDL_Rect tryAgain4Pos = {230, 300, titleW, titleH};

                            // render the quit
                            SDL_Surface* quit4Surface = TTF_RenderText_Solid(font6, "QUIT", color6);
                            quit4 = SDL_CreateTextureFromSurface(renderer4, quit4Surface);
                            SDL_FreeSurface(quit4Surface);
                            SDL_QueryTexture(quit4, NULL, NULL, &titleW, &titleH);
                            SDL_Rect quit4Pos = {275, 500, titleW, titleH};

                            // check focus for the button
                            if(checkFocus(xm, ym, tryAgain4Pos)) {
                                SDL_Surface* tmp = TTF_RenderText_Solid(font6, "TRY AGAIN", color3);
                                tryAgain4 = SDL_CreateTextureFromSurface(renderer4, tmp);
                                SDL_FreeSurface(tmp);
                                if(isPressed4) {
                                    close4();
                                    goto flag1;
                                }
                            } else {
                                SDL_Surface* tmp = TTF_RenderText_Solid(font6, "TRY AGAIN", color6);
                                tryAgain4 = SDL_CreateTextureFromSurface(renderer4, tmp);
                                SDL_FreeSurface(tmp);
                            }

                            if(checkFocus(xm, ym, quit4Pos)) {
                                SDL_Surface* tmp = TTF_RenderText_Solid(font6, "QUIT", color3);
                                quit4 = SDL_CreateTextureFromSurface(renderer4, tmp);
                                SDL_FreeSurface(tmp);
                                if(isPressed4) {
                                    exit(0);
                                }
                            } else {
                                SDL_Surface* tmp = TTF_RenderText_Solid(font6, "QUIT", color6);
                                quit4 = SDL_CreateTextureFromSurface(renderer4, tmp);
                                SDL_FreeSurface(tmp);
                            }

                            // render copy the try again and the quit button
                            SDL_RenderCopy(renderer4, tryAgain4, NULL, &tryAgain4Pos);
                            SDL_DestroyTexture(tryAgain4);
                            SDL_RenderCopy(renderer4, quit4, NULL, &quit4Pos);
                            SDL_DestroyTexture(quit4);

                            SDL_RenderPresent(renderer4);
                        }
                    }

                    if((timer1==0 && score1<score2) || heart1==0) {
                        close3();
                        // START OF THE RESULT WINDOW
                        // create the window
                        window4 = SDL_CreateWindow("BrickBreaker", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH1, HEIGHT1, SDL_WINDOW_SHOWN);
                        if(window4==NULL) {
                            std::cout << "Failed to create the window of the multi player result! Error: " << SDL_GetError() << std::endl;
                        }

                        // create the renderer
                        renderer4 = SDL_CreateRenderer(window4, -1, 0);
                        if(renderer4==NULL) {
                            std::cout << "Failed to create the renderer of the multi player result! Error: " << SDL_GetError() << std::endl;
                        }

                        // set the icon for the window
                        SDL_SetWindowIcon(window4, icon);

                        // game loop
                        bool running4 = true;
                        SDL_Event e4;
                        while(running4) {
                            while(SDL_PollEvent(&e4)) {
                                if(e4.type==SDL_QUIT) {
                                    running4 = false;
                                } else if(e4.type==SDL_MOUSEMOTION) {
                                    xm = e4.motion.x;
                                    ym = e4.motion.y;
                                } else if(e4.type==SDL_MOUSEBUTTONDOWN) {
                                    isPressed4 = true;
                                } else if(e4.type==SDL_MOUSEBUTTONUP) {
                                    isPressed4 = false;
                                }
                            }
                            SDL_SetRenderDrawColor(renderer4, 0, 0, 0, 255);
                            SDL_RenderClear(renderer4);

                            // render the result
                            TTF_Font* font5 = TTF_OpenFont("GameOfSquids-1GMVL.ttf", 40);
                            SDL_Color color5 = {51, 255, 255};
                            SDL_Surface* res4Surface = TTF_RenderText_Solid(font5, "PlAYER 2 WIN THE MATCH!", color5);
                            res4 = SDL_CreateTextureFromSurface(renderer4, res4Surface);
                            SDL_FreeSurface(res4Surface);
                            SDL_QueryTexture(res4, NULL, NULL, &titleW, &titleH);
                            SDL_Rect res4Pos = {30, 100, titleW, titleH};
                            SDL_RenderCopy(renderer4, res4, NULL, &res4Pos);
                            SDL_DestroyTexture(res4);

                            // render the try again
                            TTF_Font* font6 = TTF_OpenFont("GameOfSquids-1GMVL.ttf", 30);
                            SDL_Color color6 = {255, 255, 0};
                            SDL_Surface* tryAgain4Surface = TTF_RenderText_Solid(font6, "TRY AGAIN", color6);
                            tryAgain4 = SDL_CreateTextureFromSurface(renderer4, tryAgain4Surface);
                            SDL_FreeSurface(tryAgain4Surface);
                            SDL_QueryTexture(tryAgain4, NULL, NULL, &titleW, &titleH);
                            SDL_Rect tryAgain4Pos = {230, 300, titleW, titleH};

                            // render the quit
                            SDL_Surface* quit4Surface = TTF_RenderText_Solid(font6, "QUIT", color6);
                            quit4 = SDL_CreateTextureFromSurface(renderer4, quit4Surface);
                            SDL_FreeSurface(quit4Surface);
                            SDL_QueryTexture(quit4, NULL, NULL, &titleW, &titleH);
                            SDL_Rect quit4Pos = {275, 500, titleW, titleH};

                            // check focus for the button
                            if(checkFocus(xm, ym, tryAgain4Pos)) {
                                SDL_Surface* tmp = TTF_RenderText_Solid(font6, "TRY AGAIN", color3);
                                tryAgain4 = SDL_CreateTextureFromSurface(renderer4, tmp);
                                SDL_FreeSurface(tmp);
                                if(isPressed4) {
                                    close4();
                                    goto flag1;
                                }
                            } else {
                                SDL_Surface* tmp = TTF_RenderText_Solid(font6, "TRY AGAIN", color6);
                                tryAgain4 = SDL_CreateTextureFromSurface(renderer4, tmp);
                                SDL_FreeSurface(tmp);
                            }

                            if(checkFocus(xm, ym, quit4Pos)) {
                                SDL_Surface* tmp = TTF_RenderText_Solid(font6, "QUIT", color3);
                                quit4 = SDL_CreateTextureFromSurface(renderer4, tmp);
                                SDL_FreeSurface(tmp);
                                if(isPressed4) {
                                    exit(0);
                                }
                            } else {
                                SDL_Surface* tmp = TTF_RenderText_Solid(font6, "QUIT", color6);
                                quit4 = SDL_CreateTextureFromSurface(renderer4, tmp);
                                SDL_FreeSurface(tmp);
                            }

                            // render copy the try again and the quit button
                            SDL_RenderCopy(renderer4, tryAgain4, NULL, &tryAgain4Pos);
                            SDL_DestroyTexture(tryAgain4);
                            SDL_RenderCopy(renderer4, quit4, NULL, &quit4Pos);
                            SDL_DestroyTexture(quit4);

                            SDL_RenderPresent(renderer4);
                        }
                    }
                    if(timer1==0 && score1==score2) {
                        timer1+=60;
                        timer2+=60;
                    }
                    render3();
                }


            }
        } else {
            SDL_Surface* tmp = TTF_RenderText_Solid(font2, "MULTIPLAYER", color2);
            multi1 = SDL_CreateTextureFromSurface(renderer1, tmp);
            SDL_FreeSurface(tmp);
        }

        if(checkFocus(xm, ym, challenge1Pos)) {
            SDL_Surface* tmp = TTF_RenderText_Solid(font2, "CHALLENGE", color3);
            challenge1 = SDL_CreateTextureFromSurface(renderer1, tmp);
            SDL_FreeSurface(tmp);
            if(isPressed1) {
                close1();
                // START OF THE CHALLENGE MODES WINDOW
                // create the window
                window5 = SDL_CreateWindow("BrickBreaker", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, SDL_WINDOW_SHOWN);
                if(window5==NULL) {
                    std::cout << "Failed to create the window of the challenge modes! Error: " << SDL_GetError() << std::endl;
                }

                // create the renderer
                renderer5 = SDL_CreateRenderer(window5, -1, 0);
                if(renderer5==NULL) {
                    std::cout << "Failed to create the renderer of the challenge modes! Error: " << SDL_GetError() << std::endl;
                }

                // set the icon for the window
                SDL_SetWindowIcon(window5, icon);

                // game loop
                bool running5 = true;
                SDL_Event e5;
                while(running5) {
                    while(SDL_PollEvent(&e5)) {
                        if(e5.type==SDL_QUIT) running5 = false;
                        else if(e5.type==SDL_MOUSEMOTION) {
                            xm = e5.motion.x;
                            ym = e5.motion.y;
                        } else if(e5.type==SDL_MOUSEBUTTONDOWN) isPressed5 = true;
                        else if(e5.type==SDL_MOUSEBUTTONUP) isPressed5 = false;
                    }
                    SDL_SetRenderDrawColor(renderer5, 0, 0, 0, 255);
                    SDL_RenderClear(renderer5);

                    // render the title of the modes
                    TTF_Font* font5 = TTF_OpenFont("GameOfSquids-1GMVL.ttf", 60);
                    SDL_Color color5 = {51, 255, 255};
                    SDL_Surface* title5Surface = TTF_RenderText_Solid(font5, "CHALLENGE MODES", color5);
                    title5 = SDL_CreateTextureFromSurface(renderer5, title5Surface);
                    SDL_FreeSurface(title5Surface);
                    SDL_QueryTexture(title5, NULL, NULL, &titleW, &titleH);
                    SDL_Rect title5Pos = {70, 50, titleW, titleH};

                    // render the 1v1 mode
                    TTF_Font* font8 = TTF_OpenFont("GameOfSquids-1GMVL.ttf", 40);
                    SDL_Color color8 = {255, 255, 0};
                    SDL_Surface* mode1Surface = TTF_RenderText_Solid(font8, "1v1 VS AI", color8);
                    mode1 = SDL_CreateTextureFromSurface(renderer5, mode1Surface);
                    SDL_FreeSurface(mode1Surface);
                    SDL_QueryTexture(mode1, NULL, NULL, &titleW, &titleH);
                    SDL_Rect mode1Pos = {300, 250, titleW, titleH};

                    // render the 2v2 mode
                    SDL_Surface* mode2Surface = TTF_RenderText_Solid(font8, "2v2 VS AI", color8);
                    mode2 = SDL_CreateTextureFromSurface(renderer5, mode2Surface);
                    SDL_FreeSurface(mode2Surface);
                    SDL_QueryTexture(mode2, NULL, NULL, &titleW, &titleH);
                    SDL_Rect mode2Pos = {265, 400, titleW, titleH};

                    // render the rush mode
                    SDL_Surface* mode3Surface = TTF_RenderText_Solid(font8, "RUSH'N'BREAK", color8);
                    mode3 = SDL_CreateTextureFromSurface(renderer5, mode3Surface);
                    SDL_FreeSurface(mode3Surface);
                    SDL_QueryTexture(mode3, NULL, NULL, &titleW, &titleH);
                    SDL_Rect mode3Pos = {245, 550, titleW, titleH};

                    // render the back button
                    SDL_Surface* back5Surface = TTF_RenderText_Solid(font8, "BACK", color8);
                    back5 = SDL_CreateTextureFromSurface(renderer5, back5Surface);
                    SDL_FreeSurface(back5Surface);
                    SDL_QueryTexture(back5, NULL, NULL, &titleW, &titleH);
                    SDL_Rect back5Pos = {345, 700, titleW, titleH};

                    // check focus
                    if(checkFocus(xm, ym, mode1Pos)) {
                        SDL_Surface* tmp = TTF_RenderText_Solid(font8, "1v1 VS AI", color3);
                        mode1 = SDL_CreateTextureFromSurface(renderer5, tmp);
                        SDL_FreeSurface(tmp);
                        if(isPressed5) {
                            close5();
                            // START OF THE 1v1 VS AI MODE
                            // create the window
                            flag2:
                            window6 = SDL_CreateWindow("BrickBreaker", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH3, HEIGHT3, SDL_WINDOW_SHOWN);
                            if(window6==NULL) {
                                std::cout << "Failed to create the window of the first mode! Error: " << std::endl;
                            }

                            // create the renderer
                            renderer6 = SDL_CreateRenderer(window6, -1, 0);
                            if(renderer6==NULL) {
                                std::cout << "Failed to create the renderer of the first mode! Error: " << std::endl;
                            }

                            // set the icon for the window
                            SDL_SetWindowIcon(window6, icon);

                            // define the font and the color
                            font10 = TTF_OpenFont("G7TwinbeeForArcadeTtf-VqYl.ttf", FONT_SIZE);
                            if(font10==NULL) {
                                std::cout << "Failed to load the font of the game! Error: " << SDL_GetError() << std::endl;
                            }
                            running6 = 1;
                            static int lastTime = 0;
                            color = {255, 255, 255};

                            // some stats of the position
                            paddle3.h=paddle4.h=12;
                            paddle3.w=paddle4.w=WIDTH_L/4;
                            paddle3.y=paddle4.y=HEIGHT-paddle.h-32;
                            ball3.w=ball3.h=ball4.w=ball4.h=SIZE;
                            brick3.w=brick4.w=(WIDTH_L-(SPACING*COL))/COL;
                            brick3.h=brick4.h=22;

                            // reset the brick
                            resetBrickForThePlayer();
                            resetBrickForTheAI();

                            // game loop
                            while(running6) {
                                // limiting the frame
                                lastFrame=SDL_GetTicks();
                                if(lastFrame>=lastTime+1000) {
                                  lastTime=lastFrame;
                                  fps=frameCount;
                                  frameCount=0;
                                }
                                updateForThePlayer();
                                updateForTheAI();
                                handleInput6();
                                if((timer3==0 && score3>score4) || heart4==0) {
                                    close6();
                                    // START OF THE RESULT WINDOW
                                    // create the window
                                    window7 = SDL_CreateWindow("BrickBreaker", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, SDL_WINDOW_SHOWN);
                                    if(window7==NULL) {
                                        std::cout << "Failed to create the window of the result! Error: " << SDL_GetError() << std::endl;
                                    }

                                    // create the renderer
                                    renderer7 = SDL_CreateRenderer(window7, -1, 0);
                                    if(renderer7==NULL) {
                                        std::cout << "Failed to create the renderer of the result! Error: " << std::endl;
                                    }

                                    // set the icon for the window
                                    SDL_SetWindowIcon(window7, icon);

                                    // game loop
                                    bool running7 = true;
                                    SDL_Event e7;
                                    while(running7) {
                                        while(SDL_PollEvent(&e7)) {
                                            if(e7.type==SDL_QUIT) {
                                                running7 = false;
                                            } else if(e7.type==SDL_MOUSEMOTION) {
                                                xm = e7.motion.x;
                                                ym = e7.motion.y;
                                            } else if(e7.type==SDL_MOUSEBUTTONDOWN) {
                                                isPressed7 = true;
                                            } else if(e7.type==SDL_MOUSEBUTTONUP) {
                                                isPressed7 = false;
                                            }
                                        }
                                        SDL_SetRenderDrawColor(renderer7, 0, 0, 0, 255);
                                        SDL_RenderClear(renderer7);

                                        // render the result on the screen
                                        TTF_Font* font9 = TTF_OpenFont("GameOfSquids-1GMVL.ttf", 50);
                                        SDL_Color color9 = {51, 255, 255};
                                        SDL_Surface* res7Surface = TTF_RenderText_Solid(font9, "YOU HAVE DEFEATED THE AI!", color9);
                                        res7 = SDL_CreateTextureFromSurface(renderer7, res7Surface);
                                        SDL_FreeSurface(res7Surface);
                                        SDL_QueryTexture(res7, NULL, NULL, &titleW, &titleH);
                                        SDL_Rect res7Pos = {15, 100, titleW, titleH};

                                        // render the try again on the screen
                                        TTF_Font* font11 = TTF_OpenFont("GameOfSquids-1GMVL.ttf", 40);
                                        SDL_Color color11 = {255, 255, 0};
                                        SDL_Surface* tryAgain7Surface = TTF_RenderText_Solid(font11, "TRY AGAIN", color11);
                                        tryAgain7 = SDL_CreateTextureFromSurface(renderer7, tryAgain7Surface);
                                        SDL_FreeSurface(tryAgain7Surface);
                                        SDL_QueryTexture(tryAgain7, NULL, NULL, &titleW, &titleH);
                                        SDL_Rect tryAgain7Pos = {280, 300, titleW, titleH};

                                        // render the quit on the screen
                                        SDL_Surface* quit7Surface = TTF_RenderText_Solid(font11, "QUIT", color11);
                                        quit7 = SDL_CreateTextureFromSurface(renderer7, quit7Surface);
                                        SDL_FreeSurface(quit7Surface);
                                        SDL_QueryTexture(quit7, NULL, NULL, &titleW, &titleH);
                                        SDL_Rect quit7Pos = {350, 450, titleW, titleH};

                                        // check focus
                                        if(checkFocus(xm, ym, tryAgain7Pos)) {
                                            SDL_Surface* tmp = TTF_RenderText_Solid(font11, "TRY AGAIN", color3);
                                            tryAgain7 = SDL_CreateTextureFromSurface(renderer7, tmp);
                                            SDL_FreeSurface(tmp);
                                            if(isPressed7) {
                                                close7();
                                                goto flag2;
                                            }
                                        } else {
                                            SDL_Surface* tmp = TTF_RenderText_Solid(font11, "TRY AGAIN", color11);
                                            tryAgain7 = SDL_CreateTextureFromSurface(renderer7, tmp);
                                            SDL_FreeSurface(tmp);
                                        }

                                        if(checkFocus(xm, ym, quit7Pos)) {
                                            SDL_Surface* tmp = TTF_RenderText_Solid(font11, "QUIT", color3);
                                            quit7 = SDL_CreateTextureFromSurface(renderer7, tmp);
                                            SDL_FreeSurface(tmp);
                                            if(isPressed7) {
                                                exit(0);
                                            }
                                        } else {
                                            SDL_Surface* tmp = TTF_RenderText_Solid(font11, "QUIT", color11);
                                            quit7 = SDL_CreateTextureFromSurface(renderer7, tmp);
                                            SDL_FreeSurface(tmp);
                                        }


                                        // render copy
                                        SDL_RenderCopy(renderer7, res7, NULL, &res7Pos);
                                        SDL_DestroyTexture(res7);
                                        SDL_RenderCopy(renderer7, tryAgain7, NULL, &tryAgain7Pos);
                                        SDL_DestroyTexture(tryAgain7);
                                        SDL_RenderCopy(renderer7, quit7, NULL, &quit7Pos);
                                        SDL_DestroyTexture(quit7);

                                        SDL_RenderPresent(renderer7);
                                    }

                                } else if((timer3==0 && score3<score4) || heart4==0) {
                                    // create the window
                                    window7 = SDL_CreateWindow("BrickBreaker", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, SDL_WINDOW_SHOWN);
                                    if(window7==NULL) {
                                        std::cout << "Failed to create the window of the result! Error: " << SDL_GetError() << std::endl;
                                    }

                                    // create the renderer
                                    renderer7 = SDL_CreateRenderer(window7, -1, 0);
                                    if(renderer7==NULL) {
                                        std::cout << "Failed to create the renderer of the result! Error: " << std::endl;
                                    }

                                    // set the icon for the window
                                    SDL_SetWindowIcon(window7, icon);

                                    // game loop
                                    bool running7 = true;
                                    SDL_Event e7;
                                    while(running7) {
                                        while(SDL_PollEvent(&e7)) {
                                            if(e7.type==SDL_QUIT) {
                                                running7 = false;
                                            } else if(e7.type==SDL_MOUSEMOTION) {
                                                xm = e7.motion.x;
                                                ym = e7.motion.y;
                                            } else if(e7.type==SDL_MOUSEBUTTONDOWN) {
                                                isPressed7 = true;
                                            } else if(e7.type==SDL_MOUSEBUTTONUP) {
                                                isPressed7 = false;
                                            }
                                        }
                                        SDL_SetRenderDrawColor(renderer7, 0, 0, 0, 255);
                                        SDL_RenderClear(renderer7);

                                        // render the result on the screen
                                        TTF_Font* font9 = TTF_OpenFont("GameOfSquids-1GMVL.ttf", 50);
                                        SDL_Color color9 = {51, 255, 255};
                                        SDL_Surface* res7Surface = TTF_RenderText_Solid(font9, "THE AI HAVE DEFEATED YOU!", color9);
                                        res7 = SDL_CreateTextureFromSurface(renderer7, res7Surface);
                                        SDL_FreeSurface(res7Surface);
                                        SDL_QueryTexture(res7, NULL, NULL, &titleW, &titleH);
                                        SDL_Rect res7Pos = {15, 100, titleW, titleH};

                                        // render the try again on the screen
                                        TTF_Font* font11 = TTF_OpenFont("GameOfSquids-1GMVL.ttf", 40);
                                        SDL_Color color11 = {255, 255, 0};
                                        SDL_Surface* tryAgain7Surface = TTF_RenderText_Solid(font11, "TRY AGAIN", color11);
                                        tryAgain7 = SDL_CreateTextureFromSurface(renderer7, tryAgain7Surface);
                                        SDL_FreeSurface(tryAgain7Surface);
                                        SDL_QueryTexture(tryAgain7, NULL, NULL, &titleW, &titleH);
                                        SDL_Rect tryAgain7Pos = {280, 300, titleW, titleH};

                                        // render the quit on the screen
                                        SDL_Surface* quit7Surface = TTF_RenderText_Solid(font11, "QUIT", color11);
                                        quit7 = SDL_CreateTextureFromSurface(renderer7, quit7Surface);
                                        SDL_FreeSurface(quit7Surface);
                                        SDL_QueryTexture(quit7, NULL, NULL, &titleW, &titleH);
                                        SDL_Rect quit7Pos = {350, 450, titleW, titleH};

                                        // check focus
                                        if(checkFocus(xm, ym, tryAgain7Pos)) {
                                            SDL_Surface* tmp = TTF_RenderText_Solid(font11, "TRY AGAIN", color3);
                                            tryAgain7 = SDL_CreateTextureFromSurface(renderer7, tmp);
                                            SDL_FreeSurface(tmp);
                                            if(isPressed7) {
                                                close7();
                                                goto flag2;
                                            }
                                        } else {
                                            SDL_Surface* tmp = TTF_RenderText_Solid(font11, "TRY AGAIN", color11);
                                            tryAgain7 = SDL_CreateTextureFromSurface(renderer7, tmp);
                                            SDL_FreeSurface(tmp);
                                        }

                                        if(checkFocus(xm, ym, quit7Pos)) {
                                            SDL_Surface* tmp = TTF_RenderText_Solid(font11, "QUIT", color3);
                                            quit7 = SDL_CreateTextureFromSurface(renderer7, tmp);
                                            SDL_FreeSurface(tmp);
                                            if(isPressed7) {
                                                exit(0);
                                            }
                                        } else {
                                            SDL_Surface* tmp = TTF_RenderText_Solid(font11, "QUIT", color11);
                                            quit7 = SDL_CreateTextureFromSurface(renderer7, tmp);
                                            SDL_FreeSurface(tmp);
                                        }


                                        // render copy
                                        SDL_RenderCopy(renderer7, res7, NULL, &res7Pos);
                                        SDL_DestroyTexture(res7);
                                        SDL_RenderCopy(renderer7, tryAgain7, NULL, &tryAgain7Pos);
                                        SDL_DestroyTexture(tryAgain7);
                                        SDL_RenderCopy(renderer7, quit7, NULL, &quit7Pos);
                                        SDL_DestroyTexture(quit7);

                                        SDL_RenderPresent(renderer7);
                                    }
                                }
                                render6();
                            }

                        }
                    } else {
                        SDL_Surface* tmp = TTF_RenderText_Solid(font8, "1v1 VS AI", color8);
                        mode1 = SDL_CreateTextureFromSurface(renderer5, tmp);
                        SDL_FreeSurface(tmp);
                    }

                    if(checkFocus(xm, ym, mode2Pos)) {
                        SDL_Surface* tmp = TTF_RenderText_Solid(font8, "2v2 VS AI", color3);
                        mode2 = SDL_CreateTextureFromSurface(renderer5, tmp);
                        SDL_FreeSurface(tmp);
                        if(isPressed5) {
                            close5();
                            // START OF THE 2V2 MODE
                            // create the window
                            flag3:
                            window8 = SDL_CreateWindow("BrickBreaker", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH3, HEIGHT3, SDL_WINDOW_SHOWN);
                            if(window8==NULL) {
                                std::cout << "Failed to create the window of the second mode! Error: " << SDL_GetError() << std::endl;
                            }

                            // create the renderer
                            renderer8 = SDL_CreateRenderer(window8, -1, 0);
                            if(renderer8==NULL) {
                                std::cout << "Failed to create the renderer of the second mode! Error: " << SDL_GetError() << std::endl;
                            }

                            // set the icon of the window
                            SDL_SetWindowIcon(window8, icon);

                            // some stats of the position
                            paddle5.h=paddle6.h=paddle7.h=paddle8.h=12;
                            paddle5.w=paddle6.w=paddle7.w=paddle8.w=WIDTH_L/8;
                            paddle5.y=paddle6.y=paddle7.y=paddle8.y=HEIGHT-paddle.h-32;
                            ball5.w=ball5.h=ball6.w=ball6.h=ball7.w=ball7.h=ball8.w=ball8.h=SIZE8;
                            brick5.w=brick6.w=(WIDTH_L-(SPACING*COL8))/COL8;
                            brick5.h=brick6.h=22;

                            // define the font and the color
                            static int lastTime = 0;
                            running8=1;

                            // reset the brick
                            resetBrickForPlayerTeam();
                            resetBrickForAITeam();

                            // game loop
                            while(running8) {
                                // limiting the frame
                                lastFrame=SDL_GetTicks();
                                if(lastFrame>=lastTime+1000) {
                                  lastTime=lastFrame;
                                  fps=frameCount;
                                  frameCount=0;
                                }
                                updateForThePlayerTeam();
                                updateForTheAITeam();
                                handleInput8();

                                // logic to create the result window
                                if((timer==0 && score5>score6) || heart6==0) {
                                    close8();

                                    // START OF THE RESULT WINDOW
                                    // create the window
                                    window10 = SDL_CreateWindow("BrickBreaker", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, SDL_WINDOW_SHOWN);
                                    if(window10==NULL) {
                                        std::cout << "Failed to create the window of the result window! Error: " << SDL_GetError() << std::endl;
                                    }

                                    // create the renderer
                                    renderer10 = SDL_CreateRenderer(window10, -1, 0);
                                    if(renderer10==NULL) {
                                        std::cout << "Failed to create the renderer of the result window! Error: " << SDL_GetError() << std::endl;
                                    }

                                    // set the icon of the window
                                    SDL_SetWindowIcon(window10, icon);

                                    // game loop
                                    bool running10 = true;
                                    SDL_Event e10;
                                    while(running10) {
                                        while(SDL_PollEvent(&e10)) {
                                            if(e10.type==SDL_QUIT) {
                                                running10=false;
                                            } else if(e10.type==SDL_MOUSEMOTION) {
                                                xm = e10.motion.x;
                                                ym = e10.motion.y;
                                            } else if(e10.type==SDL_MOUSEBUTTONDOWN) {
                                                isPressed10 = true;
                                            } else if(e10.type==SDL_MOUSEBUTTONUP) {
                                                isPressed10 = false;
                                            }
                                        }
                                        SDL_SetRenderDrawColor(renderer10, 0, 0, 0, 255);
                                        SDL_RenderClear(renderer10);

                                        // render the title on the screen
                                        TTF_Font* font11 = TTF_OpenFont("GameOfSquids-1GMVL.ttf", 60);
                                        SDL_Color color11 = {51, 255, 255};
                                        SDL_Surface* title10Surface = TTF_RenderText_Solid(font11, "PLAYER TEAM WIN!", color11);
                                        title10 = SDL_CreateTextureFromSurface(renderer10, title10Surface);
                                        SDL_FreeSurface(title10Surface);
                                        SDL_QueryTexture(title10, NULL, NULL, &titleW, &titleH);
                                        SDL_Rect title10Pos = {100, 100, titleW, titleH};

                                        // render the try again on the screen
                                        TTF_Font* font12 = TTF_OpenFont("GameOfSquids-1GMVL.ttf", 50);
                                        SDL_Color color13 = {255, 255, 0};
                                        SDL_Surface* tryAgain10Surface = TTF_RenderText_Solid(font12, "TRY AGAIN", color13);
                                        tryAgain10 = SDL_CreateTextureFromSurface(renderer10, tryAgain10Surface);
                                        SDL_FreeSurface(tryAgain10Surface);
                                        SDL_QueryTexture(tryAgain10, NULL, NULL, &titleW, &titleH);
                                        SDL_Rect tryAgain10Pos = {250, 300, titleW, titleH};

                                        // render the exit on the screen
                                        SDL_Surface* quit10Surface = TTF_RenderText_Solid(font12, "QUIT", color13);
                                        quit10 = SDL_CreateTextureFromSurface(renderer10, quit10Surface);
                                        SDL_FreeSurface(quit10Surface);
                                        SDL_QueryTexture(quit10, NULL, NULL, &titleW, &titleH);
                                        SDL_Rect quit10Pos = {330, 450, titleW, titleH};

                                        // check focus
                                        if(checkFocus(xm, ym, tryAgain10Pos)) {
                                            SDL_Surface* tmp = TTF_RenderText_Solid(font12, "TRY AGAIN", color3);
                                            tryAgain10 = SDL_CreateTextureFromSurface(renderer10, tmp);
                                            SDL_FreeSurface(tmp);
                                            if(isPressed10) {
                                                close10();
                                                goto flag3;
                                            }
                                        } else {
                                            SDL_Surface* tmp = TTF_RenderText_Solid(font12, "TRY AGAIN", color13);
                                            tryAgain10 = SDL_CreateTextureFromSurface(renderer10, tmp);
                                            SDL_FreeSurface(tmp);
                                        }

                                        if(checkFocus(xm, ym, quit10Pos)) {
                                            SDL_Surface* tmp = TTF_RenderText_Solid(font12, "QUIT", color3);
                                            quit10 = SDL_CreateTextureFromSurface(renderer10, tmp);
                                            SDL_FreeSurface(tmp);
                                            if(isPressed10) {
                                                exit(0);
                                            }
                                        } else {
                                            SDL_Surface* tmp = TTF_RenderText_Solid(font12, "QUIT", color13);
                                            quit10 = SDL_CreateTextureFromSurface(renderer10, tmp);
                                            SDL_FreeSurface(tmp);
                                        }


                                        // render copy
                                        SDL_RenderCopy(renderer10, title10, NULL, &title10Pos);
                                        SDL_DestroyTexture(title10);
                                        SDL_RenderCopy(renderer10, tryAgain10, NULL, &tryAgain10Pos);
                                        SDL_DestroyTexture(tryAgain10);
                                        SDL_RenderCopy(renderer10, quit10, NULL, &quit10Pos);

                                        SDL_RenderPresent(renderer10);
                                    }
                                } else if((timer5==0 && score5<score6) || heart5==0) {
                                    close8();

                                    // START OF THE RESULT WINDOW
                                    // create the window
                                    window10 = SDL_CreateWindow("BrickBreaker", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, SDL_WINDOW_SHOWN);
                                    if(window10==NULL) {
                                        std::cout << "Failed to create the window of the result window! Error: " << SDL_GetError() << std::endl;
                                    }

                                    // create the renderer
                                    renderer10 = SDL_CreateRenderer(window10, -1, 0);
                                    if(renderer10==NULL) {
                                        std::cout << "Failed to create the renderer of the result window! Error: " << SDL_GetError() << std::endl;
                                    }

                                    // set the icon of the window
                                    SDL_SetWindowIcon(window10, icon);

                                    // game loop
                                    bool running10 = true;
                                    SDL_Event e10;
                                    while(running10) {
                                        while(SDL_PollEvent(&e10)) {
                                            if(e10.type==SDL_QUIT) {
                                                running10=false;
                                            } else if(e10.type==SDL_MOUSEMOTION) {
                                                xm = e10.motion.x;
                                                ym = e10.motion.y;
                                            } else if(e10.type==SDL_MOUSEBUTTONDOWN) {
                                                isPressed10 = true;
                                            } else if(e10.type==SDL_MOUSEBUTTONUP) {
                                                isPressed10 = false;
                                            }
                                        }
                                        SDL_SetRenderDrawColor(renderer10, 0, 0, 0, 255);
                                        SDL_RenderClear(renderer10);

                                        // render the title on the screen
                                        TTF_Font* font11 = TTF_OpenFont("GameOfSquids-1GMVL.ttf", 60);
                                        SDL_Color color11 = {51, 255, 255};
                                        SDL_Surface* title10Surface = TTF_RenderText_Solid(font11, "THE AI TEAM WIN!", color11);
                                        title10 = SDL_CreateTextureFromSurface(renderer10, title10Surface);
                                        SDL_FreeSurface(title10Surface);
                                        SDL_QueryTexture(title10, NULL, NULL, &titleW, &titleH);
                                        SDL_Rect title10Pos = {100, 100, titleW, titleH};

                                        // render the try again on the screen
                                        TTF_Font* font12 = TTF_OpenFont("GameOfSquids-1GMVL.ttf", 50);
                                        SDL_Color color13 = {255, 255, 0};
                                        SDL_Surface* tryAgain10Surface = TTF_RenderText_Solid(font12, "TRY AGAIN", color13);
                                        tryAgain10 = SDL_CreateTextureFromSurface(renderer10, tryAgain10Surface);
                                        SDL_FreeSurface(tryAgain10Surface);
                                        SDL_QueryTexture(tryAgain10, NULL, NULL, &titleW, &titleH);
                                        SDL_Rect tryAgain10Pos = {250, 300, titleW, titleH};

                                        // render the exit on the screen
                                        SDL_Surface* quit10Surface = TTF_RenderText_Solid(font12, "QUIT", color13);
                                        quit10 = SDL_CreateTextureFromSurface(renderer10, quit10Surface);
                                        SDL_FreeSurface(quit10Surface);
                                        SDL_QueryTexture(quit10, NULL, NULL, &titleW, &titleH);
                                        SDL_Rect quit10Pos = {330, 450, titleW, titleH};

                                        // check focus
                                        if(checkFocus(xm, ym, tryAgain10Pos)) {
                                            SDL_Surface* tmp = TTF_RenderText_Solid(font12, "TRY AGAIN", color3);
                                            tryAgain10 = SDL_CreateTextureFromSurface(renderer10, tmp);
                                            SDL_FreeSurface(tmp);
                                            if(isPressed10) {
                                                close10();
                                                goto flag3;
                                            }
                                        } else {
                                            SDL_Surface* tmp = TTF_RenderText_Solid(font12, "TRY AGAIN", color13);
                                            tryAgain10 = SDL_CreateTextureFromSurface(renderer10, tmp);
                                            SDL_FreeSurface(tmp);
                                        }

                                        if(checkFocus(xm, ym, quit10Pos)) {
                                            SDL_Surface* tmp = TTF_RenderText_Solid(font12, "QUIT", color3);
                                            quit10 = SDL_CreateTextureFromSurface(renderer10, tmp);
                                            SDL_FreeSurface(tmp);
                                            if(isPressed10) {
                                                exit(0);
                                            }
                                        } else {
                                            SDL_Surface* tmp = TTF_RenderText_Solid(font12, "QUIT", color13);
                                            quit10 = SDL_CreateTextureFromSurface(renderer10, tmp);
                                            SDL_FreeSurface(tmp);
                                        }


                                        // render copy
                                        SDL_RenderCopy(renderer10, title10, NULL, &title10Pos);
                                        SDL_DestroyTexture(title10);
                                        SDL_RenderCopy(renderer10, tryAgain10, NULL, &tryAgain10Pos);
                                        SDL_DestroyTexture(tryAgain10);
                                        SDL_RenderCopy(renderer10, quit10, NULL, &quit10Pos);

                                        SDL_RenderPresent(renderer10);
                                    }
                                }

                                render8();
                            }

                        }
                    } else {
                        SDL_Surface* tmp = TTF_RenderText_Solid(font8, "2v2 VS AI", color8);
                        mode2 = SDL_CreateTextureFromSurface(renderer5, tmp);
                        SDL_FreeSurface(tmp);
                    }

                    if(checkFocus(xm, ym, mode3Pos)) {
                        SDL_Surface* tmp = TTF_RenderText_Solid(font8, "RUSH'N'BREAK", color3);
                        mode3 = SDL_CreateTextureFromSurface(renderer5, tmp);
                        SDL_FreeSurface(tmp);
                        if(isPressed5) {
                            close5();
                            // START OF THE RUSH MODE WINDOW
                            // create the window
                            window9 = SDL_CreateWindow("BrickBreaker", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, SDL_WINDOW_SHOWN);
                            if(window9==NULL) {
                                std::cout << "Failed to create the window of the rush mode! Error: " << std::endl;
                            }

                            // create the renderer
                            renderer9 = SDL_CreateRenderer(window9, -1, 0);
                            if(renderer9==NULL) {
                                std::cout << "Failed to create the renderer of the rush mode! Error: " << std::endl;
                            }

                            // create the icon for the window
                            SDL_SetWindowIcon(window9, icon);

                            // define the font and other stats
                            static int lastTime = 0;
                            running9=1;
                            paddle9.h=12;paddle9.w=WIDTH/4;
                            paddle9.y= HEIGHT-paddle9.h-32;
                            ball9.w=ball9.h=SIZE;
                            brick9.w=(WIDTH-(SPACING*COL))/COL;
                            brick9.h=22;

                            // reset the brick
                            resetBrick9();

                            // game loop
                            while(running9) {
                                // limiting the framw
                                lastFrame=SDL_GetTicks();
                                if(lastFrame>=lastTime+1000) {
                                  lastTime=lastFrame;
                                  fps=frameCount;
                                  frameCount=0;
                                }
                                update9();
                                handleInput9();
                                render9();
                            }
                        }
                    } else {
                        SDL_Surface* tmp = TTF_RenderText_Solid(font8, "RUSH'N'BREAK", color8);
                        mode3 = SDL_CreateTextureFromSurface(renderer5, tmp);
                        SDL_FreeSurface(tmp);
                    }

                    if(checkFocus(xm, ym, back5Pos)) {
                        SDL_Surface* tmp = TTF_RenderText_Solid(font8, "BACK", color3);
                        back5 = SDL_CreateTextureFromSurface(renderer5, tmp);
                        SDL_FreeSurface(tmp);
                        if(isPressed5) {
                            close5();
                            goto flag;
                        }
                    } else {
                        SDL_Surface* tmp = TTF_RenderText_Solid(font8, "BACK", color8);
                        back5 = SDL_CreateTextureFromSurface(renderer5, tmp);
                        SDL_FreeSurface(tmp);
                    }


                    // render copy
                    SDL_RenderCopy(renderer5, title5, NULL, &title5Pos);
                    SDL_DestroyTexture(title5);
                    SDL_RenderCopy(renderer5, mode1, NULL, &mode1Pos);
                    SDL_DestroyTexture(mode1);
                    SDL_RenderCopy(renderer5, mode2, NULL, &mode2Pos);
                    SDL_DestroyTexture(mode2);
                    SDL_RenderCopy(renderer5, mode3, NULL, &mode3Pos);
                    SDL_DestroyTexture(mode3);
                    SDL_RenderCopy(renderer5, back5, NULL, &back5Pos);
                    SDL_DestroyTexture(back5);
                    SDL_RenderPresent(renderer5);
                }

            }
        } else {
            SDL_Surface* tmp = TTF_RenderText_Solid(font2, "CHALLENGE", color2);
            challenge1 = SDL_CreateTextureFromSurface(renderer1, tmp);
            SDL_FreeSurface(tmp);
        }

        if(checkFocus(xm, ym, exit1Pos)) {
            SDL_Surface* tmp = TTF_RenderText_Solid(font4, "EXIT", color3);
            exit1 = SDL_CreateTextureFromSurface(renderer1, tmp);
            SDL_FreeSurface(tmp);
            if(isPressed1) {
                exit(0);
            }
        } else {
            SDL_Surface* tmp = TTF_RenderText_Solid(font2, "EXIT", color2);
            exit1 = SDL_CreateTextureFromSurface(renderer1, tmp);
            SDL_FreeSurface(tmp);
        }

        // render copy
        SDL_RenderCopy(renderer1, title1, NULL, &title1Pos);
        SDL_DestroyTexture(title1);
        SDL_RenderCopy(renderer1, enter1, NULL, &enter1Pos);
        SDL_DestroyTexture(enter1);
        SDL_RenderCopy(renderer1, challenge1, NULL, &challenge1Pos);
        SDL_DestroyTexture(challenge1);
        SDL_RenderCopy(renderer1, multi1, NULL, &multi1Pos);
        SDL_DestroyTexture(multi1);
        SDL_RenderCopy(renderer1, exit1, NULL, &exit1Pos);
        SDL_DestroyTexture(exit1);

        SDL_RenderPresent(renderer1);
    }
    return 0;
}
