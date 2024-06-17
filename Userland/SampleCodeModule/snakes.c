// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <inout.h>
#include <syscalls.h>
#include <snakes.h>
#include <colors.h>

#define SIZE 8
#define MAX_LENGTH 35
#define FREQUENCY 400
#define NANO_SECS 30000000

typedef struct
{

    uint64_t posX;
    uint64_t posY;

} TCoordinates;

typedef struct
{

    uint8_t direction;
    uint8_t state;
    uint64_t color;
    uint64_t length;
    uint64_t points;

    TCoordinates head;
    TCoordinates body[MAX_LENGTH];

} Player;

Player player1;
Player player2;

TCoordinates food;

// Función que obtiene los valores de la pantalla
static void getScreenData(uint16_t *data)
{
    return sys_getScreenData(data);
}

// Función que verifica si la food se generó dentro de alguna snake
static int checkInSnake()
{

    for (int i = 0, j = 0; i < player1.length || j < player2.length; i++, j++)
    {
        if (i < player1.length)
        {
            if (player1.body[i].posX == food.posX && player1.body[i].posY == food.posY)
                return 1;
        }
        if (i < player2.length)
        {
            if (player2.body[i].posX == food.posX && player2.body[i].posY == food.posY)
                return 1;
        }
    }
    return 0;
}

// Función que actualiza el puntaje de cada jugador
void updatePoints(char playersChoice)
{

    // Limpia la pantalla para actualizar los puntos
    do_clearScreen(BLACK);
    do_changeFontSize(2);
    printf("\n\t\t\t\t\t\t\t\t\t\t\t\t");
    do_printColor("Player 1: ", player1.color);
    printf("%d", (int)player1.points);
    if (playersChoice == '2')
    {
        printf("\t\t\t\t\t");
        do_printColor("Player 2: ", player2.color);
        printf("%d", (int)player2.points);
    }

    // Restablece el tamaño de fuente a su valor original
    do_changeFontSize(1);
}

// Función para generar una posición pseudoaleatoria
void generateRandomPosition(uint16_t width, uint16_t height, uint64_t *x, uint64_t *y)
{

    // Usa los ticks del sistema como una semilla pseudoaleatoria
    do
    {
        uint64_t seed = sys_getTicks();

        // Realiza operaciones matemáticas simples para obtener valores "aleatorios"
        *x = (seed % (width / SIZE));
        *y = (seed % (height / SIZE));

        if (*y <= 30)
        {
            *y += 30;
        }

        if (*x <= 10)
        {
            *x += 10;
        }
    } while (checkInSnake() || *y >= (height / SIZE) || *x >= (width / SIZE));

    return;
}

// Función para obtener un valor pseudoaleatorio para el color de la food 
static uint64_t randomColor(uint64_t dim)
{
    return sys_getTicks() % dim;
}

// Función para setear la food
void setFood(uint64_t width, uint64_t height)
{

    static uint64_t foodColor[] = {RED, BLUE, YELLOW, ORANGE, WHITE, CYAN, DARKERCYAN, GREY};
    generateRandomPosition(width, height, &food.posX, &food.posY);
    uint64_t dim = sizeof(foodColor) / sizeof(foodColor[0]);
    uint64_t colorIndex = randomColor(dim);
    do_paintRect((food.posX) * SIZE, (food.posY) * SIZE, SIZE, SIZE, foodColor[colorIndex]);
}

// Función para dibujar la snake
void drawSnake(Player player, uint64_t color)
{

    for (int i = 0; i < player.length; i++)
    {
        do_paintRect((player.body[i].posX) * SIZE, (player.body[i].posY) * SIZE, SIZE, SIZE, color);
    }
}

// Función que verifica si un jugador se encuentra por fuera de los bordes de la pantalla
static int outOfBorders(Player p, uint16_t width, uint16_t height)
{

    if (p.head.posX < 1 || p.head.posX >= (width + 1) || p.head.posY < 11 || p.head.posY >= (height + 1))
    {
        do_beep(FREQUENCY, NANO_SECS);
        return 1; // La serpiente ha chocado con el borde, por lo que está muerta.
    }
    return 0;
}

// Función que verifica si un jugador se chocó consigo mismo o con el otro jugador
static int checkCollision(Player player, Player other)
{
    if (!other.state)
    {
        return 0;
    }
    for (int i = 1; i < other.length; i++)
    {
        if (player.head.posX == other.body[i].posX && player.head.posY == other.body[i].posY)
        {
            do_beep(FREQUENCY, NANO_SECS);
            return 1;
        }
    }
    return 0;
}

// Función que verifica si el jugador tuvo algún tipo de colisión
static int collided(Player player, Player other)
{
    if (player1.state && player2.state && player1.head.posX == player2.head.posX && player1.head.posY == player2.head.posY){
        player1.state = 0;
        player2.state = 0;
        do_beep(FREQUENCY, NANO_SECS);
        return 1;
    }
    return checkCollision(player, player) || checkCollision(player, other);
}

// Función que agrega una nueva cola
TCoordinates addNewTail(Player *player)
{

    TCoordinates toReturn;
    toReturn.posX = player->body[player->length - 1].posX;
    toReturn.posY = player->body[player->length - 1].posY;

    switch (player->direction)
    {
    case 'w':
    case 'U':
        toReturn.posY += 1;
        break;
    case 'a':
    case 'L':
        toReturn.posX += 1;
        break;
    case 's':
    case 'D':
        toReturn.posY -= 1;
        break;
    case 'd':
    case 'R':
        toReturn.posX -= 1;
        break;
    }
    return toReturn;
}

// Función que incrementa el tamaño de la snake
void increaseSnake(Player *player, char playersChoice)
{

    if (player->length < MAX_LENGTH)
    {
        player->body[player->length] = addNewTail(player);
        player->length++;
        player->points++;
        updatePoints(playersChoice);
    }
}

// Función que verifica si un jugador comió alguna food
void ateFood(Player *player, uint16_t screen[2], char playersChoice)
{

    if (player->head.posX == food.posX && player->head.posY == food.posY)
    {
        do_beep(FREQUENCY, NANO_SECS);
        increaseSnake(player, playersChoice);
        setFood(screen[0], screen[1]);
    }
}

// Función que actualiza los movimientos de la snake
void updateSnake(Player *player, uint16_t screen[2], char playersChoice)
{

    TCoordinates aux;
    aux.posX = player->head.posX;
    aux.posY = player->head.posY;

    for (int i = player->length; i > 1; i--)
    {
        player->body[i - 1] = player->body[i - 2];
    }
    player->body[0] = aux;

    ateFood(player, screen, playersChoice);
}

// Función que actualiza la posición de los jugadores en función de las teclas presionadas
static void updatePosition(uint64_t keypressed, uint16_t screen[2], char playersChoice)
{

    drawSnake(player1, BLACK);
    drawSnake(player2, BLACK);

    switch (keypressed)
    {
    // casos para el jugador 1
    case 'w':
        if (player1.direction != 's')
        {
            player1.head.posY--;
            player1.direction = 'w';
            updateSnake(&player1, screen, playersChoice);
        }
        break;
    case 'a':
        if (player1.direction != 'd')
        {
            player1.head.posX--;
            player1.direction = 'a';
            updateSnake(&player1, screen, playersChoice);
        }
        break;
    case 's':
        if (player1.direction != 'w')
        {
            player1.head.posY++;
            player1.direction = 's';
            updateSnake(&player1, screen, playersChoice);
        }
        break;
    case 'd':
        if (player1.direction != 'a')
        {
            player1.head.posX++;
            player1.direction = 'd';
            updateSnake(&player1, screen, playersChoice);
        }
        break;

    // casos para el jugador 2
    case 'U':
        if (playersChoice == '2' && player2.direction != 'D')
        {
            player2.head.posY--;
            player2.direction = 'U';
            updateSnake(&player2, screen, playersChoice);
        }
        break;
    case 'L':
        if (playersChoice == '2' && player2.direction != 'R')
        {
            player2.head.posX--;
            player2.direction = 'L';
            updateSnake(&player2, screen, playersChoice);
        }
        break;
    case 'D':
        if (playersChoice == '2' && player2.direction != 'U')
        {
            player2.head.posY++;
            player2.direction = 'D';
            updateSnake(&player2, screen, playersChoice);
        }
        break;
    case 'R':
        if (playersChoice == '2' && player2.direction != 'L')
        {
            player2.head.posX++;
            player2.direction = 'R';
            updateSnake(&player2, screen, playersChoice);
        }
        break;
    }

    // Dibuja las serpientes después de actualizar su posición
    if (player1.state)
        drawSnake(player1, player1.color);

    if (playersChoice == '2' && player2.state)
        drawSnake(player2, player2.color);
}

// Función que verifica la cantidad de jugadores elegidos
static char get_choice()
{

    char c;

    do
    {

        c = do_getChar();

    } while (c != '1' && c != '2');

    return c;
}

// Función que verifica el estado de los jugadores y actualiza sus posiciones
static void check_state(char playersChoice, uint64_t width, uint64_t height, uint64_t speed, uint64_t lastTicks, uint64_t ticks, uint16_t screen[2])
{

    char c;

    while (player1.state || player2.state)
    {

        if (lastTicks != ticks && ticks % speed == 0)
        {

            c = do_getChar();

            if (player1.state || player2.state)
            {

                if (c != player1.direction && c != player2.direction)
                    updatePosition(c, screen, playersChoice);

                if (player1.state)
                {
                    updatePosition(player1.direction, screen, playersChoice);
                    player1.state = !outOfBorders(player1, width, height) && !collided(player1, player2);
                }
                else
                {
                    drawSnake(player1, BLACK);
                }

                if (player2.state)
                {
                    updatePosition(player2.direction, screen, playersChoice);
                    player2.state = !outOfBorders(player2, width, height) && !collided(player2, player1);
                }
                else
                {
                    drawSnake(player2, BLACK);
                }
            }

            lastTicks = ticks;
        }

        ticks = sys_getTicks();
    }

    return;
}

// Función que imprime al ganador (si es de a 2 jugadores) o el puntaje (si es de 1 jugador)
static void winner(char playersChoice)
{

    if (playersChoice == '2')
    {

        if (player1.points != player2.points)
        {

            printf("\n\n\n\n\n\n\n\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t   THE WINNER IS: \n");
            printf("\n\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t");

            do_changeFontSize(3);

            if (player1.points > player2.points)
            {
                do_printColor("PLAYER 1!", player1.color);
            }
            else
            {
                do_printColor("PLAYER 2!", player2.color);
            }
            do_changeFontSize(2);
        }
        else
            printf("\n\n\n\n\n\n\n\n\n\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t  IT'S A TIE!\n");
    }
    else
    {

        printf("\n\n\n\n\n\n\n\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t");
        do_printColor("Points: ", player1.color);
        printf("%d", (int)player1.points);
    }
}

// Funnción que verifica si se desea volver a jugar al snakes
static void restart()
{

    printf("\n\n\n\n\t\t\t\t\t\t\t\t\t\t\t\tWant to play again? Press y or n \n");

    do_changeFontSize(1);

    char d;

    do
    {

        d = do_getChar();

    } while (d != 'y' && d != 'n');

    if (d == 'y')
    {
        play_snakes();
    }

    else
    {
        do_clearScreen(BLACK);
    }

    return;
}

// Función que ejecuta el juego snakes
void play_snakes()
{

    uint16_t screen[2] = {0};
    getScreenData(screen); // width = 0, heigth = 1
    do_clearScreen(BLACK);
    do_changeFontSize(2);

    printf("\n\n\n\n\n\n\n\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t   Welcome to SNAKES!\n");
    printf("\n\t\t\t\t\t\t\t\t\t\t\t\t\t Choose the number of players:\n");
    printf("\n\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t Press 1 or 2.");

    char playersChoice = get_choice();

    do_clearScreen(BLACK);
    setPlayers(playersChoice, screen[0], screen[1]);
    setFood(screen[0], screen[1]);

    const uint16_t width = (screen[0] - 16) / 8;
    const uint16_t height = (screen[1] - 16) / 8;
    uint64_t speed = 1;
    uint64_t ticks = sys_getTicks();
    uint64_t lastTicks = 0;

    check_state(playersChoice, width, height, speed, lastTicks, ticks, screen);

    do_clearScreen(BLACK);
    do_changeFontSize(2);

    winner(playersChoice);
    restart();
}

// Función que setea a los jugadores
static void setSnake(Player * player, int n){

    for(int i = 0; i < player -> length; i++){

        player -> body[i].posX = player -> head.posX + n * i;
        player -> body[i].posY = player -> head.posY;
    }
}

// Función que inicializa a los jugadores
void setPlayers(char playersChoice, uint16_t width, uint16_t height){

    player1.head.posX = (width / 4) / SIZE;
    player1.head.posY = (height / 2) / SIZE;
    player1.direction = 'd';
    player1.color = PINK;
    player1.state = 1;
    player1.length = 15;
    player1.points = 0;

    setSnake(&player1, -1);

    if (playersChoice == '2'){

        player2.head.posX = (width - width / 4) / SIZE;
        player2.head.posY = (height / 2) / SIZE;
        player2.direction = 'L';
        player2.color = GREEN;
        player2.state = 1;
        player2.length = 15;
        player2.points = 0;

        setSnake(&player2, 1);
    }
}