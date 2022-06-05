#define LATCH 2
#define CLOCK 3
#define DATA 4
#define RESET_BTN 5
#define UP 6
#define DOWN 7
#define LEFT 8
#define RIGHT 9
#define PUT 10
#define LOOP(name, n) for (byte name = 0; name < n; name++)
#define SHOW(STONE)                                             \
    LOOP(i, 8)                                                  \
    {                                                           \
        byte b = B00000000;                                     \
                                                                \
        digitalWrite(LATCH, LOW);                               \
        shiftOut(DATA, CLOCK, MSBFIRST, ~(B00000001 << i));     \
                                                                \
        LOOP(j, 8)                                              \
        b |= board[i][j] == STONE ? B10000000 >> j : B00000000; \
                                                                \
        shiftOut(DATA, CLOCK, MSBFIRST, b);                     \
        digitalWrite(LATCH, HIGH);                              \
    }
#define PASS(NAME) \
    if (NAME)      \
        return;    \
    else           \
        Serial.println("Pressed");

enum Stone
{
    BLACK,
    WHITE,
    EMPTY,
};

Stone board[8][8];
Stone player = BLACK;
short x = 0, y = 0;
bool _reset = false,
     _up = false,
     _down = false,
     _left = false,
     _right = false,
     _put = false;

void reset()
{
    LOOP(i, 8)
    LOOP(j, 8)
    board[i][j] = EMPTY;
}

void move(byte a, byte b)
{
    x = constrain(a, 0, 7);
    y = constrain(b, 0, 7);

    Serial.print("X,Y: ");
    Serial.print(x);
    Serial.print(",");
    Serial.println(y);
}

bool check(Stone stone)
{
    static const short DX[] = {0, 1, 1, 1},
                       DY[] = {1, 0, 1, -1};

    LOOP(i, 8)
    LOOP(j, 8)
    {
        if (board[i][j] == stone)
            LOOP(k, 4)
            {
                byte nx = i,
                     ny = j;
                byte cnt = 0;

                while (0 <= nx && nx < 8 && 0 <= ny && ny < 8 && board[nx][ny] == stone)
                {
                    nx += DX[k];
                    ny += DY[k];
                    cnt++;
                }

                if (cnt >= 5)
                    return true;
            }
    }

    return false;
}

void push(int pin)
{
    int result = digitalRead(pin);

    if (result == LOW)
        switch (pin)
        {
        case RESET_BTN:
            PASS(_reset)
            reset();
            _reset = true;
            break;
        case UP:
            PASS(_up)
            move(x, y + 1);
            _up = true;
            break;
        case DOWN:
            PASS(_down)
            move(x, y - 1);
            _down = true;
            break;
        case LEFT:
            PASS(_left)
            move(x - 1, y);
            _left = true;
            break;
        case RIGHT:
            PASS(_right)
            move(x + 1, y);
            _right = true;
            break;
        case PUT:
            PASS(_put)
            if (board[x][y] == EMPTY)
            {
                board[x][y] = player;
                player = (player == BLACK) ? WHITE : BLACK;

                Serial.print("Put at ");
                Serial.print(x);
                Serial.print(",");
                Serial.print(y);
                Serial.print(" Now Player: ");
                Serial.println(player == BLACK ? "BLACK" : "WHITE");
            }
            else
            {
                Serial.println("Not Empty");
            }
            _put = true;
            break;
        }
    else
        switch (pin)
        {
        case RESET_BTN:
            _reset = false;
            break;
        case UP:
            _up = false;
            break;
        case DOWN:
            _down = false;
            break;
        case LEFT:
            _left = false;
            break;
        case RIGHT:
            _right = false;
            break;
        case PUT:
            _put = false;
            break;
        }
}

void setup()
{
    reset();
    pinMode(LATCH, OUTPUT);
    pinMode(CLOCK, OUTPUT);
    pinMode(DATA, OUTPUT);
    pinMode(RESET_BTN, INPUT_PULLUP);
    pinMode(UP, INPUT_PULLUP);
    pinMode(DOWN, INPUT_PULLUP);
    pinMode(LEFT, INPUT_PULLUP);
    pinMode(RIGHT, INPUT_PULLUP);
    Serial.begin(9600);
}

void loop()
{
    push(RESET_BTN);
    push(UP);
    push(DOWN);
    push(LEFT);
    push(RIGHT);
    push(PUT);

    if (check(WHITE))
    {
        Serial.println("White win!");
        reset();
    }
    if (check(BLACK))
    {
        Serial.println("Black win!");
        reset();
    }

    SHOW(WHITE)
    delay(20);
    SHOW(EMPTY)
}