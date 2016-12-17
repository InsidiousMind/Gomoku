import curses
from curses.textpad import Textbox


class Screen(object):
    def __init__(self, height, width, one_begin_y,
                 one_begin_x, two_begin_y, two_begin_x,
                 thr_begin_y, thr_begin_x, player, player2, chat_v):
        '''
        init for screen in main
        screen = Screen(40, 40, 1, 15, 70, 15, 121, 15, player)
        screen = Screen(40, 40, 43, 120, 15, 72, 15, 121, player, chat)
        Y (first) DOWN
        X (sec) accros -> that way
        the 'self.derwin' windows are derivative windows meant to be borders
        still
        '''

        self.stdscr = self.initialize()

        self.game_command_win = curses.newwin(4, 4, 33, one_begin_x)

        # self.win1_sub = self.win1.derwin(1, 1)
        # self.win1.box()

        self.chat_win = curses.newwin(((3 * height) // 4), ((width * 3) - 5),
                                  two_begin_y, two_begin_x)

        # self.win2_sub = self.win2.derwin(2, 1)
        # self.win2.box()

        self.game_board_win = curses.newwin(30, 30, 14, 120)

        # self.win3_sub = self.win2.derwin(2, 1)
        # self.win3.box()

        self.current_message_win = curses.newwin((height // 4), (width*3)-1,
                                  (two_begin_y + ((3 * height) // 4)), two_begin_x)
        # self.win4_sub = self.win4.derwin(2, 1)
        # self.win4.box()

        self.error_correction_win = curses.newwin(1, 60, 10, 90)

        self.player_stats_win = curses.newwin(10, 16, 0, 70)

        self.other_players_stats_win = curses.newwin(10, 16, 0, 90)

        self.game = Textbox(self.game_command_win)

        self.board_mesg = Textbox(self.current_message_win)

        self.chat = chat_v
        self.player = player
        self.player2 = player2

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
        #Y (first) DOWN
        #X (sec) across -> that way

    def refresh_windows(self):
        """
        Used to force Curses to re-display the entire game window.
        """
        self.print_title()
        self.player.update_pwin()
        self.player2.update_pwin()
        self.game_command_win.refresh()
        self.chat_win.refresh()
        self.game_board_win.refresh()
        self.current_message_win.refresh()
        self.player_stats_win.refresh()
        self.error_correction_win.refresh()
        self.other_players_stats_win.refresh()
        self.stdscr.refresh()       # changing the order of thes

    def update_actionbox(self, msg):
        self.error_correction_win.clear()
        self.error_correction_win.addstr(msg, curses.A_BOLD | curses.A_STANDOUT)
        self.error_correction_win.refresh()