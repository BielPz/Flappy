#ifndef FLAPPYPLAY_H_INCLUDED
#define FLAPPYPLAY_H_INCLUDED

/*
    Creditos para smvd (GitHub user)
    Nome     : Not Flappy Duck
    compiler : GCC
    command  : gcc source.c -o FlappyBird.exe -Werror -Wall -W -s
    license  : www.unlicense.org (Please credit my channel tho)
    youtube  : www.rebrand.ly/eclips-coding
    version  : 3
    source   : https://gist.github.com/smvd/aeaa7544f75988081285ab399c2579c7
    editor   : sublime text 3

    Codigo base foi alterado, sendo as mudanças:
    - Alteração da geração de canos
    - Alteração do sistema de colisão
    - Adicionando um sistema de pontuação
    - Sistema de captura de tecla para Linux
*/

#include <termios.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>                                 // Header for standard functions
#include <string.h>                                 // Header for string manipulation (windows.h also adds this)
#include <stdio.h>                                  // Header for reading and writing
#include <time.h>                                   // Header for time interaction

#define xSize       32
#define ySize       16
#define pipeCount   3
#define qKey        0x51                            // Windows virtual key code for 'Q'
#define GREEN       "\e[32m"                        // ANSI code for green output
#define YELLOW      "\e[33m"                        // ANSI code for yellow output
#define NC          "\e[0m"                         // ANSI code for uncolord output

typedef struct
{
    int x;
    int y;
}PIX;

PIX bird;
PIX pipes[3];

int pontuacao = 0;
int ganhou = 0;
int EmJogo = 1;

void Draw();                                        // Function to draw the game board based on the object positions
void Pipes();                                       // Function to reset the pipes if the hit the end of the screen
void HitTest();                                     // Function to test for collisions with the floor or the pipes

// Ajuda do ChatGPT
char getKey() {
    struct termios oldt, newt;
    int ch;
    
    tcgetattr(STDIN_FILENO, &oldt);                 // Salva o estado atual do terminal
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);               // Desativa buffer de linha e eco
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    
    fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK);       // Modo não bloqueante
    ch = getchar();
    
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);        // Restaura o terminal
    fcntl(STDIN_FILENO, F_SETFL, F_LOCK);          // Volta ao modo normal
    
    return (ch != EOF) ? ch : 0; // Retorna 0 se nenhuma tecla foi pressionada
}

void Draw()
{
    char buff[5000];                                // Variable to hold the buffer that will be drawn
    strcpy(buff, "\e[17A");                         // ANSI code to move the cursor up 17 lines

    for (int y = 0; y <= ySize; y++)                // Loop over each row
    {
        for (int x = 0; x <= xSize; x++)            // Loop over each column
        {
            if                                      // If its a screen edge
            (
                y == 0     ||
                y == ySize ||
                x == 0     ||
                x == xSize
            )
            {
                strcat(buff, NC "[]");              // Add '[]' to the buffer in white
                continue;                           // Move to the next column
            }

            for (int i = 0; i < pipeCount; i++)     // Loop over each pipe
            {
                if                                  // If its the top or bottom pipe face
                (
                    pipes[i].x >= x -1 &&
                    pipes[i].x <= x +1 &&
                    (
                        pipes[i].y == y + 3 ||
                        pipes[i].y == y - 3
                    )
                )
                {
                    strcat(buff, GREEN "[]");       // Add '[]' to the buffer in green
                    goto bottom;                    // Skip the rest of the tests
                }
                else if                             // If its the right angle of the bottom pipe
                (
                    pipes[i].x == x -1 &&
                    pipes[i].y == y - 4
                )
                {
                    strcat(buff, GREEN "]/");
                    goto bottom;
                }
                else if                             // If its the center of the pipe
                (
                    pipes[i].x == x &&
                    (
                        pipes[i].y <= y -4 ||
                        pipes[i].y >= y +4
                    )
                )
                {
                    strcat(buff, GREEN "][");
                    goto bottom;
                }
                else if                             // If its the left angle of the bottom pipe
                (
                    pipes[i].x == x +1 &&
                    pipes[i].y == y - 4
                )
                {
                    strcat(buff, GREEN "\\[");
                    goto bottom;
                }
                else if                             // If its the right angle of the top pipe
                (
                    pipes[i].x == x -1 &&
                    pipes[i].y == y + 4
                )
                {
                    strcat(buff, GREEN "]\\");
                    goto bottom;
                }
                else if                             // If its the left angle of the top pipe
                (
                    pipes[i].x == x +1 &&
                    pipes[i].y == y + 4
                )
                {
                    strcat(buff, GREEN "/[");
                    goto bottom;
                }
                else if                             // If its the left side of the pipe
                (
                    pipes[i].x == x +1 &&
                    (
                        pipes[i].y <= y -5 ||
                        pipes[i].y >= y +5
                    )
                )
                {
                    strcat(buff, GREEN " [");
                    goto bottom;
                }
                else if                             // If its the right side of the pipe
                (
                    pipes[i].x == x -1 &&
                    (
                        pipes[i].y <= y -5 ||
                        pipes[i].y >= y +5
                    )
                )
                {
                    strcat(buff, GREEN "] ");
                    goto bottom;
                }
            }

            // The next bit will simply draw the bird in yellow based on x,y offsets
            if
            (
                bird.y == y &&
                bird.x == x
            )
            {
                strcat(buff, YELLOW ")>");
            }
            else if
            (
                bird.y == y &&
                bird.x == x +1
            )
            {
                strcat(buff, YELLOW "_(");
            }
            else if
            (
                bird.y == y &&
                bird.x == x +2)
            {
                strcat(buff, YELLOW " _");
            }
            else if
            (
                bird.y == y -1 &&
                bird.x == x
            )
            {
                strcat(buff, YELLOW ") ");
            }
            else if
            (
                bird.y == y -1 &&
                bird.x == x +1
            )
            {
                strcat(buff, YELLOW "__");
            }
            else if
            (
                bird.y == y -1 &&
                bird.x == x +2
            )
            {
                strcat(buff, YELLOW " \\");
            }
            else                                    // If its non of the other parts
            {
                strcat(buff, NC "  ");
            }

            bottom:;                                // The point moved to with "goto bottom;"
        }

        strcat(buff, "\n");                         // Append a new line to the buffer

    }
    printf(buff);                                   // Imprime o jogo
}

void Pipes()
{
    for (int i = 0; i < pipeCount; i++)
    {
        if (pipes[i].x == -1)                       // If the pipe is of screen
        {
            int spacing = (rand() % 5) + 13;        // Espaçamento aleatório entre 13 e 17
            pipes[i].x = (i == 0) ? pipes[2].x + spacing : pipes[i - 1].x + spacing;
            pipes[i].y = (rand() % 7) + 5;
        }
    }
}

void Pontuar()
{
    for (int i = 0; i < pipeCount; i++)
    {
        if (bird.x == pipes[i].x) // Só pontua quando o pássaro exatamente passa pelo tubo
        {
            pontuacao++;
        }
    }
}

void PontuacaoFinal()
{
    if(pontuacao >= 4)
    {
        ganhou = 1;    
    }
    else
    {
        ganhou = 0;
    }
}

void HitTest()
{
    if (bird.y+1 >= ySize)                           // If the bird is on the floor
    {
        PontuacaoFinal();
        EmJogo = 0;
    }

    if (bird.y+1 <= 0)                               // If the bird is on the floor
    {
        PontuacaoFinal();
        EmJogo = 0;
    }

    for (int i = 0; i < pipeCount; i++)
    {
        /*
            The next 2 ifs are collision detection.
            With the 2 simple diagrams bellow A is the bird and B is the pipe
            A1  A2
             +--+
             |  |
             +--+
            A3
            B1  B2
             +--+
             |  |
             +--+
            B3
        */


        // Melhorando a detecção da colisão horizontal
        if (bird.x >= pipes[i].x - 1 && bird.x <= pipes[i].x + 1)
        {
            // Definição da área segura (onde o pássaro pode passar sem colidir)
            if ((bird.y < pipes[i].y -2) || (bird.y > pipes[i].y +1))
            {
                PontuacaoFinal();
                EmJogo = 0;
            }
        }
    }
}

int PlayFlappy()
{
    srand(time(NULL));                              // Sets the seed for the random number generator to the current UNIX time stamp
    system("clear");

    char tecla;

    bird.x = 10;                                    // Set the birds start position
    bird.y = 10;

    for (int i = 0; i < pipeCount; i++)
    {
        pipes[i].x = 25 + 15 * i;                   // Set the pipes x to be spaces 15 blocks appart
        pipes[i].y = (rand()%7) +5;                 // Set the pipes y to a random number between 5 and 11
    }

    int frame = 0;                                  // Variable to hold the passed frames

    printf("Press UP to jump and Q to quit.\n");    // List the controls

    for (int i = 0; i <= ySize; i++)                // Makes space for the game
    {
        printf("\n");
    }

    Draw();

    while (EmJogo)
    {
        tecla = getKey();

        if (tecla == '\033')
        {
            if(getKey() == '[')
            {
                switch(getKey())
                {
                case 'A':
                    bird.y -= 2;                    // Move the bird up 2 pixels
                    break;
                }
            }
        }

        if (tecla == 'q')                           // If the user is pressing Q
        {
            break;
        }

        if (frame == 2)                             // If its the third frame
        {
            bird.y++;                               // Drop the bird by 1 pixel

            for (int i = 0; i < 3; i++)             // Move the pipes forwards
            {
                pipes[i].x--;
            }

            Pontuar();                              // Atualiza a pontuação

            frame = 0;                              // Reset the frames
        }

        HitTest();                                  // Test for collisions
        Draw();                                     // Draw the game
        Pipes();                                    // Update the pipes

        frame++;
        usleep(100000);                                 // Wait 100 milliseconds (This may need to be tuned on faster and slower machines as -Os was enough to break it for me)
    }
    if(ganhou)
    {
        printf("\nGame Over! Voce ganhou com pontuacao de: %d\n", pontuacao);
    }
    else
    {
        printf("\nGame Over! Voce perdeu com pontuacao de %d faltando %d para ganhar\n", pontuacao, (4-pontuacao));
    }
    return ganhou;
}



#endif // FLAPPYPLAY_H_INCLUDED
