#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define HOME_POSITION 100
#define BOARD_SIZE 52
#define NUM_PLAYERS 4
#define TOKEN_COUNT 4

const int START_POSITIONS[NUM_PLAYERS] = {0, 13, 26, 39};

typedef struct {
    int position;
    int inPlay; // 0 = not in play, 1 = in play
} Token;

typedef struct {
    Token tokens[TOKEN_COUNT];
    int playerIndex;
} Player;

typedef struct {
    int path[BOARD_SIZE];
} Board;

void initializeBoard(Board *board, int playerIndex) {
    for (int i = 0; i < BOARD_SIZE; i++) {
        board->path[i] = (START_POSITIONS[playerIndex] + i) % BOARD_SIZE;
    }
}

void initializePlayer(Player *player, int index) {
    player->playerIndex = index;
    for (int i = 0; i < TOKEN_COUNT; i++) {
        player->tokens[i].position = -1; // Not in play
        player->tokens[i].inPlay = 0; // Not in play
    }
}

void displayBoard(Player players[], int numPlayers) {
    printf("\nCurrent Board:\n");
    for (int i = 0; i < numPlayers; i++) {
        printf("Player %d tokens: ", i + 1);
        for (int j = 0; j < TOKEN_COUNT; j++) {
            if (players[i].tokens[j].position == HOME_POSITION) {
                printf("H "); // Token has won
            } else if (players[i].tokens[j].inPlay) {
                printf("%d ", players[i].tokens[j].position);
            } else {
                printf("NP "); // Not in Play
            }
        }
        printf("\n");
    }
}

int rollDice() {
    return (rand() % 6) + 1;
}

int chooseToStart(int numPlayers) {
    int currentPlayer = 0;
    int highestRoll = 0;
    int startingPlayer = -1;
    int rolls[numPlayers];

    while (1) {
        printf("Player %d's turn. Press Enter to roll the dice.", currentPlayer + 1);
        getchar();

        int diceRoll = rollDice();
        rolls[currentPlayer] = diceRoll;
        printf("Player %d rolled a %d\n", currentPlayer + 1, diceRoll);

        if (diceRoll == 6) {
            startingPlayer = currentPlayer;
            break;
        }

        if (diceRoll > highestRoll) {
            highestRoll = diceRoll;
            startingPlayer = currentPlayer;
        }

        currentPlayer = (currentPlayer + 1) % numPlayers;

        if (currentPlayer == 0) {
            break;
        }
    }

    if (startingPlayer == -1) {
        for (int i = 0; i < numPlayers; i++) {
            if (rolls[i] > highestRoll) {
                highestRoll = rolls[i];
                startingPlayer = i;
            }
        }
    }

    return startingPlayer;
}

void enterTokenIntoPlay(Player *player, int startPos) {
    for (int i = 0; i < TOKEN_COUNT; i++) {
        if (!player->tokens[i].inPlay) {
            player->tokens[i].position = startPos;
            player->tokens[i].inPlay = 1;
            break;
        }
    }
}

void moveToken(Player *player, int tokenIndex, int steps, const Board *board) {
    if (tokenIndex >= 0 && tokenIndex < TOKEN_COUNT && player->tokens[tokenIndex].inPlay) {
        int currentPosition = player->tokens[tokenIndex].position;
        int path[BOARD_SIZE];
        initializeBoard((Board *)path, player->playerIndex);

        int currentPositionIndex = -1;
        for (int i = 0; i < BOARD_SIZE; i++) {
            if (path[i] == currentPosition) {
                currentPositionIndex = i;
                break;
            }
        }

        if (currentPositionIndex != -1) {
            int newPositionIndex = (currentPositionIndex + steps) % BOARD_SIZE;
            player->tokens[tokenIndex].position = path[newPositionIndex];
        } else {
            printf("Error: Current position not found in path.\n");
        }
    } else {
        printf("Invalid token index or token is not in play.\n");
    }
}

int hasTokensInPlay(Player *player) {
    for (int i = 0; i < TOKEN_COUNT; i++) {
        if (player->tokens[i].inPlay && player->tokens[i].position != HOME_POSITION) {
            return 1;
        }
    }
    return 0;
}

int onlyOneTokenInPlay(Player *player) {
    int inPlayCount = 0;
    for (int i = 0; i < TOKEN_COUNT; i++) {
        if (player->tokens[i].inPlay) {
            inPlayCount++;
        }
    }
    return inPlayCount == 1;
}

void playerTurn(Player *player, const Board *board) {
    int maxChances = 3;
    int chances = 0;

    while (chances < maxChances) {
        int diceRoll = rollDice();
        printf("You rolled a %d\n", diceRoll);

        if (diceRoll == 6) {
            if (!hasTokensInPlay(player)) {
                printf("You rolled a 6. No tokens are in play, so you must enter a token into play.\n");
                enterTokenIntoPlay(player, START_POSITIONS[player->playerIndex]);
            } else {
                int choice = 0;
                while (choice != 1 && choice != 2) {
                    printf("You rolled a 6. Choose an option:\n");
                    printf("1. Move a token 6 spaces.\n");
                    printf("2. Enter a new token into play.\n");
                    scanf("%d", &choice);
                    if (choice != 1 && choice != 2) {
                        printf("Invalid choice. Try again.\n");
                    }
                }
                if (choice == 1) {
                    printf("Choose a token to move 6 spaces (1-%d):\n", TOKEN_COUNT);
                    int tokenIndex;
                    scanf("%d", &tokenIndex);
                    while (tokenIndex < 1 || tokenIndex > TOKEN_COUNT || !player->tokens[tokenIndex - 1].inPlay) {
                        printf("Invalid choice. Try again:\n");
                        scanf("%d", &tokenIndex);
                    }
                    moveToken(player, tokenIndex - 1, 6, board);
                } else {
                    enterTokenIntoPlay(player, START_POSITIONS[player->playerIndex]);
                }
            }
            chances++;
        } else {
            if (onlyOneTokenInPlay(player)) {
                for (int i = 0; i < TOKEN_COUNT; i++) {
                    if (player->tokens[i].inPlay) {
                        moveToken(player, i, diceRoll, board);
                        break;
                    }
                }
            } else if (hasTokensInPlay(player)) {
                printf("Choose a token to move %d spaces (1-%d):\n", diceRoll, TOKEN_COUNT);
                int tokenIndex;
                scanf("%d", &tokenIndex);
                while (tokenIndex < 1 || tokenIndex > TOKEN_COUNT || !player->tokens[tokenIndex - 1].inPlay) {
                    printf("Invalid choice. Try again:\n");
                    scanf("%d", &tokenIndex);
                }
                moveToken(player, tokenIndex - 1, diceRoll, board);
            } else {
                printf("No tokens in play and you did not roll a 6. Turn skipped.\n");
            }
            break;
        }
    }
}

int main() {
    srand(time(0));
    int numPlayers;

    while (1) {
        printf("Enter the number of players (2-4): ");
        scanf("%d", &numPlayers);
        if (numPlayers >= 2 && numPlayers <= 4) {
            break;
        }
        printf("Invalid number of players. Please enter a number between 2 and 4.\n");
    }

    Player players[NUM_PLAYERS];
    Board board;
    for (int i = 0; i < numPlayers; i++) {
        initializePlayer(&players[i], i);
    }

    int currentPlayerIndex = chooseToStart(numPlayers);
    printf("Player %d starts the game!\n", currentPlayerIndex + 1);

    int gameOver = 0;

    while (!gameOver) {
        Player *currentPlayer = &players[currentPlayerIndex];
        printf("\nPlayer %d's turn.\n", currentPlayerIndex + 1);
        playerTurn(currentPlayer, &board);
        displayBoard(players, numPlayers);

        int allTokensInHome = 1;
        for (int i = 0; i < TOKEN_COUNT; i++) {
            if (players[currentPlayerIndex].tokens[i].position != HOME_POSITION) {
                allTokensInHome = 0;
                break;
            }
        }

        if (allTokensInHome) {
            printf("Player %d wins!\n", currentPlayerIndex + 1);
            gameOver = 1;
        } else {
            currentPlayerIndex = (currentPlayerIndex + 1) % numPlayers;
        }
    }

    return 0;
}

