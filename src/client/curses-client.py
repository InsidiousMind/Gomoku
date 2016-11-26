#!/usr/bin/env python

import curses
import traceback
import sys
from curses.textpad import Textbox, rectangle


def main():
    stdscr = initialize()  # Starts the Curses application.
    board = init_board()
    print_board(board)
    game_running = True
    one_begin_x = 1
    one_begin_y = 15
    two_begin_x = 70
    two_begin_y = 15
    thr_begin_x = 121
    thr_begin_y = 15
    height = 40
    width = 40
    # Window 1 takes commands for the game.
    win1 = curses.newwin(height, width, one_begin_y, one_begin_x)
    # Window 2 carries the chat.
    win2 = curses.newwin(height, width, two_begin_y, two_begin_x)
    # Window 3 displays the current game board.
    win3 = curses.newwin(66, 66, thr_begin_y, thr_begin_x)
    win3.addstr(1,1,"Testing")
    print_title(stdscr)
    box1 = Textbox(win1)
    box2 = Textbox(win2)
    try:
        while game_running:
            stdscr.refresh()  # This line begins the interface logic.
            display_board(stdscr, board, win3)
            stdscr.refresh()  # This begins the user interaction
            c = stdscr.getch()
            if c == ord('q'):
                game_running = False  # Exit the while loop
            if c == ord('m'):
                box1.edit()
                stuff = box1.gather()
            if c == ord('c'):
                box2.edit()
                stuff = box2.gather()
            stdscr.refresh() # Redraws the screen.
    except Exception as e:
        print(str(e))
        down(stdscr)
    down(stdscr)  # Breaks the application down and ends it.


def display_board(stdscr, board, win):
    x = 1
    y = 1
    for a in board:
        for b in a:
            win.addch(y, x, ord(b))
            y += 1
        x += 1
    stdscr.refresh()


def init_board():
    '''
    >>> board = init_board()
    >>> board[0][0]
    'o'
    '''
    return [['o' for x in range(8)] for y in range(8)]


def initialize():
    stdscr = curses.initscr()
    stdscr.clear()
    curses.noecho()
    curses.cbreak()
    stdscr.keypad(True)
    sys.stdout = open("log.txt", "a")
    sys.stderr = open("log.txt", "a")
    print("")
    return stdscr


def down(stdscr):
    print("")
    curses.nocbreak()
    stdscr.keypad(False)
    curses.echo()
    curses.endwin()
    import doctest
    doctest.testmod()
    sys.stdout.close()
    sys.stderr.close()
    sys.exit(0)


def print_title(stdscr):
    stdscr.addstr(0,0,"GGGGGGGGGG OOOOOOOOOO MMM MMM MMM OOOOOOOOOO KK      KK UU     UU")
    stdscr.addstr(1,0,"GG         OO      OO MMM MMM MMM OO      OO KK     KK  UU     UU")
    stdscr.addstr(2,0,"GG         OO      OO MM M   M MM OO      OO KK    KK   UU     UU")
    stdscr.addstr(3,0,"GG         OO      OO MM M   M MM OO      OO KK   KK    UU     UU")
    stdscr.addstr(4,0,"GG         OO      OO MM M   M MM OO      OO KK KK      UU     UU")
    stdscr.addstr(5,0,"GG         OO      OO MM M   M MM OO      OO KKK        UU     UU")
    stdscr.addstr(6,0,"GG   GGGG  OO      OO MM M   M MM OO      OO KK KK      UU     UU")
    stdscr.addstr(7,0,"GG   GGGG  OO      OO MM M   M MM OO      OO KK  KK     UU     UU")
    stdscr.addstr(8,0,"GG     GG  OO      OO MM M   M MM OO      OO KK   KK    UU     UU")
    stdscr.addstr(9,0,"GG     GG  OO      OO MM M   M MM OO      OO KK    KK   UU     UU")
    stdscr.addstr(10,0,"GG     GG  OO      OO MM M   M MM OO      OO KK     KK   UU   UU")
    stdscr.addstr(11,0,"GGGGGGGGG  OOOOOOOOOO MM M   M MM OOOOOOOOOO KK      KK   UUUUU")
    stdscr.addstr(14,70,"Chat")
    stdscr.addstr(14,1,"Game Window")
    stdscr.addstr(14,120,"The Board")


def print_board(board):
    for a in board:
        for b in a:
            print(b, end="")
        print("")


if __name__ == "__main__":
    main()
