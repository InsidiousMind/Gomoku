import curses

def main():
    initialize() # Starts the Curses application.
    down() # Breaks the application down and ends it.

def initialize():
    stdscr = curses.initsrc()
    curses.noecho()
    curses.cbreak()
    stdscr.keypad(True)

def down():
    curses.nocbreak()
    stdscr.keypad(False)
    curses.echo()
    curses.endwin()

if __name__ == "__main__":
    main()
