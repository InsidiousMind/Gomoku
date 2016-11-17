#!/usr/bin/env python

import curses
import sys


def main():
    stdscr = initialize()  # Starts the Curses application.
    game_running = True
    one_begin_x = 20
    one_begin_y = 7
    two_begin_x = 70
    two_begin_y = 7
    height = 5
    width = 40
    win1 = curses.newwin(height, width, one_begin_y, one_begin_x)
    win2 = curses.newwin(height, width, two_begin_y, two_begin_x)
    while game_running:
        c = stdscr.getch()
        if c == ord('q'):
            game_running = False  # Exit the while loop
        stdscr.refresh()
    down(stdscr)  # Breaks the application down and ends it.


def initialize():
    stdscr = curses.initscr()
    stdscr.clear()
    curses.noecho()
    curses.cbreak()
    stdscr.keypad(True)
    return stdscr


def down(stdscr):
    curses.nocbreak()
    stdscr.keypad(False)
    curses.echo()
    curses.endwin()
    sys.exit(0)

if __name__ == "__main__":
    main()
