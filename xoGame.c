  #include <io.h>
#include <lcd.h>
#include <delay.h>
#include <stdio.h>
#include <string.h>

#asm
.equ __lcd_port = 0x12
#endasm

    // global variable
int game_board[3][3];
int win[8];
int player, count, state;
int r, c, k;
unsigned char row[4] = {0xFE, 0xFD, 0xFB, 0xF7};
char txt[2];

void keypad()
{
    while (1)
    {
        for (r = 0; r < 4; r++)
        {
            c = -10;
            PORTC = row[r];
            DDRC = 0x0F;
            if (PINC .4 == 0)
                c = 0;
            if (PINC .5 == 1)
                c = 1;
            if (PINC .6 == 1)
                c = 2;
            if (PINC .7 == 1)
                c = 3;

            if (!(c == -10))
            {
                while (PINC .4 == 0)
                    ;
                while (PINC .5 == 0)
                    ;
                while (PINC .6 == 0)
                    ;
                while (PINC .7 == 0)
                    ;
                return;
            }
            delay_ms(5);
        }
    }
}

void init()
{
    player = 0; // player==0 player X gonna play OR player==1 player O gonna play
    count = 0;
    state = 0; // state 0 -> play   state 1 -> player X wins  state 2 -> player O wins
    strcpy(txt, "");
    for (r = 0; r < 3; r++)
        for (c = 0; c < 3; c++)
            game_board[r][c] = 0; // filling our game_board with 0

    for (r = 0; r < 8 ;r++)
        win[r] = 0; // filling our win_board with 0
    r = 0;
    c = 0;
    lcd_clear();
    lcd_puts("Press ON to Start...");

    do
    {
        keypad();
        k = r * 4 + c; // number between 0 to 15
    } while (k != 12);

    lcd_clear();
    for (r = 3; r > 0; r--)
    {
        sprintf(txt, "%d", r);
        lcd_puts(txt);
        delay_ms(200);
        lcd_clear();
    }
}

void showBoard()
{

    lcd_clear();
    for (r = 0; r < 3; r++)
    {
        for (c = 0; c < 3; c++)
        {
            lcd_gotoxy(c, r);
            if (game_board[r][c] == 1)
            {
                lcd_putchar('X');
            }
            else if (game_board[r][c] == -1)
            {
                lcd_putchar('O');
            }
            else if (game_board[r][c] == 0)
            {
                lcd_putchar('-');
            }
        }
    }
}

void check_win()
{
    for (r = 0; r < 8; r++)
    {
        win[r] = 0;
    }
    for (r = 0; r < 3; r++)
        for (c = 0; c < 3; c++)
        {
            win[r] += game_board[r][c];
            win[r + 3] += game_board[c][r];
            if (c == r)
            {
                win[6] = game_board[r][c];
            }
            if (r + c == 2)
            {
                win[7] += game_board[r][c];
            }
        }
}

void main(void)
{
    PORTC = 0xff; // initial value of portc
    DDRC = 0x0f;  // rows are input and columns are output
    lcd_init(20); // set number of lcd columns
    init();
    showBoard();

    while (state == 0)
    {
        keypad();
        if (r != 3 && c != 3)
        {
            if (game_board[r][c] == 0)
            {
                if (player == 0)
                {
                    game_board[r][c] = 1;
                }
                else
                {
                    game_board[r][c] = -1;
                }
                player = !player;
                count++;
            }
        }
        k = r * 4 + c;
        if (k == 13)
        {
            init();
        }
        showBoard();
        check_win();
        for (r = 0; r < 8; r++)
        {
            if (win[r] == 3)
            {
                state = 1;
            }
            else if (win[r] == -3)
            {
                state = -1;
            }
        }
        if (count == 9 && state == 0)
            state = 2;
    }
    lcd_gotoxy(9, 2);
    if (state == 1)
    {
        lcd_puts("X wins!");
    }
    else if (state == -1)
    {
        lcd_puts("O wins!");
    }
    else if (state == 2)
    {
        lcd_puts("Draw");
    }
    while(1){
        keypad();
        k = r*4+c;
        if(k == 13){
            init();
        }
    }
}
