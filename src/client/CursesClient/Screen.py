import curses
from curses.textpad import Textbox

# TODO:


class Screen(object):
    def __init__(self, chat_v):
        """
        init for screen in main
        screen = Screen(40, 40, 1, 15, 70, 15, 121, 15, player)
        screen = Screen(40, 40, 43, 120, 15, 72, 15, 121, player, chat)
        Y (first) DOWN
        X (sec) across -> that way
        the 'self.derwin' windows are derivative windows meant to be borders
        still
        """
        self.stdscr = self.initialize()
        self.windows = {}
        self.init_base_windows()
        self.game = Textbox(self.windows["game_commands"])
        self.board_mesg = Textbox(self.windows["current_message"])
        self.chat = chat_v

    @staticmethod
    def initialize():
        stdscr = curses.initscr()
        stdscr.clear()
        curses.noecho()
        curses.cbreak()
        curses.start_color()
        curses.use_default_colors()
        stdscr.keypad(True)
        return stdscr

    '''
    y (first) DOWN
    x (second) across -> that way
    more will be added in this function when we add things like
    borders etc
    '''
    def create(self, height, width, begin_y, begin_x, key):
        self.windows[key] = curses.newwin(height, width, begin_y, begin_x)

    # create windows here
    # a function in Screen should handle resizing based on
    # stuff passed here
    # TODO
    def init_base_windows(self):
        # screen = Screen(40, 40, 43, 120, 15, 72, 15, 121, player, chat)
        self.create(4, 4, 33, 120, "game_commands")
        self.create(((3 * 40) // 4), ((40 * 3) - 5), 15, 72, "chat")
        self.create(30, 30, 14, 120, "board")
        self.create(40 // 4, ((40 * 3) - 1), (15 + ((3 * 40) // 4)), 72, "current_message")
        self.create(1, 60, 10, 90, "actionbox")
        self.create(10, 16, 0, 70, "player_stats")
        self.create(10, 16, 0, 70, "player2_stats")


    # 75-> that way
    # 12 down
    def print_title(self):
        self.stdscr.addstr(0, 0, "GGGGGGGGGG OOOOOOOOOO MMM MMM MMM OOOOOOOOOO KK      KK UU     UU", curses.A_BLINK)
        self.stdscr.addstr(1, 0, "GG         OO      OO MMM MMM MMM OO      OO KK     KK  UU     UU", curses.A_BLINK)
        self.stdscr.addstr(2, 0, "GG         OO      OO MM M   M MM OO      OO KK    KK   UU     UU", curses.A_BLINK)
        self.stdscr.addstr(3, 0, "GG         OO      OO MM M   M MM OO      OO KK   KK    UU     UU", curses.A_BLINK)
        self.stdscr.addstr(4, 0, "GG         OO      OO MM M   M MM OO      OO KK KK      UU     UU", curses.A_BLINK)
        self.stdscr.addstr(5, 0, "GG         OO      OO MM M   M MM OO      OO KKK        UU     UU", curses.A_BLINK)
        self.stdscr.addstr(6, 0, "GG   GGGG  OO      OO MM M   M MM OO      OO KK KK      UU     UU", curses.A_BLINK)
        self.stdscr.addstr(7, 0, "GG   GGGG  OO      OO MM M   M MM OO      OO KK  KK     UU     UU", curses.A_BLINK)
        self.stdscr.addstr(8, 0, "GG     GG  OO      OO MM M   M MM OO      OO KK   KK    UU     UU", curses.A_BLINK)
        self.stdscr.addstr(9, 0, "GG     GG  OO      OO MM M   M MM OO      OO KK    KK   UU     UU", curses.A_BLINK)
        self.stdscr.addstr(10, 0, "GG     GG  OO      OO MM M   M MM OO      OO KK     KK   UU   UU", curses.A_BLINK)
        self.stdscr.addstr(11, 0, "GGGGGGGGG  OOOOOOOOOO MM M   M MM OOOOOOOOOO KK      KK   UUUUU", curses.A_BLINK)
        self.stdscr.addstr(13, 1, "Chat")
        self.stdscr.addstr(32, 120, "Game Window")
        self.stdscr.addstr(13, 120, "The Board")
        self.stdscr.addstr(7, 70, "Press 'm' to move, 'c' to chat, 'Ctrl-g' to enter a move or chat msg")
        # Y (first) DOWN
        # X (sec) across -> that way

    # args should be a list of Player objects, if they aren't Player objs
    # ignore them
    def refresh_windows(self):
        """
        Used to force Curses to re-display the entire game window.
        """
        self.print_title()
        for key in self.windows:
            self.windows[key].refresh()
        self.stdscr.refresh()

    def update_actionbox(self, msg):
        self.windows["actionbox"].clear()
        self.windows["actionbox"].addstr(msg, curses.A_BOLD | curses.A_STANDOUT)
        self.windows["actionbox"].refresh()
