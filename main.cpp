#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>


#include <string>
#include <ctime>
#include <cstdlib>
#include <iostream>

using namespace std;
using namespace sf;

unsigned short score = 0;
unsigned short highScore = 0;
unsigned short grid[4][4] = { 0 };

Font font;
Texture backgroundTexture;
Sprite backgroundSprite;
SoundBuffer moveSoundBuffer;
Sound moveSound;
Music backgroundMusic;
Color textColour(139, 0, 139, 255); // Dark magenta text color
Color darkPink(231, 84, 128, 255); // Dark pink color

float padding = 10.f;
float gridSize = (400.f - (padding * 2.f));/*Grid ka size calculate karne ke liye, jo padding ko consider karta hai.•	Total window size 400 pixels hai.
•	Padding dono sides se minus ki gayi hai(10 pixels each side).
•	Isliye, gridSize = 400 - (10 * 2) = 380 pixels.*/
float tileSize = (gridSize - (5 * padding)) / 4;
/*Grid size 380 pixels hai.
•	5 padding gaps hain(4 tiles ke beech mein 5 gaps hote hain).
•	Total padding gaps ka size 5 * 10 = 50 pixels hai.
•	Remaining size 380 - 50 = 330 pixels hai.
•	4 tiles hain, to har tile ka size 330 / 4 = 82.5 pixels hai.*/

RenderWindow window(VideoMode(400, 600), "3072", Style::Close | Style::Titlebar);
RectangleShape gridBackgroundRect(Vector2f(gridSize, gridSize));
RectangleShape gameOverBackground(Vector2f(gridSize, gridSize));
RectangleShape gridTile(Vector2f(tileSize, tileSize));
RectangleShape button(Vector2f(150.f, 50.f));
Text titleText("3072!", font, 48);
Text gameOverText("Game Over!", font, 50);
Text scoreText(to_string(score), font, 48);
Text scoreDecoratorText("", font, 25);
Text tileText;
Text buttonText;

bool showGameOver = false;
bool gameStarted = false;

// Function Prototypes

void renderScreen();
void reset();
void addNumber();

unsigned char moveUp(), moveDown(), moveLeft(), moveRight();
void combine(vector<int>&);
bool isGameOver();
unsigned short getMovesLeft();
void drawButton(const string& text, float x, float y, float width, float height, bool isHovered = false, bool isDark = false);
void addAnimations();

void renderScreen() {
    window.clear();
    window.draw(backgroundSprite);
    Vector2i mousePos = Mouse::getPosition(window);

    if (!gameStarted) {
        drawButton("Start Game", 125, 250, 150, 50, false, true);
        window.display();
       
    }

    window.draw(gridBackgroundRect);

    // Render the grid....Har tile ko screen par display karne ke liye.
    for (int x = 0; x < 4; x++) {
        for (int y = 0; y < 4; y++) {
            gridTile.setPosition(Vector2f(20 + (x * (tileSize + padding)), 120 + (y * (tileSize + padding))));
            gridTile.setFillColor(numberColours[grid[x][y]]);
            window.draw(gridTile);

            if (grid[x][y] > 0) {
                tileText.setString(to_string(grid[x][y]));
                Vector2f textPosition = gridTile.getPosition() + (gridTile.getSize() / 2.f);
                textPosition.x -= tileText.getGlobalBounds().width / 2.f;
                textPosition.y -= tileText.getGlobalBounds().height / 1.5f;
                tileText.setPosition(textPosition);
                window.draw(tileText);
            }
        }
    }

    // Render the score,score ki decoration ky liye
    scoreText.setString(to_string(score));
    scoreText.setPosition(Vector2f(380 - scoreText.getGlobalBounds().width, 25));
    scoreText.setFillColor(Color(138, 43, 226, 255));
    window.draw(scoreText);

    scoreDecoratorText.setString("score:");
    scoreDecoratorText.setPosition(Vector2f(scoreText.getPosition().x - scoreDecoratorText.getGlobalBounds().width - 10, 25));
    scoreDecoratorText.setFillColor(Color(138, 43, 226, 255));
    window.draw(scoreDecoratorText);

    scoreDecoratorText.setString("highscore: " + to_string(highScore));
    scoreDecoratorText.setPosition(Vector2f(20, 72));
    scoreDecoratorText.setFillColor(Color(138, 43, 226, 255));
    window.draw(scoreDecoratorText);

    drawButton("New Game", 20, 530, 150, 50, false, true);
    drawButton("End Game", 230, 530, 150, 50, false, true);

    // Render Game Over,game over banany ka function
    if (showGameOver) {
        window.draw(gameOverBackground);
        window.draw(gameOverText);

        scoreDecoratorText.setString("Hit Enter to Restart");
        scoreDecoratorText.setPosition(Vector2f(60, 325));
        window.draw(scoreDecoratorText);
    }

    window.display();
}

void drawButton(const string& text, float x, float y, float width, float height, bool isHovered, bool isDark) {
    // Create a button shape with rounded corner effect (simulate with outline and mask)
    RectangleShape button(Vector2f(width, height));
    button.setPosition(Vector2f(x, y));

    // Gradient-like effect (simulate with two layers)
    if (isHovered) {
        button.setFillColor(isDark ? Color(140, 70, 60, 255) : Color(140, 70, 60, 255)); // Dark pink or light pink gradient-like effect
    }
    else {
        button.setFillColor(isDark ? Color(138, 43, 226, 255) : Color(138, 43, 226, 255));// Dark magenta or hot pink base color
    }

    // Add a smooth border,thickness or uss mien uss ka color
    button.setOutlineThickness(4.f);
    button.setOutlineColor(isDark ? Color(0, 0, 0, 255) : Color(0, 0, 0, 255)); // Dark magenta or medium violet red

    // Draw subtle shadow for depth
    RectangleShape shadow(Vector2f(width + 5.f, height + 5.f));
    shadow.setPosition(Vector2f(x + 3, y + 3));
    shadow.setFillColor(Color(0, 0, 0, 50)); // Semi-transparent shadow color
    window.draw(shadow);

    // Draw the button shape,rectangle shape mein dalny ky liye
    window.draw(button);

    // Setup and style the text
    Text buttonText;
    buttonText.setString(text);
    buttonText.setCharacterSize(22);
    buttonText.setFont(font);
    buttonText.setStyle(Text::Bold);
    buttonText.setFillColor(isHovered ? Color(230, 230, 250, 255) : Color(230, 230, 250, 255)); // Deep pink or pale violet red

    // Center the text in the button,taky woh center par rhy
    FloatRect textBounds = buttonText.getLocalBounds();
    buttonText.setPosition(
        x + (width / 2.f) - (textBounds.width / 2.f),
        y + (height / 2.f) - (textBounds.height / 1.5f)
    );

    // Optional: Add a glossy overlay for a modern look
    RectangleShape glossyOverlay(Vector2f(width, height / 2.f));
    glossyOverlay.setPosition(Vector2f(x, y));
    glossyOverlay.setFillColor(Color(255, 182, 193, 100)); // Semi-transparent white
    glossyOverlay.setOutlineThickness(0);

    // Draw text and overlay
    window.draw(buttonText);
    window.draw(glossyOverlay);
}

void reset() {
    if (score > highScore) highScore = score;
    score = 0;
    memset(grid, 0, sizeof(grid));
    showGameOver = false;
    addNumber();
    addNumber();
}

int getEmptyCells(Vector2i emptyCells[], int maxCells) {
    int count = 0;
    for (int x = 0; x < 4; x++) {
        for (int y = 0; y < 4; y++) {
            if (grid[x][y] == 0 && count < maxCells) {
                emptyCells[count++] = Vector2i(x, y);
            }
        }
    }
    return count;
}

void addNumber() {
    Vector2i emptyCells[16];
    int count = getEmptyCells(emptyCells, 16);
    if (count > 0) {
        int index = rand() % count;
        grid[emptyCells[index].x][emptyCells[index].y] = (rand() % 10 == 0) ? 6 : 3;
    }
}


void combine(int line[], int& size) {
    for (int i = 0; i + 1 < size; ++i) {
        if (line[i] == line[i + 1] && line[i] % 3 == 0) {
            line[i] *= 2;
            score += line[i];
            for (int j = i + 1; j < size - 1; ++j) {
                line[j] = line[j + 1];
            }
            --size;
        }
    }
}


unsigned char moveUp() {
    unsigned char moves = 0;
    for (int x = 0; x < 4; x++) {
        int line[4];
        int size = 0;
        for (int y = 0; y < 4; y++) {
            if (grid[x][y] != 0) line[size++] = grid[x][y];
        }
        combine(line, size);
        for (int y = 0; y < 4; y++) {
            int value = (y < size ? line[y] : 0);
            if (grid[x][y] != value) moves++;
            grid[x][y] = value;
        }
    }
    if (moves > 0) moveSound.play();
    return moves;
}
unsigned char moveDown() {
    unsigned char moves = 0;
    for (int x = 0; x < 4; x++) {
        int line[4];
        int size = 0;
        for (int y = 3; y >= 0; y--) {
            if (grid[x][y] != 0) line[size++] = grid[x][y];
        }
        combine(line, size);
        for (int y = 3; y >= 0; y--) {
            int value = (3 - y < size ? line[3 - y] : 0);
            if (grid[x][y] != value) moves++;
            grid[x][y] = value;
        }
    }
    if (moves > 0) moveSound.play();
    return moves;
}

unsigned char moveLeft() {
    unsigned char moves = 0;
    for (int y = 0; y < 4; y++) {
        int line[4];
        int size = 0;
        for (int x = 0; x < 4; x++) {
            if (grid[x][y] != 0) line[size++] = grid[x][y];
        }
        combine(line, size);
        for (int x = 0; x < 4; x++) {
            int value = (x < size ? line[x] : 0);
            if (grid[x][y] != value) moves++;
            grid[x][y] = value;
        }
    }
    if (moves > 0) moveSound.play();
    return moves;
}


unsigned char moveRight() {
    unsigned char moves = 0;
    for (int y = 0; y < 4; y++) {
        int line[4];
        int size = 0;
        for (int x = 3; x >= 0; x--) {
            if (grid[x][y] != 0) line[size++] = grid[x][y];
        }
        combine(line, size);
        for (int x = 3; x >= 0; x--) {
            int value = (3 - x < size ? line[3 - x] : 0);
            if (grid[x][y] != value) moves++;
            grid[x][y] = value;
        }
    }
    if (moves > 0) moveSound.play();
    return moves;
}
bool isGameOver() {
    Vector2i emptyCells[16];
    return getEmptyCells(emptyCells, 16) == 0 && getMovesLeft() == 0;
}


unsigned short getMovesLeft() {
    unsigned short moves = 0;
    for (int x = 0; x < 4; x++) {
        for (int y = 0; y < 4; y++) {
            if (grid[x][y] == 0) continue;
            if (x < 3 && grid[x][y] == grid[x + 1][y]) moves++;
            if (y < 3 && grid[x][y] == grid[x][y + 1]) moves++;
        }
    }
    return moves;
}


int main() {
    srand(static_cast<unsigned int>(time(nullptr)));
    /*
    Ye line random number generator ko seed karne ke liye use hoti hai. 
    Iska matlab hai ki ye random number generator ko initialize karti hai 
    taake har baar program run hone par different random numbers generate ho sakein.*/

    if (!font.loadFromFile("Brown Cake.ttf")) {
        cerr << "Failed to load font\n";
        return 1;
    }

    if (!backgroundTexture.loadFromFile("C:\\Users\\kum41l\\Desktop\\GameSetup\\Project5\\hey03.jpg")) {
        cerr << "Failed to load background image\n";
        return 1;
    }
    backgroundSprite.setTexture(backgroundTexture);

    if (!moveSoundBuffer.loadFromFile("C:\\Users\\kum41l\\Desktop\\GameSetup\\Project5\\sound.wav")) {
        cerr << "Failed to load move sound\n";
        return 1;
    }
    moveSound.setBuffer(moveSoundBuffer);

    if (!backgroundMusic.openFromFile("C:\\Users\\kum41l\\Desktop\\GameSetup\\Project5\\backsound.mp3")) {
        cerr << "Failed to load background music\n";
        return 1;
    }
    backgroundMusic.setVolume(20); // Set volume to a low level
    backgroundMusic.setLoop(true); // Loop the music
    backgroundMusic.play(); // Play the music

    // Initialize colors,color theme pink hain
    numberColours[0] = Color(255, 240, 245, 255); // Light pink
    numberColours[3] = Color(255, 182, 193, 255); // Light pink
    numberColours[6] = Color(255, 105, 180, 255); // Hot pink
    numberColours[12] = Color(255, 20, 147, 255); // Deep pink
    numberColours[24] = Color(219, 112, 147, 255); // Pale violet red
    numberColours[48] = Color(199, 21, 133, 255); // Medium violet red
    numberColours[96] = Color(255, 0, 255, 255); // Magenta
    numberColours[192] = Color(238, 130, 238, 255); // Violet
    numberColours[384] = Color(221, 160, 221, 255); // Plum
    numberColours[768] = Color(218, 112, 214, 255); // Orchid
    numberColours[1536] = Color(186, 85, 211, 255); // Medium orchid
    numberColours[3072] = Color(153, 50, 204, 255); // Dark orchid

    gridBackgroundRect.setFillColor(Color(255, 182, 193, 200)); // Light pink with some transparency
    gridBackgroundRect.setPosition(Vector2f(10, 110));

    titleText.setFillColor(textColour);
    titleText.setStyle(Text::Bold);
    titleText.setPosition(Vector2f(10, 25));
    Text scoreText(to_string(score), font, 50);
    scoreText.setFillColor(darkPink);
    scoreText.setStyle(Text::Bold);

    tileText.setFont(font);
    tileText.setCharacterSize(32);
    tileText.setFillColor(textColour);

    gameOverBackground.setFillColor(Color(255, 240, 245, 175)); // Light pink with transparency
    gameOverBackground.setPosition(Vector2f(10, 110));

    gameOverText.setFillColor(textColour);
    gameOverText.setStyle(Text::Bold);
    gameOverText.setPosition(Vector2f(70, 225));

    while (window.isOpen()) {
        Event event;
        while (window.pollEvent(event)) {
            if (event.type == Event::Closed) window.close();

            if (!gameStarted && event.type == Event::MouseButtonPressed && event.mouseButton.button == Mouse::Left)//check karny ky liye ky gamestart na hua ho or  mouse button press event ho.
            {
                Vector2i mousePos = Mouse::getPosition(window);//check karny ky liye ky muse window par ho
                if (mousePos.x >= 125 && mousePos.x <= 275 && mousePos.y >= 250 && mousePos.y <= 300)//click start button par hua hai check karta hain
                {
                    gameStarted = true;
                    reset();
                }
            }

            if (gameStarted && event.type == Event::MouseButtonPressed && event.mouseButton.button == Mouse::Left) {
                Vector2i mousePos = Mouse::getPosition(window);
                if (mousePos.x >= 20 && mousePos.x <= 170 && mousePos.y >= 530 && mousePos.y <= 580) {
                    reset();
                }
                else if (mousePos.x >= 230 && mousePos.x <= 380 && mousePos.y >= 530 && mousePos.y <= 580) {
                    gameStarted = false;
                }
            }

            if (gameStarted && !showGameOver && event.type == Event::KeyPressed) {
                unsigned char moves = 0;
                if (event.key.code == Keyboard::Up) moves += moveUp();
                else if (event.key.code == Keyboard::Down) moves += moveDown();
                else if (event.key.code == Keyboard::Left) moves += moveLeft();
                else if (event.key.code == Keyboard::Right) moves += moveRight();

                if (moves > 0) addNumber();
                showGameOver = isGameOver();
            }
        }

        renderScreen();
    }

    return 0;
}

