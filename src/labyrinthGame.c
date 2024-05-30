#include <stdio.h> // standard library
#include <stdlib.h> // for malloc function
#include <string.h> // for strlen function
#include <conio.h> // for getch function
#include <time.h> // for srand and time functions
#include <windows.h> // for color function

// Define constants for particle types
#define P_PLUS 'P'
#define E_MINUS 'e'
#define P_MINUS 'p'
#define E_PLUS 'E'
#define BLACK_HOLE 'K'
#define USER 'X'
#define EXIT 'C'
#define WALL '1'
#define PATH '0'
#define START 'G'

// Define structure for user
struct User
{
    char firstName[20]; // 20 characters for first name
    char lastName[20]; // 20 characters for last name
    char username[20]; // 20 characters for username
    char password[20]; // 20 characters for password
    int highScore; // high score
};

// Define structure for game map
struct GameMap
{
    char **Lab;
    int rows;
    int cols;
};

// Function prototypes
void howToPlay(); // how to play
void displayMenu(); // display menu
void viewHighScores(); // view high scores
void loadMapFromFile(struct GameMap *gameMap, const char *fileName); // load map from file
void printMap(struct GameMap *gameMap, char *particlesCollected); // print map
void moveUser(struct User *currentUser, struct GameMap *gameMap, int *userRow, int *userCol, int *key, char *particlesCollected); // move user and collect particles
void autoPlay(struct GameMap *gameMap, int *userRow, int *userCol, int *key, int *prevRow, int *prevCol); // auto play with slow and fast mode
void playGame(struct User *currentUser); // play game
void saveUser(struct User *currentUser); // save user to file
void loadUser(struct User *currentUser); // load user from file
int isUserExists(char *usernameToFind); // check if user exists
int isPasswordCorrect(struct User *currentUser); // check if password is correct

int main()
{
    int key; // for menu choices

    FILE *file = fopen("users.txt", "rb"); // read binary

    if (file == NULL) // if file does not exist
    {
        // create file if not exists
        file = fopen("users.txt", "wb"); // write binary
        file = fopen("users.txt", "rb"); // read binary
        if (file == NULL) // if file does not exist
        {
            fprintf(stderr, "An error occurred while opening the file\n");
            printf("Press any key to exit.\n");
            key = getch();
            exit(EXIT_FAILURE); // exit program
        }
    }

    struct User currentUser; // current user

    // while (fread(&currentUser, sizeof(struct User), 1, file) == 1) 
    // {
    //     printf("%s %s %s %s %d\n", currentUser.firstName, currentUser.lastName, currentUser.username, currentUser.password, currentUser.highScore);
    // }

    printf("1- I'm already a user (Sign In)\n2- I'm not a user (Sign Up)\nChoose to continue: "); // sign in or sign up
    do
    {
        key = getch(); // get key
        if (key == '1') // sign in
        {
            printf("Sign In\n"); 
            loadUser(&currentUser); // load user from file
        }
        else if (key == '2') // sign up
        {
            printf("Sign Up\n");

            printf("Enter your first name: ");
            scanf("%s", currentUser.firstName); // get first name

            printf("Enter your last name: ");
            scanf("%s", currentUser.lastName); // get last name
            do
            {
                printf("Enter your username: ");
                scanf("%s", currentUser.username); // get username

                if (isUserExists(currentUser.username)) // check if user exists
                {
                    printf("This username is already taken! Please try again.\n");
                }
            } while (isUserExists(currentUser.username)); // if user exists, try again

            printf("Enter your password: "); 
            scanf("%s", currentUser.password); // get password
            currentUser.highScore = 0; // set high score to 0

            saveUser(&currentUser); // save user to file
            printf("Welcome %s %s!\n", currentUser.firstName, currentUser.lastName);
        }
        else
        {
            printf("Wrong choice!\n");
        }
    } while (key != '1' && key != '2'); // if key is not 1 or 2, try again

    do 
    {
        displayMenu(); // display menu
        key = getch(); // get key

        switch (key) // switch case
        {
        case '1':
            printf("\nStarting the game...\n");
            playGame(&currentUser); // play game
            break;
        case '2':
            printf("\nViewing High Scores...\n");
            viewHighScores(); // view high scores
            break;
        case '3':
            printf("\nHow to Play...\n");
            howToPlay(); // how to play
            break;
        case '4': // if key is 4 or ESC, exit program
        case 27:
            printf("\nExiting...\n");
            break;
        default: // if key is not 1, 2, 3, 4 or ESC, try again
            printf("\nWrong choice! Please choose a valid option.\n");
            break;
        }

    } while (key != '1' && key != '4' && key != 27); // if key is not 1, 4 or ESC, try again

    return 0;
}

void displayMenu() 
{
    printf("1- Play Game\n");
    printf("2- View High Scores\n");
    printf("3- How to Play\n");
    printf("4- Exit\n");
    printf("Choose to continue: ");
}

void viewHighScores()
{
    FILE *file = fopen("users.txt", "rb"); // read binary

    int i, j, count = 0;

    if (file == NULL) // if file does not exist
    {
        fprintf(stderr, "An error occurred while opening the file\n");
        printf("Press any key to exit.\n");
        int key = getch();
        exit(EXIT_FAILURE);
    }

    struct User users[5]; // array of users

    while (fread(&users[count], sizeof(struct User), 1, file) == 1) // read users from file
    {
        if(users[count].firstName[0] != '\0')
            count++;
    }

    fclose(file);

    for (i = 0; i < count - 1; i++) // sort users by high score
    {
        for (j = 0; j < count - i - 1; j++) 
        {
            if (users[j].highScore < users[j + 1].highScore) // if high score of user j is less than high score of user j + 1
            {
                // Swap
                struct User temp = users[j];
                users[j] = users[j + 1];
                users[j + 1] = temp;
            }
        }
    }

    printf("+-----------------+-----------------+---------+\n");
    printf("|   First Name    |    Last Name    |  Score  |\n");
    printf("+-----------------+-----------------+---------+\n");

    for (i = 0; i < (count < 5 ? count : 5); i++) // print top 5 users
    {
        printf("| %d. %-12s | %-15s | %-7d |\n+-----------------+-----------------+---------+\n", i + 1, users[i].firstName, users[i].lastName, users[i].highScore);
    }
}

void howToPlay()
{
    int key; // for language choice
    printf("1- English\n2- Turkce\nChoose the language: "); // choose language

    do
    {
        key = getch(); // get key
        if (key == '1')
        {
            printf("\n+---------------------------------------------------------------------------------------------------------------------+\n");
            printf("|                                                   LABYRINTH GAME                                                    |\n");
            printf("+---------------------------------------------------------------------------------------------------------------------+\n");
            printf("| 1. The goal of the game is to collect the correct particles and reach the exit with the maximum anti-hydrogen score.|\n");
            printf("| 2. In the labyrinth, there are P+, e-, P-, e+ particles.                                                            |\n");
            printf("| 3. To produce anti-hydrogen, only P- and e+ particles should be in your possession at the exit of the labyrinth.    |\n");
            printf("| 4. If a particle comes together with counterpart, they annihilate each other.     +-------------------------+       |\n");
            printf("| 5. In the game, you will see yourself marked with an x.                           | p+ : proton (P)         |       |\n");
            printf("| 6. If the exit cannot be reached within a certain time, the game ends.            | e- : electron (e)       |       |\n");
            printf("| 7. Passing through black holes ends the game.                                     | p- : anti-proton (p)    |       |\n");
            printf("| 8. You can play the game in manual or auto-play mode.                             | e+ : anti-electron (E)  |       |\n");
            printf("| 9. You can choose a map or load your own map from a file.                         | K : Black hole          |       |\n");
            printf("| 10. In manual mode, you can move with arrow keys and exit with the Esc key.       | G : Entry   C : Exit    |       |\n");
            printf("| 11. You can see the particles you collected after each move.                      | 0 : Path    1 : Wall    |       |\n");
            printf("| 12. In auto-play mode, you can play automatically.                                +-------------------------+       |\n");
            printf("+---------------------------------------------------------------------------------------------------------------------+\n");
        }
        else if (key == '2')
        {
            printf("\n+--------------------------------------------------------------------------------------------------------------+\n");
            printf("|                                                LABIRENT OYUNU                                                |\n");
            printf("+--------------------------------------------------------------------------------------------------------------+\n");
            printf("| 1. Oyunun amaci dogru parcaciklari toplayip maksimum karsit hidrojenle cikisa ulasip skor kazanmaktir.       |\n");
            printf("| 2. Labirentte P+, e-, P-, e+ parcaciklari bulunmaktadir.                                                     |\n");
            printf("| 3. Karsit hidrojen uretebilmek icin labirentin cikisinda elinizde sadece P- ve e+ parcaciklari bulunmalidir. |\n");
            printf("| 4. Bir parcacikla onun karsit parcacigi bir araya gelirse birbirini yok eder.   +-------------------------+  |\n");
            printf("| 5. Oyun icinde kendinizi x isareti ile goreceksiniz.                            | p+ : proton (P)         |  |\n");
            printf("| 6. Belirli surede cikisa ulasilamazsa oyun sonlanir.                            | e- : elektron (e)       |  |\n");
            printf("| 7. Karadeliklerden gecmek oyunu sonlandirir.                                    | p- : karsit proton (p)  |  |\n");
            printf("| 8. Oyunu manuel ya da otomatik modda oynayabilirsiniz.                          | e+ : karsit elektron (E)|  |\n");
            printf("| 9. Harita secimi yapabilir ya da kendi haritanizi yukleyebilirsiniz.            | K : Karadelik           |  |\n");
            printf("| 10. Manuel modda, ok tuslari ile hareket edebilir, Esc tusu ile cikabilirsiniz. | G : Giris   C : Cikis   |  |\n");
            printf("| 11. Her hamlede topladiginiz parcaciklari gorebilirsiniz.                       | 0 : Yol     1 : Duvar   |  |\n");
            printf("| 12. Auto play modunda otomatik olarak oynayabilirsiniz.                         +-------------------------+  |\n");
            printf("+--------------------------------------------------------------------------------------------------------------+\n");
        }
        else
        {
            printf("Wrong choice!\n");
        }
    } while (key != '1' && key != '2'); // if key is not 1 or 2, try again
}

void loadMapFromFile(struct GameMap *gameMap, const char *fileName) // load map from file
{
    FILE *file = fopen(fileName, "r"); // read

    if (file == NULL) // if file does not exist
    {
        fprintf(stderr, "Error opening file %s! Please enter the file name again.\n", fileName);
        scanf("%s", fileName);
        loadMapFromFile(gameMap, fileName); // try again
        return;
    }

    int i, j;

    // Read each character from the file and update the gameMap
    for (i = 0; i < gameMap->rows; i++)
    {
        for (j = 0; j < gameMap->cols; j++)
        {
            if (fscanf(file, " %c", &gameMap->Lab[i][j]) != 1)
            {
                fprintf(stderr, "\nError reading file %s\n", fileName);
                fclose(file);
                printf("Press any key to exit.\n");
                int key = getch();
                exit(EXIT_FAILURE);
            }
        }
    }
    fclose(file); // close file
}

void printMap(struct GameMap *gameMap, char *particlesCollected)
{
    system("cls"); // clear screen

    int i, j, k;

    for (i = 0; i < gameMap->rows; i++) // print map
    {
        printf("\n-");
        for (k = 0; k < gameMap->cols; k++)
            printf("----");
        printf("\n|");
        for (j = 0; j < gameMap->cols; j++)
        {
            setConsoleColor(gameMap->Lab[i][j]); // set color
            printf(" %c ", gameMap->Lab[i][j]); // print character
            resetConsoleColor(); // reset color
            printf("|"); 
        }
    }
    printf("\n-");
    for (k = 0; k < gameMap->cols; k++) // print bottom line
    {
        printf("----");
    }

    printf("\nCollected particles: ");
    for (i = 0; i < strlen(particlesCollected); i++) // print collected particles
    {
        setConsoleColor(particlesCollected[i]); // set color
        if (particlesCollected[i] == P_PLUS)
        {
            printf(" p+ ");
        }
        else if (particlesCollected[i] == E_MINUS)
        {
            printf(" e- ");
        }
        else if (particlesCollected[i] == P_MINUS)
        {
            printf(" p- ");
        }
        else if (particlesCollected[i] == E_PLUS)
        {
            printf(" e+ ");
        }
        resetConsoleColor(); // reset color
    }
    printf("\n");
}

void moveUser(struct User *currentUser, struct GameMap *gameMap, int *userRow, int *userCol, int *key, char *particlesCollected)
{
    int newRow = *userRow; 
    int newCol = *userCol;
    int i;

    // Determine the new position based on the arrow key // 72 up, 80 down, 75 left, 77 right
    if (*key == 72 && *userRow > 0 && gameMap->Lab[*userRow - 1][*userCol] != WALL)
    {
        newRow = *userRow - 1; // Up arrow key
    }
    else if (*key == 80 && *userRow < gameMap->rows - 1 && gameMap->Lab[*userRow + 1][*userCol] != WALL)
    {
        newRow = *userRow + 1; // Down arrow key
    }
    else if (*key == 75 && *userCol > 0 && gameMap->Lab[*userRow][*userCol - 1] != WALL)
    {
        newCol = *userCol - 1; // Left arrow key
    }
    else if (*key == 77 && *userCol < gameMap->cols - 1 && gameMap->Lab[*userRow][*userCol + 1] != WALL)
    {
        newCol = *userCol + 1; // Right arrow key
    }

    // Check if the new position contains a black hole
    if (gameMap->Lab[newRow][newCol] == BLACK_HOLE) 
    {
        printf("You fell into a black hole! Game over!!\nPress any key to exit.\n");
        key = getch();
        exit(EXIT_SUCCESS);
    }
    else if (gameMap->Lab[newRow][newCol] == P_PLUS) 
    {
        particlesCollected[strlen(particlesCollected)] = P_PLUS;
    }
    else if (gameMap->Lab[newRow][newCol] == E_MINUS)
    {
        particlesCollected[strlen(particlesCollected)] = E_MINUS;
    }
    else if (gameMap->Lab[newRow][newCol] == P_MINUS)
    {
        particlesCollected[strlen(particlesCollected)] = P_MINUS;
    }
    else if (gameMap->Lab[newRow][newCol] == E_PLUS)
    {
        particlesCollected[strlen(particlesCollected)] = E_PLUS;
    }
    else if (gameMap->Lab[newRow][newCol] == EXIT) // Check if the new position contains the exit
    {
        if (strlen(particlesCollected) == 0) // Check if the user has any particles
        {
            printf("You don't have any particles! Game over!!\nPress any key to exit.\n");
            key = getch();
            exit(EXIT_SUCCESS);
        }
        else // Check if the user has the correct particles to produce anti-hydrogen
        {
            int pPlusCount = 0, eMinusCount = 0, pMinusCount = 0, ePlusCount = 0, hydroCount = 0;
            for (i = 0; i < strlen(particlesCollected); i++)
            {
                if (particlesCollected[i] == P_PLUS)
                {
                    pPlusCount++;
                }
                else if (particlesCollected[i] == E_MINUS)
                {
                    eMinusCount++;
                }
                else if (particlesCollected[i] == P_MINUS)
                {
                    pMinusCount++;
                }
                else if (particlesCollected[i] == E_PLUS)
                {
                    ePlusCount++;
                }
            }
            int boom = 0; // number of annihilations
            if (pPlusCount >= pMinusCount && pPlusCount > 0 && pMinusCount > 0) // Check if p+ particles annihilate p- particles
            {
                pPlusCount -= pMinusCount;
                pMinusCount = 0;
                boom++;
                printf("\033[1;41m"); // Red
                printf("BOOM! p+ particle(s) annihilated p- particle(s)!\n");
                resetConsoleColor();
            }
            else if (pMinusCount > pPlusCount && pPlusCount > 0 && pMinusCount > 0) // Check if p- particles annihilate p+ particles
            {
                pMinusCount -= pPlusCount;
                pPlusCount = 0;
                boom++;
                printf("\033[1;41m"); // Red
                printf("BOOM! p+ particle(s) annihilated p- particle(s)!\n");
                resetConsoleColor();
            }
            if (eMinusCount >= ePlusCount && ePlusCount > 0 && eMinusCount > 0) // Check if e- particles annihilate e+ particles
            {
                eMinusCount -= ePlusCount;
                ePlusCount = 0;
                boom++;
                printf("\033[1;44m"); // Blue
                printf("BOOM! e+ particle(s) annihilated e- particle(s)!\n");
                resetConsoleColor();
            }
            else if (ePlusCount > eMinusCount && ePlusCount > 0 && eMinusCount > 0) // Check if e+ particles annihilate e- particles
            {
                ePlusCount -= eMinusCount;
                eMinusCount = 0;
                boom++;
                printf("\033[1;44m"); // Blue
                printf("BOOM! e+ particle(s) annihilated e- particle(s)!\n");
                resetConsoleColor();
            }

            if (pMinusCount > 0 && ePlusCount > 0) // Check if anti-hydrogen can be produced
            {
                if (pMinusCount > ePlusCount)
                    hydroCount = ePlusCount;
                else
                    hydroCount = pMinusCount;

                printf("\033[1;42m"); // Green
                printf("Congratulations! You produced %d anti-hydrogen!\nYour score is %d!\n", hydroCount, hydroCount * 100 - 50 * boom); // print score
                resetConsoleColor(); // reset color

                if (hydroCount * 100 - 50 * boom > currentUser->highScore) // update high score
                {
                    currentUser->highScore = hydroCount * 100 - 50 * boom;
                }

                FILE *file = fopen("users.txt", "rb+"); // read and write binary

                if (file == NULL)
                {
                    fprintf(stderr, "An error occurred while opening the file\n"); // if file does not exist
                    printf("Press any key to exit.\n");
                    key = getch();
                    exit(EXIT_FAILURE); // exit program
                }

                int flag = 1; // flag for while loop

                struct User *user = (struct User *)malloc(sizeof(struct User)); // allocate memory for user

                while (fread(user, sizeof(struct User), 1, file) == 1 && flag == 1) // read users from file
                {
                    if (strcmp(user->username, currentUser->username) == 0) // if username is found
                    {
                        user->highScore = currentUser->highScore; // update high score
						printf("scoreuserrr: %s %s",user,currentUser);
                        fseek(file, -sizeof(struct User), SEEK_CUR); // go back
                        fwrite(user, sizeof(struct User), 1, file); // write user to file
                        flag = 0; // exit while loop
                    }
                }

                fclose(file); // close file
                free(user); // free memory

                printf("Press any key to exit.\n");
                key = getch();
                exit(EXIT_SUCCESS); // exit program
            }
            else
            {
                printf("You don't have the correct particles to produce anti-hydrogen! Your score is %d\nPress any key to exit.\n", -50 * boom); // print score and exit
                key = getch();
                exit(EXIT_SUCCESS); // exit program
            }
        }
    }

    // Update the user's position
    gameMap->Lab[*userRow][*userCol] = PATH; // set previous position to path
    *userRow = newRow; // update user row
    *userCol = newCol; // update user column
    gameMap->Lab[*userRow][*userCol] = USER; // set new position to user
}

void autoPlay(struct GameMap *gameMap, int *userRow, int *userCol, int *key, int *prevRow, int *prevCol)
{

    // generate random number
    srand(time(NULL));
    int random = rand() % 2; // 0 or 1

    // 72 up, 80 down, 75 left, 77 right
    if (*userRow > 0 && gameMap->Lab[*userRow - 1][*userCol] == EXIT && prevRow != *userRow - 1)
    {
        *key = 72;
    }
    else if (*userRow < gameMap->rows - 1 && gameMap->Lab[*userRow + 1][*userCol] == EXIT && prevRow != *userRow + 1)
    {
        *key = 80;
    }
    else if (*userCol > 0 && gameMap->Lab[*userRow][*userCol - 1] == EXIT && prevCol != *userCol - 1)
    {
        *key = 75;
    }
    else if (*userCol < gameMap->cols - 1 && gameMap->Lab[*userRow][*userCol + 1] == EXIT && prevCol != *userCol + 1)
    {
        *key = 77;
    } // check if exit is near
    else if (*userRow > 0 && gameMap->Lab[*userRow - 1][*userCol] == P_MINUS || *userRow > 0 && gameMap->Lab[*userRow - 1][*userCol] == E_PLUS)
    {
        *key = 72;
    }
    else if (*userRow < gameMap->rows - 1 && gameMap->Lab[*userRow + 1][*userCol] == P_MINUS || *userRow < gameMap->rows - 1 && gameMap->Lab[*userRow + 1][*userCol] == E_PLUS)
    {
        *key = 80;
    }
    else if (*userCol > 0 && gameMap->Lab[*userRow][*userCol - 1] == P_MINUS || *userCol > 0 && gameMap->Lab[*userRow][*userCol - 1] == E_PLUS)
    {
        *key = 75;
    }
    else if (*userCol < gameMap->cols - 1 && gameMap->Lab[*userRow][*userCol + 1] == P_MINUS || *userCol < gameMap->cols - 1 && gameMap->Lab[*userRow][*userCol + 1] == E_PLUS)
    {
        *key = 77;
    } // check if necessary particles are near
    else if (random == 0) // random move to avoid getting stuck
    {
        if (*prevCol != *userCol + 1 && *userCol < gameMap->cols - 1 && gameMap->Lab[*userRow][*userCol + 1] == PATH)
        {
            *key = 77; // Right arrow key
        }
        else if (*prevRow != *userRow + 1 && *userRow < gameMap->rows - 1 && gameMap->Lab[*userRow + 1][*userCol] == PATH)
        {
            *key = 80; // Down arrow key
        }
        else if (*prevRow != *userRow - 1 && *userRow > 0 && gameMap->Lab[*userRow - 1][*userCol] == PATH)
        {
            *key = 72; // Up arrow key
        }
        else if (*prevCol != *userCol - 1 && *userCol > 0 && gameMap->Lab[*userRow][*userCol - 1] == PATH)
        {
            *key = 75; // Left arrow key
        } // check if previous position is not near
    }
    else if (random == 1) // random move to avoid getting stuck
    {
        if (*prevRow != *userRow - 1 && *userRow > 0 && gameMap->Lab[*userRow - 1][*userCol] == PATH)
        {
            *key = 72; // Up arrow key
        }
        else if (*prevCol != *userCol - 1 && *userCol > 0 && gameMap->Lab[*userRow][*userCol - 1] == PATH)
        {
            *key = 75; // Left arrow key
        }
        else if (*prevCol != *userCol + 1 && *userCol < gameMap->cols - 1 && gameMap->Lab[*userRow][*userCol + 1] == PATH)
        {
            *key = 77; // Right arrow key
        }
        else if (*prevRow != *userRow + 1 && *userRow < gameMap->rows - 1 && gameMap->Lab[*userRow + 1][*userCol] == PATH)
        {
            *key = 80; // Down arrow key
        } // check if previous position is not near
    }
    else if (*userRow > 0 && gameMap->Lab[*userRow - 1][*userCol] == P_PLUS || *userRow > 0 && gameMap->Lab[*userRow - 1][*userCol] == E_MINUS)
    {
        *key = 72;
    }
    else if (*userRow < gameMap->rows - 1 && gameMap->Lab[*userRow + 1][*userCol] == P_PLUS || *userRow < gameMap->rows - 1 && gameMap->Lab[*userRow + 1][*userCol] == E_MINUS)
    {
        *key = 80;
    }
    else if (*userCol > 0 && gameMap->Lab[*userRow][*userCol - 1] == P_PLUS || *userCol > 0 && gameMap->Lab[*userRow][*userCol - 1] == E_MINUS)
    {
        *key = 75;
    }
    else if (*userCol < gameMap->cols - 1 && gameMap->Lab[*userRow][*userCol + 1] == P_PLUS || *userCol < gameMap->cols - 1 && gameMap->Lab[*userRow][*userCol + 1] == E_MINUS)
    {
        *key = 77;
    } // choose unnecessary particles if path, necessary particle or exit is not near
    else if (*userRow > 0 && gameMap->Lab[*userRow - 1][*userCol] == BLACK_HOLE)
    {
        *key = 72;
    }
    else if (*userRow < gameMap->rows - 1 && gameMap->Lab[*userRow + 1][*userCol] == BLACK_HOLE)
    {
        *key = 80;
    }
    else if (*userCol > 0 && gameMap->Lab[*userRow][*userCol - 1] == BLACK_HOLE)
    {
        *key = 75;
    }
    else if (*userCol < gameMap->cols - 1 && gameMap->Lab[*userRow][*userCol + 1] == BLACK_HOLE)
    {
        *key = 77;
    } // choose the black hole if nothing is an option
    else
    {
        printf("You are stuck! Game over!!\nPress any key to exit.\n");
        key = getch();
        exit(EXIT_SUCCESS);
    } // exit if not an option is available
}

void playGame(struct User *currentUser)
{
    struct GameMap gameMap; // game map object
    int key, i, j; // key for arrow keys, i and j for loops
    int userRow = 0, userCol = 0, particles = 0; // user row and column, particles collected
    char particlesCollected[20] = ""; // particles collected

    do 
    {
        printf("1- Choose a map\n2- Load your own map from a file\nChoose to continue: ");
        key = getch(); // get key
        if (key == '1') // choose a map
        {
            printf("\nChoose a map:\n");
            printf("1- Map 1\n2- Map 2\n3- Map 3\nChoose to continue: ");
            key = getch(); // get key

            if (key == '1')
            {
                gameMap.rows = 10; // set rows
                gameMap.cols = 10; // set columns

                gameMap.Lab = (char **)malloc(gameMap.rows * sizeof(char *)); // allocate memory for rows

                for (i = 0; i < gameMap.rows; i++) 
                {
                    gameMap.Lab[i] = (char *)malloc(gameMap.cols * sizeof(char)); // allocate memory for columns
                }

                char map[10][10] = { 
                    {'G', '0', '0', '0', 'E', '1', '0', '0', '0', 'p'},
                    {'0', '1', '1', 'P', '0', 'K', 'e', '1', '0', 'K'},
                    {'0', '0', 'e', '0', '0', '1', '0', '1', '0', 'E'},
                    {'K', '0', '1', '0', 'K', '0', '0', '0', '0', '1'},
                    {'0', '0', '1', '0', '1', 'p', '1', '0', 'K', '1'},
                    {'E', '1', '0', '0', '0', '0', 'e', '0', '0', '0'},
                    {'0', 'K', '0', 'p', '1', '0', 'K', '1', '0', '1'},
                    {'0', '0', '0', '1', '0', '0', 'C', '0', '0', '1'},
                    {'1', '0', 'K', 'p', '0', '1', 'P', '1', '0', 'K'},
                    {'E', '0', '0', '0', 'P', '0', '0', '1', '0', 'p'}}; // map

                for (i = 0; i < gameMap.rows; i++) // copy map to game map
                {
                    for (j = 0; j < gameMap.cols; j++)
                    {
                        gameMap.Lab[i][j] = map[i][j];
                        if (gameMap.Lab[i][j] == USER || gameMap.Lab[i][j] == START) // find user position
                        {
                            userRow = i;
                            userCol = j;
                        }
                    }
                }
            }
            else if (key == '2') // choose map 2
            {
                gameMap.rows = 10;
                gameMap.cols = 10;

                gameMap.Lab = (char **)malloc(gameMap.rows * sizeof(char *)); // allocate memory for rows
                for (i = 0; i < gameMap.rows; i++)
                {
                    gameMap.Lab[i] = (char *)malloc(gameMap.cols * sizeof(char)); // allocate memory for columns
                }

                char map[10][10] = {
                    {'C', '0', '0', '0', 'E', '1', '0', '0', '0', 'p'},
                    {'0', '1', '1', 'P', '0', 'K', 'e', '1', '0', 'K'},
                    {'0', '0', 'e', '0', '0', '1', '0', '1', '0', 'E'},
                    {'K', '0', '1', '0', 'K', '0', '0', '0', '0', '1'},
                    {'0', '0', '1', '0', '1', 'p', '1', '0', 'K', '1'},
                    {'E', '1', '0', '0', '0', '0', 'e', '0', '0', '0'},
                    {'0', 'K', '0', 'p', '1', '0', 'K', '1', '0', '1'},
                    {'0', '0', '0', '1', '0', '0', 'G', '0', '0', '1'},
                    {'1', '0', 'K', 'p', '0', '1', 'P', '1', '0', 'K'},
                    {'E', '0', '0', '0', 'P', '0', '0', '1', '0', 'p'}};

                for (i = 0; i < gameMap.rows; i++) // copy map to game map
                {
                    for (j = 0; j < gameMap.cols; j++)
                    {
                        gameMap.Lab[i][j] = map[i][j];
                        if (gameMap.Lab[i][j] == USER || gameMap.Lab[i][j] == START) // find user position
                        {
                            userRow = i;
                            userCol = j;
                        }
                    }
                }
            }
            else
            {
                gameMap.rows = 10;
                gameMap.cols = 10;

                gameMap.Lab = (char **)malloc(gameMap.rows * sizeof(char *)); // allocate memory for rows
                for (i = 0; i < gameMap.rows; i++)
                {
                    gameMap.Lab[i] = (char *)malloc(gameMap.cols * sizeof(char)); // allocate memory for columns
                }

                char map[10][10] = {
                    {'0', '0', '0', '0', 'E', '1', '0', '0', '0', 'p'},
                    {'0', '1', '1', 'P', '0', 'K', 'e', '1', '0', 'K'},
                    {'0', '0', 'e', '0', '0', '1', '0', '1', '0', 'E'},
                    {'K', '0', '1', '0', 'K', '0', '0', '0', '0', '1'},
                    {'C', '0', '1', '0', '1', 'p', '1', '0', 'K', '1'},
                    {'E', '1', '0', '0', '0', '0', 'e', '0', '0', '0'},
                    {'0', 'K', '0', 'p', '1', '0', 'K', '1', '0', '1'},
                    {'0', '0', '0', '1', '0', '0', '0', '0', '0', '1'},
                    {'1', '0', 'K', 'p', '0', '1', 'P', '1', '0', 'K'},
                    {'E', '0', '0', '0', 'P', '0', '0', '1', '0', 'G'}};

                for (i = 0; i < gameMap.rows; i++) // copy map to game map
                {
                    for (j = 0; j < gameMap.cols; j++)
                    {
                        gameMap.Lab[i][j] = map[i][j];
                        if (gameMap.Lab[i][j] == USER || gameMap.Lab[i][j] == START) // find user position
                        {
                            userRow = i;
                            userCol = j;
                        }
                    }
                }
                key = '1'; // set key to 1 to break loop
            }
        }
        else if (key == '2') // load map from file
        {
            char fileName[30]; // file name

            printf("\nEnter the number of rows: "); // get rows, columns and file name
            scanf("%d", &gameMap.rows);
            printf("Enter the number of columns: ");
            scanf("%d", &gameMap.cols);
            printf("Enter the file name (i.e. map.txt): ");
            scanf("%s", fileName);

            gameMap.Lab = (char **)malloc(gameMap.rows * sizeof(char *)); // allocate memory for rows
            int i;
            for (i = 0; i < gameMap.rows; i++)
            {
                gameMap.Lab[i] = (char *)malloc(gameMap.cols * sizeof(char)); // allocate memory for columns
            }

            loadMapFromFile(&gameMap, fileName); // load map from file

            for (i = 0; i < gameMap.rows; i++) // find user position
            {
                for (j = 0; j < gameMap.cols; j++)
                {
                    if (gameMap.Lab[i][j] == USER || gameMap.Lab[i][j] == START)
                    {
                        userRow = i;
                        userCol = j;
                    }
                }
            }
        }
        else
        {
            printf("Wrong choice!\n");
        }
    } while (key != '1' && key != '2'); // if key is not 1 or 2, try again

    gameMap.Lab[userRow][userCol] = USER; // set user position

    printMap(&gameMap, particlesCollected); // print map

    printf("You can move with arrow keys and exit with the Esc key.\nPress 'a' to auto-play.\n"); 

    time_t startTime, currentTime; // start time and current time
    time(&startTime); // get start time

    while (1) // infinite loop
    {
        key = getch(); // get key

        if (key == 27) 
        { // esc
            printf("Exiting...\nPress any key to exit.\n");
            key = getch();
            exit(EXIT_SUCCESS);
        }
        else if (key == 224)
        { // arrow keys
            key = getch();
            i = 0;
            moveUser(currentUser, &gameMap, &userRow, &userCol, &key, particlesCollected); // move user
        }
        else if (key == 'a' || key == 'A') // auto play
        {
            int prevRow = userRow, prevCol = userCol, speed = 1; // previous row and column, speed for auto play
            do
            {
                printf("Choose the speed.\nFast [1]\nSlow [2]\n"); // get speed
                speed = getch(); // get key
            } while (speed != '1' && speed != '2'); // if key is not 1 or 2, try again
            while (1) // infinite loop
            {
                autoPlay(&gameMap, &userRow, &userCol, &key, &prevRow, &prevCol); // auto play
                prevRow = userRow; // update previous row and column
                prevCol = userCol; // update previous row and column
                moveUser(currentUser, &gameMap, &userRow, &userCol, &key, particlesCollected); // move user
                printMap(&gameMap, particlesCollected); // print map
                time(&currentTime); // get current time

                if (speed == '1') // fast
                {
                    printf("Time left: %d seconds\n", 31 - (int)difftime(currentTime, startTime)); // print time left
                    if (difftime(currentTime, startTime) > 30) // if time is up, exit
                    {
                        printf("Time is up! Game over!!\nPress any key to exit.\n");
                        key = getch(); 
                        exit(EXIT_SUCCESS); // exit program
                    }
                    sleep(0); // sleep for 0 seconds
                }
                else if (speed == '2') // slow
                {
                    printf("Time left: %d seconds\n", 121 - (int)difftime(currentTime, startTime)); // print time left
                    if (difftime(currentTime, startTime) > 120) // if time is up, exit
                    {
                        printf("Time is up! Game over!!\nPress any key to exit.\n");
                        key = getch();
                        exit(EXIT_SUCCESS); // exit program
                    }
                    sleep(1); // sleep for 1 second
                }
            }
        }
        else
        {
            printf("Wrong choice!\n");
        }

        printMap(&gameMap, particlesCollected); // print map

        time(&currentTime); // get current time
        printf("Time left: %d seconds\n", 41 - (int)difftime(currentTime, startTime)); // print time left

        if (difftime(currentTime, startTime) > 40) // if time is up, exit
        {
            printf("Time is up! Game over!!\nPress any key to exit.\n");
            key = getch();
            exit(EXIT_SUCCESS); // exit program
        }
    }

    // Free the memory allocated for the game map
    for (i = 0; i < gameMap.rows; i++)
    {
        free(gameMap.Lab[i]);
    }
    free(gameMap.Lab);
}

void saveUser(struct User *currentUser) // save user to file
{
    FILE *file = fopen("users.txt", "ab"); // append binary

    if (file == NULL) // if file does not exist
    {
        fprintf(stderr, "An error occurred while opening the file\n"); // print error
        printf("Press any key to exit.\n");
        int key = getch();
        exit(EXIT_FAILURE); // exit program
    }

    fwrite(currentUser, sizeof(struct User), 1, file); // write user to file

    fclose(file); // close file
}

void loadUser(struct User *currentUser)
{
    FILE *file = fopen("users.txt", "rb"); // read binary

    if (file == NULL) // if file does not exist
    {
        fprintf(stderr, "An error occurred while opening the file\n");
        printf("Press any key to exit.\n");
        int key = getch();
        exit(EXIT_FAILURE); // exit program
    }

    int userFound = 0; // flag for while loop
    struct User *user = (struct User *)malloc(sizeof(struct User)); // allocate memory for user

    char usernameToFind[20]; // username to find
    do
    {
        printf("Enter your username: ");
        scanf("%s", usernameToFind);

        while (fread(user, sizeof(struct User), 1, file) == 1 && userFound == 0) // read users from file
        {
            if (strcmp(user->username, usernameToFind) == 0) // if username is found
            {
                do
                {
                    printf("Enter your password: ");
                    scanf("%s", user->password); // get password

                    if (!isPasswordCorrect(user)) // if password is not correct
                    {
                        printf("Wrong password! Please try again.\n");
                    }
                } while (!isPasswordCorrect(user)); // if password is not correct, try again

                printf("Welcome %s %s!\n", user->firstName, user->lastName); // welcome user
                userFound = 1; // exit while loop
                *currentUser = *user; // update current user
            }
        }
        if (userFound == 0) // if user is not found
        {
            printf("User not found! Please try again. \n");
            fseek(file, 0, SEEK_SET); // go back to the beginning of the file
        }
    } while (userFound == 0); // if user is not found, try again

    fclose(file); // close file
    free(user); // free memory
}

int isUserExists(char *usernameToFind)
{
    FILE *file = fopen("users.txt", "rb"); // read binary

    if (file == NULL) // if file does not exist
    {
        fprintf(stderr, "An error occurred while opening the file\n");
        printf("Press any key to exit.\n");
        int key = getch();
        exit(EXIT_FAILURE); // exit program
    }

    struct User *user = (struct User *)malloc(sizeof(struct User)); // allocate memory for user

    while (fread(user, sizeof(struct User), 1, file) == 1) // read users from file
    {
        if (strcmp(user->username, usernameToFind) == 0) // if username is found
        {
            return 1; // return true
        }
    }

    fclose(file); // close file
    free(user); // free memory
    return 0; // return false
}

int isPasswordCorrect(struct User *currentUser)
{
    FILE *file = fopen("users.txt", "rb"); // read binary
 
    if (file == NULL) // if file does not exist
    {
        fprintf(stderr, "An error occurred while opening the file\n");
        printf("Press any key to exit.\n");
        int key = getch();
        exit(EXIT_FAILURE); // exit program
    }

    struct User *user = (struct User *)malloc(sizeof(struct User)); // allocate memory for user

    while (fread(user, sizeof(struct User), 1, file) == 1) // read users from file
    {
        if (strcmp(user->username, currentUser->username) == 0 && strcmp(user->password, currentUser->password) == 0) // if username and password are correct
        {
            return 1; // return true
        }
    }

    fclose(file); // close file
    free(user); // free memory
    return 0; // return false
}

void setConsoleColor(char c) // set a color for each character
{
    if (c == USER) 
    {
        printf("\033[1;42m"); // Green color for USER
    }
    if (c == P_PLUS)
    {
        printf("\033[1;43m"); // Yellow color for P_PLUS
    }
    if (c == E_MINUS)
    {
        printf("\033[1;44m"); // Blue color for E_MINUS
    }
    if (c == P_MINUS)
    {
        printf("\033[1;41m"); // Red color for P_MINUS
    }
    if (c == E_PLUS)
    {
        printf("\033[1;46m"); // Cyan color for E_PLUS
    }
    if (c == BLACK_HOLE)
    {
        printf("\033[1;45m"); // Magenta color for BLACK_HOLE
        printf("\033[1;30m"); // Black color for BLACK_HOLE
    }
    if (c == EXIT)
    {
        printf("\033[1;42m"); // Green color for EXIT
    }
    if (c == WALL)
    {
        printf("\033[1;47m"); // White color for WALL
    }
    if (c == PATH)
    {
        printf("\033[1;37m"); // White color for PATH
    }
    if (c == START)
    {
        printf("\033[1;42m"); // Green color for START
    }
}

void resetConsoleColor() 
{ 
    printf("\033[0m"); // Reset color
}
