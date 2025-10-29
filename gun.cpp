#include <graphics.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <unistd.h>
#include <fstream>
#include <iostream>
#include <termios.h>
#include <fcntl.h>

using namespace std;

// Non-blocking keyboard input
int kbhit() {
    struct termios oldt, newt;
    int ch;
    int oldf;
    
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);
    
    ch = getchar();
    
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    fcntl(STDIN_FILENO, F_SETFL, oldf);
    
    if(ch != EOF) {
        ungetc(ch, stdin);
        return 1;
    }
    return 0;
}

// Abstract Base class for all game objects
class GameObject {
protected:
    int x, y;
    bool active;
public:
    GameObject(int x1, int y1) : x(x1), y(y1), active(true) {}
    virtual void draw() = 0;  // Pure virtual function
    virtual void update() = 0;
    int getX() { return x; }
    int getY() { return y; }
    bool isActive() { return active; }
    void setActive(bool a) { active = a; }
    virtual ~GameObject() {}  // Virtual destructor
};

// Gun class with lives system
class Gun : public GameObject {
private:
    int lives;
public:
    Gun(int x1, int y1) : GameObject(x1, y1), lives(3) {}
    
    void draw() {
        // Gun turret - triangular shape
        setcolor(CYAN);
        line(x, y-35, x-15, y);
        line(x, y-35, x+15, y);
        line(x-15, y, x+15, y);
        
        // Gun base - wider platform
        setcolor(LIGHTGRAY);
        line(x-20, y, x+20, y);
        line(x-20, y, x-18, y+5);
        line(x+20, y, x+18, y+5);
        line(x-18, y+5, x+18, y+5);
        
        // Gun barrel
        setcolor(DARKGRAY);
        line(x-3, y-15, x-3, y-35);
        line(x+3, y-15, x+3, y-35);
        
        // Wheels
        setcolor(WHITE);
        circle(x-12, y+8, 3);
        circle(x+12, y+8, 3);
    }
    
    void update() {}
    void moveLeft() { if(x > 40) x -= 15; }
    void moveRight() { if(x < 600) x += 15; }
    
    int getLives() { return lives; }
    void loseLife() { lives--; }
    void addLife() { if(lives < 5) lives++; }
};

// Bullet class
class Bullet : public GameObject {
public:
    Bullet(int x1, int y1) : GameObject(x1, y1) {}
    
    void draw() {
        setcolor(YELLOW);
        circle(x, y, 4);
        
        // Bullet trail
        setcolor(WHITE);
        circle(x, y+5, 2);
    }
    
    void update() {
        y -= 15;
        if(y < 0) active = false;
    }
};

// Base Target class - demonstrates inheritance hierarchy
class Target : public GameObject {
protected:
    int dir, speed, color, points, radius;
public:
    Target(int x1, int y1, int s, int p, int r) 
        : GameObject(x1, y1), speed(s), points(p), radius(r) {
        dir = (rand() % 2) ? 1 : -1;
        // Colorful targets - random bright colors
        int colors[] = {RED, GREEN, BLUE, YELLOW, CYAN, MAGENTA, LIGHTRED, LIGHTGREEN, LIGHTBLUE, LIGHTCYAN, LIGHTMAGENTA};
        color = colors[rand() % 11];
    }
    
    virtual void draw() {
        setcolor(color);
        // Draw filled solid ball with no gaps
        for(int i = radius; i > 0; i--) {
            circle(x, y, i);
        }
        
        // Add shiny highlight for 3D effect
        setcolor(WHITE);
        for(int i = radius/4; i > 0; i--) {
            circle(x-radius/3, y-radius/3, i);
        }
    }
    
    void update() {
        x += (int)(dir * speed * 1.3);
        if(x <= 40 || x >= 600) {
            dir *= -1;
            // Keep target within bounds
            if(x < 40) x = 40;
            if(x > 600) x = 600;
        }
    }
    
    bool hit(Bullet* b) {
        if(!b->isActive()) return false;
        int dx = b->getX() - x;
        int dy = b->getY() - y;
        int hitRange = radius * radius;
        if(dx*dx + dy*dy <= hitRange) {
            active = false;
            b->setActive(false);
            return true;
        }
        return false;
    }
    
    int getPoints() { return points; }
    virtual ~Target() {}
};

// Fast Target - Polymorphism example
class FastTarget : public Target {
public:
    FastTarget(int x1, int y1, int level) 
        : Target(x1, y1, 4 + level, 20, 12) {
        color = LIGHTRED;
    }
    
    void draw() {
        // Fast target - solid ball with motion blur effect
        setcolor(LIGHTRED);
        for(int i = radius; i > 0; i--) {
            circle(x, y, i);
        }
        
        // Triple speed lines for extra spice!
        setcolor(YELLOW);
        line(x-25, y, x-12, y);
        line(x+12, y, x+25, y);
        line(x-25, y-4, x-12, y-4);
        line(x+12, y-4, x+25, y-4);
        line(x-25, y+4, x-12, y+4);
        line(x+12, y+4, x+25, y+4);
        
        // Glowing highlight
        setcolor(WHITE);
        for(int i = radius/4; i > 0; i--) {
            circle(x-radius/3, y-radius/3, i);
        }
        
        // Add stars for extra spice
        setcolor(YELLOW);
        putpixel(x-radius-5, y, YELLOW);
        putpixel(x+radius+5, y, YELLOW);
    }
};

// Bonus Target - gives extra life
class BonusTarget : public Target {
private:
    static int flashCounter;
public:
    BonusTarget(int x1, int y1) 
        : Target(x1, y1, 3, 50, 18) {
        color = GREEN;
    }
    
    void draw() {
        // Animated bonus target - filled solid ball with glow
        flashCounter = (flashCounter + 1) % 20;
        
        if(flashCounter < 10) setcolor(GREEN);
        else setcolor(LIGHTGREEN);
        
        for(int i = radius; i > 0; i--) {
            circle(x, y, i);
        }
        
        // Outer glow ring for extra spice
        if(flashCounter < 10) {
            setcolor(LIGHTGREEN);
            circle(x, y, radius+3);
            circle(x, y, radius+2);
        }
        
        // Thick plus sign
        setcolor(WHITE);
        line(x-10, y, x+10, y);
        line(x-10, y-1, x+10, y-1);
        line(x-10, y+1, x+10, y+1);
        line(x, y-10, x, y+10);
        line(x-1, y-10, x-1, y+10);
        line(x+1, y-10, x+1, y+10);
        
        // Shiny highlight
        for(int i = radius/4; i > 0; i--) {
            circle(x-radius/3, y-radius/3, i);
        }
    }
};

int BonusTarget::flashCounter = 0;

// Bomb Target - dangerous! Explodes and causes area damage
class BombTarget : public Target {
private:
    static int pulseCounter;
public:
    BombTarget(int x1, int y1, int level) 
        : Target(x1, y1, 2 + level/3, -30, 20) {  // Negative points = penalty
        color = RED;  // RED BOMB!
    }
    
    void draw() {
        // Animated pulsing RED bomb - filled solid ball
        pulseCounter = (pulseCounter + 1) % 30;
        
        int pulseSize = (pulseCounter < 15) ? 0 : 3;
        
        // Draw filled red bomb
        setcolor(RED);
        for(int i = radius + pulseSize; i > 0; i--) {
            circle(x, y, i);
        }
        
        // Danger glow ring - pulses
        if(pulseCounter < 15) {
            setcolor(YELLOW);
            circle(x, y, radius + pulseSize + 2);
            circle(x, y, radius + pulseSize + 3);
        }
        
        // Sparking fuse on top - animated
        if(pulseCounter % 10 < 5) {
            setcolor(YELLOW);
        } else {
            setcolor(WHITE);
        }
        line(x, y-radius, x, y-radius-8);
        // Spark effect
        for(int i = 4; i > 0; i--) {
            circle(x, y-radius-10, i);
        }
        line(x-3, y-radius-10, x+3, y-radius-10);
        line(x, y-radius-13, x, y-radius-7);
        
        // Skull symbol (danger!)
        setcolor(YELLOW);
        circle(x-3, y-2, 2);
        circle(x+3, y-2, 2);
        line(x-4, y+3, x-2, y+5);
        line(x-2, y+5, x+2, y+5);
        line(x+2, y+5, x+4, y+3);
        
        // Dark highlight for 3D effect
        setcolor(LIGHTRED);
        for(int i = radius/4; i > 0; i--) {
            circle(x-radius/3, y-radius/3, i);
        }
    }
    
    bool isBomb() { return true; }
};

int BombTarget::pulseCounter = 0;

// Explosion effect class
class Explosion : public GameObject {
    int radius, maxRadius;
    bool isBombExplosion;
    int frame;
public:
    Explosion(int x1, int y1, bool bomb = false) 
        : GameObject(x1, y1), radius(5), isBombExplosion(bomb), frame(0) {
        maxRadius = bomb ? 60 : 35;  // Even bigger explosion for bombs!
    }
    
    void draw() {
        if(isBombExplosion) {
            // MASSIVE dramatic explosion with shockwave!
            setcolor(RED);
            for(int i = 0; i < 3; i++) {
                circle(x, y, radius + i);
            }
            
            setcolor(YELLOW);
            for(int i = 0; i < 3; i++) {
                circle(x, y, radius - 8 + i);
            }
            
            setcolor(WHITE);
            if(radius > 15) {
                for(int i = 0; i < 2; i++) {
                    circle(x, y, radius - 16 + i);
                }
            }
            
            // Explosion rays for extra spice!
            if(frame % 3 == 0) {
                setcolor(YELLOW);
                for(int angle = 0; angle < 360; angle += 45) {
                    int dx = (int)(radius * 0.7 * (angle % 90 == 0 ? 1 : 0.7));
                    line(x, y, x + dx, y + dx);
                }
            }
        } else {
            // Regular explosion - filled
            setcolor(YELLOW);
            for(int i = radius; i > radius - 3; i--) {
                circle(x, y, i);
            }
            if(radius > 3) {
                setcolor(RED);
                for(int i = radius - 5; i > radius - 8 && i > 0; i--) {
                    circle(x, y, i);
                }
            }
        }
    }
    
    void update() {
        radius += isBombExplosion ? 6 : 4;
        frame++;
        if(radius >= maxRadius) active = false;
    }
};

// File handler for high score
class ScoreManager {
    const char* filename;
public:
    ScoreManager() : filename("highscore.dat") {}
    
    int loadHighScore() {
        ifstream file(filename);
        int score = 0;
        if(file.is_open()) {
            file >> score;
            file.close();
        }
        return score;
    }
    
    void saveHighScore(int score) {
        ofstream file(filename);
        if(file.is_open()) {
            file << score;
            file.close();
        }
    }
};

// Main Game class with enhanced features
class Game {
    Gun gun;
    Bullet* bullets[30];
    Target* targets[8];
    Explosion* explosions[10];
    int score, bulletsLeft, level, highScore;
    bool paused, gameOver;
    ScoreManager scoreManager;
    int frameCount;
    
public:
    Game() : gun(320, 450), score(0), bulletsLeft(20), level(1), 
             paused(false), gameOver(false), frameCount(0) {
        for(int i = 0; i < 30; i++) bullets[i] = NULL;
        for(int i = 0; i < 8; i++) targets[i] = NULL;
        for(int i = 0; i < 10; i++) explosions[i] = NULL;
        
        highScore = scoreManager.loadHighScore();
        spawnTargets();
    }
    
    ~Game() {
        for(int i = 0; i < 30; i++) if(bullets[i]) delete bullets[i];
        for(int i = 0; i < 8; i++) if(targets[i]) delete targets[i];
        for(int i = 0; i < 10; i++) if(explosions[i]) delete explosions[i];
        
        if(score > highScore) {
            scoreManager.saveHighScore(score);
        }
    }
    
    void spawnTargets() {
        // Clear old targets
        for(int i = 0; i < 8; i++) {
            if(targets[i]) delete targets[i];
            targets[i] = NULL;
        }
        
        // Spawn regular targets with higher speed even at level 1
        for(int i = 0; i < 3; i++) {
            targets[i] = new Target(80 + rand() % 480, 60 + i*70, 
                                   5 + level, 10, 15);  // Higher base speed: 5 + level
        }
        
        // Spawn fast targets (polymorphism)
        if(level >= 2) {
            targets[3] = new FastTarget(100 + rand() % 400, 150, level);
        }
        
        // Spawn bonus target
        if(level >= 2 && rand() % 3 == 0) {
            targets[4] = new BonusTarget(200 + rand() % 200, 100);
        }
        
        // Spawn bomb targets from level 3 - DANGEROUS!
        if(level >= 3) {
            int numBombs = 1 + (level - 3) / 2;  // More bombs at higher levels
            if(numBombs > 3) numBombs = 3;
            
            for(int i = 0; i < numBombs; i++) {
                targets[5 + i] = new BombTarget(100 + rand() % 400, 80 + rand() % 150, level);
            }
        }
    }
    
    void addExplosion(int x, int y, bool isBomb = false) {
        for(int i = 0; i < 10; i++) {
            if(!explosions[i] || !explosions[i]->isActive()) {
                if(explosions[i]) delete explosions[i];
                explosions[i] = new Explosion(x, y, isBomb);
                return;
            }
        }
    }
    
    void shoot() {
        if(bulletsLeft <= 0 || gameOver) return;
        for(int i = 0; i < 30; i++) {
            if(!bullets[i] || !bullets[i]->isActive()) {
                if(bullets[i]) delete bullets[i];
                bullets[i] = new Bullet(gun.getX(), gun.getY()-30);
                bulletsLeft--;
                return;
            }
        }
    }
    
    void update() {
        if(paused || gameOver) return;
        
        frameCount++;
        
        // Update bullets
        for(int i = 0; i < 30; i++)
            if(bullets[i] && bullets[i]->isActive())
                bullets[i]->update();
        
        // Update explosions
        for(int i = 0; i < 10; i++)
            if(explosions[i] && explosions[i]->isActive())
                explosions[i]->update();
        
        // Update targets and check hits
        int activeTargets = 0;
        for(int i = 0; i < 8; i++) {
            if(targets[i] && targets[i]->isActive()) {
                targets[i]->update();
                activeTargets++;
                
                for(int j = 0; j < 30; j++) {
                    if(bullets[j] && targets[i]->hit(bullets[j])) {
                        // Check if it's a bomb - casting to check type
                        BombTarget* bomb = dynamic_cast<BombTarget*>(targets[i]);
                        if(bomb) {
                            // Bomb explodes! Big explosion and lose life
                            addExplosion(targets[i]->getX(), targets[i]->getY(), true);
                            gun.loseLife();
                            score += targets[i]->getPoints();  // Negative score
                        } else {
                            // Regular target hit
                            score += targets[i]->getPoints();
                            addExplosion(targets[i]->getX(), targets[i]->getY(), false);
                            
                            // Check if it's a bonus target
                            BonusTarget* bonus = dynamic_cast<BonusTarget*>(targets[i]);
                            if(bonus) {
                                gun.addLife();
                            }
                        }
                    }
                }
            }
        }
        
        // Next level
        if(activeTargets == 0 && gun.getLives() > 0) {
            level++;
            bulletsLeft += 15;
            spawnTargets();
        }
        
        // Game over
        if(gun.getLives() <= 0 || (bulletsLeft <= 0 && activeTargets > 0)) {
            gameOver = true;
        }
    }
    
    void drawBackground() {
        // Draw border
        setcolor(CYAN);
        rectangle(10, 10, 630, 470);
        rectangle(11, 11, 629, 469);
        
        // Draw stars
        setcolor(WHITE);
        for(int i = 0; i < 30; i++) {
            int sx = 20 + (i * 37) % 600;
            int sy = 20 + (i * 43) % 400;
            putpixel(sx, sy, WHITE);
        }
    }
    
    void drawHUD() {
        char text[50];
        
        // Score and stats
        setcolor(WHITE);
        sprintf(text, "Score: %d", score);
        outtextxy(20, 20, text);
        
        sprintf(text, "Level: %d", level);
        outtextxy(20, 35, text);
        
        sprintf(text, "Bullets: %d", bulletsLeft);
        outtextxy(520, 20, text);
        
        sprintf(text, "High: %d", highScore);
        outtextxy(520, 35, text);
        
        // Lives display (hearts)
        setcolor(RED);
        for(int i = 0; i < gun.getLives(); i++) {
            circle(250 + i*25, 25, 8);
            circle(250 + i*25, 25, 6);
        }
        
        // Pause indicator
        if(paused) {
            setcolor(YELLOW);
            outtextxy(280, 220, (char*)"PAUSED");
            outtextxy(230, 250, (char*)"Press P to resume");
        }
    }
    
    void draw() {
        cleardevice();
        
        drawBackground();
        drawHUD();
        
        // Draw game objects
        gun.draw();
        
        for(int i = 0; i < 30; i++)
            if(bullets[i] && bullets[i]->isActive())
                bullets[i]->draw();
        
        for(int i = 0; i < 8; i++)
            if(targets[i] && targets[i]->isActive())
                targets[i]->draw();
        
        for(int i = 0; i < 10; i++)
            if(explosions[i] && explosions[i]->isActive())
                explosions[i]->draw();
        
        // Game over screen
        if(gameOver) {
            char text[50];
            setcolor(RED);
            outtextxy(220, 200, (char*)"GAME OVER!");
            
            setcolor(YELLOW);
            sprintf(text, "Final Score: %d", score);
            outtextxy(250, 240, text);
            
            if(score >= highScore) {
                setcolor(GREEN);
                outtextxy(230, 260, (char*)"NEW HIGH SCORE!");
            }
            
            setcolor(WHITE);
            outtextxy(210, 290, (char*)"Press Q to exit");
        }
    }
    
    void processKeys(char key) {
        // Handle special keys (arrow keys send 2 bytes)
        if(key == 27) {  // ESC key
            gameOver = true;
            return;
        }
        
        if(key == 'p' || key == 'P') {
            paused = !paused;
            return;
        }
        if(paused) return;
        
        // Arrow keys or WASD
        if(key == 'a' || key == 'A' || key == 75 || key == 68) {  // Left arrow or A
            gun.moveLeft();
        }
        else if(key == 'd' || key == 'D' || key == 77 || key == 67) {  // Right arrow or D
            gun.moveRight();
        }
        else if(key == ' ') {
            shoot();
        }
        else if(key == 'q' || key == 'Q') {
            gameOver = true;
        }
    }
    
    bool isRunning() { return !gameOver; }
};

int main() {
    int gd = DETECT, gm;
    initgraph(&gd, &gm, (char*)"");
    srand(time(0));
    
    // Instructions screen
    cleardevice();
    setcolor(CYAN);
    outtextxy(200, 100, (char*)"TARGET SHOOTER");
    
    setcolor(WHITE);
    outtextxy(220, 150, (char*)"CONTROLS:");
    outtextxy(180, 180, (char*)"A/D or Arrow Keys - Move Gun");
    outtextxy(180, 200, (char*)"Space - Shoot");
    outtextxy(180, 220, (char*)"P - Pause/Resume");
    outtextxy(180, 240, (char*)"Q or ESC - Quit");
    
    setcolor(YELLOW);
    outtextxy(220, 280, (char*)"OBJECTIVES:");
    outtextxy(180, 300, (char*)"Red Targets - 10 pts");
    outtextxy(180, 320, (char*)"Fast Targets - 20 pts");
    outtextxy(180, 340, (char*)"Green Bonus - 50 pts + Life");
    
    setcolor(RED);
    outtextxy(180, 360, (char*)"RED BOMBS - Avoid! (-30pts + LOSE LIFE)");
    
    setcolor(GREEN);
    outtextxy(200, 400, (char*)"Press ENTER to start...");
    
    // Wait for enter key
    char ch;
    do {
        usleep(10000);
        if(kbhit()) {
            ch = getchar();
            if(ch == '\n') break;
        }
    } while(1);
    
    Game game;
    
    // Main game loop
    while(game.isRunning()) {
        // Handle input - check for both regular keys and arrow keys
        if(kbhit()) {
            char key = getchar();
            
            // Handle arrow keys (they send escape sequences)
            if(key == 27) {  // ESC or arrow key start
                if(kbhit()) {
                    char next = getchar();
                    if(next == '[' && kbhit()) {  // Arrow key sequence
                        char arrow = getchar();
                        if(arrow == 'D') key = 'a';      // Left arrow
                        else if(arrow == 'C') key = 'd';  // Right arrow
                    } else {
                        key = 27;  // Just ESC
                    }
                }
            }
            
            game.processKeys(key);
        }
        
        game.update();
        game.draw();
        usleep(40000);  // ~25 FPS
    }
    
    game.draw();  // Show final screen
    
    // Wait for quit
    while(1) {
        if(kbhit()) {
            char key = getchar();
            if(key == 'q' || key == 'Q') break;
        }
        usleep(100000);
    }
    
    closegraph();
    return 0;
}
