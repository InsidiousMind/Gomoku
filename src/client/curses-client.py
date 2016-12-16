#!/usr/bin/env python

import curses
import logging
import random
import socket
import struct
import sys
import threading
import time
from curses.textpad import Textbox
from socket import ntohl


class Player(object):
    def __init__(self, name, upid, wins, losses, ties, chat_v):
        self.chat = chat_v
        self.name = name
        self.upid = upid
        self.wins = wins
        self.losses = losses
        self.ties = ties
        self.win = None

    def print_player(self):
        print(str(self.name) + ' UPID: ' + str(self.upid) + ' has ' + str(self.wins) + ' wins ' + str(
            self.losses) + ' losses ' + ' and ' + str(self.ties) + ' ties.')

    def recv_player(self, sock):
        while True:
            check = sock.recv(1, socket.MSG_PEEK).decode("utf-8")
            if check == '\v':
                self.chat.recv_msg()
            else:
                player = sock.recv(40)
                player = struct.unpack('!IccccccccccccccccccccIIII', player)
                self.upid = int(player[0])
                self.name = ''
                for x in range(1, 21):
                    if player[x].decode("utf-8") != ' ':
                        self.name += player[x].decode("utf-8")
                self.wins = int(player[21])
                self.losses = int(player[22])
                self.ties = int(player[23])
                break

    def update_pwin(self):
        self.win.clear()
        self.win.addstr(0, 0, "Player: " + str(self.name))
        self.win.addstr(1, 0, "Unique PID: " + str(self.upid))
        self.win.addstr(2, 0, "Wins: " + str(self.wins))
        self.win.addstr(3, 0, "Losses: " + str(self.losses))
        self.win.addstr(4, 0, "Ties:" + str(self.ties))
        self.win.refresh()



class Chat(threading.Thread):
    def __init__(self, win, sock):
        # noinspection PyArgumentList
        super(Chat, self).__init__()
        self.sock = sock
        self.window = win
        self.row = 0
        self.col = 0

    def run(self):
        while True:
            time.sleep(1)
            self.window.refresh()

    def recv_msg(self):
        msg = self.sock.recv(1024)
        msg = msg.decode("utf-8")
        msg = msg[1:]  # get rid of the vertical tab
        if self.row >= 40:  # scrolling stuff
            self.window.clear()
            self.row = 0
        self.window.addstr(self.row, self.col, msg)
        self.row += 1
        self.window.refresh()


# define screen variables
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
        '''

        self.stdscr = self.initialize()  # Starts the Curses application.

        # Window 1 takes commands for the game.      43        120
        self.win1 = curses.newwin(4, 4, 33, one_begin_x)
        # self.win1_sub = self.win1.derwin(1, 1)
        # self.win1.box()

        # Window 2 carries the chat.
        self.win2 = curses.newwin(((3 * height) // 4), ((width * 3) - 5),
                                  two_begin_y, two_begin_x)

        # self.win2_sub = self.win2.derwin(2, 1)
        # self.win2.box()

        # Window 3 displays the current game board. 40 40      15 121
        self.win3 = curses.newwin(30, 30, 14, 120)
        # self.win3_sub = self.win2.derwin(2, 1)
        # self.win3.box()

        # Window 4 displays the message that the player is currently typing out.
        self.win4 = curses.newwin((height // 4), (width*3)-1,
                                  (two_begin_y + ((3 * height) // 4)), two_begin_x)
        # self.win4_sub = self.win4.derwin(2, 1)
        # self.win4.box()

        # win5 is the textbox for player stats
        self.win5 = curses.newwin(10, 16, 0, 70)

        #win6 is the window for error correction and telling the player what to do
        self.win6 = curses.newwin(1, 60, 10, 90)

        # window 7 is the window for the other players stats
        self.win7 = curses.newwin(10, 16, 0, 90)

        # takes messages for the game
        self.game = Textbox(self.win1)
        # takes messages for current game board
        self.board_mesg = Textbox(self.win4)
        # commands for game
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

    def halt(self):  # Just to cut down on a few lines in main()
        down(self.stdscr)

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
        self.print_title()
        self.player.update_pwin()
        self.player2.update_pwin()
        self.win1.refresh()
        self.win2.refresh()
        self.win3.refresh()
        self.win4.refresh()
        self.win5.refresh()
        self.win6.refresh()
        self.win7.refresh()
        self.stdscr.refresh()       # changing the order of thes

    def update_actionbox(self, msg):
        self.win6.clear()
        self.win6.addstr(msg, curses.A_BOLD | curses.A_STANDOUT)
        self.win6.refresh()

class GIPS(object):
    def __init__(self, sock, chat_v):
        logging.debug("GIPS.sock is being defined.")
        self.sock = sock
        logging.debug(self.sock)
        self.pid = 0
        self.is_win = 0
        self.move_a = -1
        self.move_b = -1
        self.isEarlyExit = 0
        self.chat = chat_v

        # not a real part of gips struct
        # it's in gips here for ease of use
        self.upid = 0

    def pack(self, pid, is_win, move_a, move_b, is_early_exit):
        logging.debug("Packing: " + str(pid) + " " + str(is_win) +
                      " " + str(move_a) + " " + str(move_b))
        self.is_win = is_win
        self.pid = pid
        self.move_a = move_a
        self.move_b = move_b
        self.isEarlyExit = is_early_exit

    def send(self):
        logging.debug("Sending.")
        self.sock.send(struct.pack('ccccc', int(self.pid).to_bytes(1, sys.byteorder),
                                   int(self.is_win).to_bytes(1, sys.byteorder),
                                   int(self.move_a).to_bytes(1, sys.byteorder),
                                   int(self.move_b).to_bytes(1, sys.byteorder),
                                   int(self.isEarlyExit).to_bytes(1, sys.byteorder)))

    def recv(self):
        while True:
            check = self.sock.recv(1, socket.MSG_PEEK).decode("utf-8")
            if check == '\v':
                self.chat.recv_msg()
            else:
                self.sock.setblocking(True)
                self.pid = ord(self.sock.recv(1))
                self.is_win = ord(self.sock.recv(1))
                self.move_a = ord(self.sock.recv(1))
                self.move_b = ord(self.sock.recv(1))
                self.isEarlyExit = ord(self.sock.recv(1))

                if self.move_a == 255:
                    self.move_a = -1
                if self.move_b == 255:
                    self.move_b = -1
                logging.debug("Received: " + str(self))
                break

    def recv_iswin(self):
        while True:
            check = self.sock.recv(1, socket.MSG_PEEK).decode("utf-8")
            if check == '\v':
                self.chat.recv_msg()
            else:
                is_win = self.sock.recv(4)
                is_win = struct.unpack('!i', is_win)
                is_win = ntohl(is_win[0])
                return is_win

    def recv_pid(self):
        while True:
            check = self.sock.recv(1, socket.MSG_PEEK).decode("utf-8")
            if check == '\v':
                self.chat.recv_msg()
            else:
                return ord(self.sock.recv(1))

    # Upid is not apart of the GIPs being sent to and from the server
    def set_upid(self, upid):
        self.upid = upid


# noinspection PyBroadException
def main():
    file_id = str(random.randrange(1000))
    logging.basicConfig(filename='log' + file_id + '.txt', level=logging.DEBUG,
                        format='[%(asctime)-15s] %(message)s PID: %(process)d LINE: %(lineno)d')
    logging.debug("Client initializing!")
    host = "localhost"
    port = 32200
    logging.info("Trying to connect on " + str(host) + ":" + str(port))
    print("Welcome to GOMOKU")
    print("USERNAME")
    username = input("> ")
    print("PLAYER ID NUMBER")
    upid = input("> ")
    # Login with our unique pid.
    # Talk to the server and see what we can get.
    # Get a chat_socket
    # Get your player number from the server.
    # create player object
    chat_v = object
    pid = ''
    # screen = Screen(40, 40, 43, 120, 15, 72, 15, 121, player, chat)
    # Y (first) DOWN
    # X (sec) across -> that way
    # create player objects and windows
    player = Player(username, upid, 0, 0, 0 , chat )
    player2 = Player("", 0, 0, 0, 0, chat)
    screen = Screen(40, 40, 43, 120, 15, 1, 15, 121, player, player2, chat)
    player.win = screen.win5
    player2.win = screen.win7

    logging.debug("Game starting.")
    gips = GIPS
    try:
        keep_playing = True
        while keep_playing:
            screen.stdscr.clear()
            screen.win6.clear()
            screen.print_title()
            screen.stdscr.addstr(6, 70, "The game will be starting shortly....",
                         curses.A_BLINK | curses.A_BOLD | curses.COLOR_RED)
            screen.refresh_windows()
            sock = establish_connection(host, port)
            gips = GIPS(sock, chat_v)
            upid = login(sock, upid, username)
            gips.upid = upid
            # init chat_v thread
            chat_v = Chat(screen.win2, sock)
            init_chat(chat_v, screen, gips)
            screen.stdscr.clear()
            pid = gips.recv_pid()
            screen.player.recv_player(sock)
            # player2 for stats
            screen.player2.recv_player(sock)
            screen.refresh_windows()
            board = init_board()
            gips = game_loop(board, pid, screen, gips)
            keep_playing = reboot_game_seq(gips, screen, pid)
        # end the game once the while loop dies
        end_game(gips, screen, pid)

    except Exception:
        logging.exception("Exception caught")
        gips.sock.shutdown(socket.SHUT_RDWR)
        gips.sock.close()
        screen.halt()
        sys.exit(0)
    except KeyboardInterrupt:
        logging.exception("SIGINT received")
        gips.sock.shutdown(socket.SHUT_RDWR)
        gips.sock.close()
        screen.halt()
        sys.exit(0)


def init_chat(chat_v, screen, gips):
    screen.chat = chat_v
    gips.chat = chat_v


# noinspection PyBroadException


def establish_connection(host, port):
    try:
        logging.warning("Trying to connect to the server.")
        sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        logging.warning("Socket created, connecting... " + str(sock))
        sock.connect((host, port))
        logging.warning("Socket issue check: " + str(sock))
        return sock
    except Exception:
        logging.critical("Server could not be reached!")
        print(
            "Couldn't connect to the server. Check your internet connection and try again.")
        sys.exit(0)


# if the player so chooses, reboots the game w/ another player waiting
def reboot_game_seq(gips, screen, pid):
    if gips.isEarlyExit != 0 and gips.is_win == 0:
        screen.update_actionbox("Other client D/Ced. Do you want to play another game? [Y/n]")
        screen.refresh_windows()
        return prompt_endgame(screen, gips)

    else:
        screen.update_actionbox("Game has ended! Do you want to play again? [Y/n] ")
        screen.refresh_windows()
        return prompt_endgame(screen, gips)

def prompt_endgame(screen, gips):
    c = chr(screen.stdscr.getch())
    if(c == 'y' or c == 'Y'):
        gips.sock.shutdown(socket.SHUT_RDWR)
        gips.sock.close()
        return True
    else:
        screen.update_actionbox("Are you sure you want to quit? [Y/n]")
        c = chr(screen.stdscr.getch())
        if(c == 'y' or c == 'Y'):
            screen.halt()
            print("Thanks for playing!!!")
            return False
        else:
            gips.sock.shutdown(socket.SHUT_RDWR)
            gips.sock.close()
            return True

def end_game(gips, screen, pid):
    if gips.is_win == pid:
        print("You Win! :-}")
    else:
        print("You Lost! :-{")
    print("Your new stats are:")
    screen.player.recv_player(gips.sock)
    screen.player.print_player()
    gips.sock.shutdown(socket.SHUT_RDWR)
    gips.sock.close()
    sys.exit(0)


def game_loop(board, pid, screen, gips):
    game_running = True
    while gips.is_win == 0 and gips.isEarlyExit == 0 and game_running:
        logging.debug("Starting the loop.")
        gips.recv()

        if gips.is_win != 0 or gips.isEarlyExit != 0:
            return gips
        elif gips.move_a == -1 and gips.move_b == -1:
            pass
        else:  # update board, get move
            board = update_board(gips, board)

        display_board(board, screen)
        screen.update_actionbox("Now you can move!")
        screen.refresh_windows()
        actions_taken = False
        while actions_taken == False:
            actions_taken = check_keys(screen, gips, board, pid)
            screen.update_actionbox("You still have actions left")
        screen.update_actionbox("Wait your turn. Be Respectful A+ ; ;))")
        screen.refresh_windows()
        is_win = gips.recv_iswin()
        gips.is_win = is_win
    return gips


def check_keys(screen, gips, board, pid):
    c = screen.stdscr.getch()
    logging.debug("checkKeys")
    if c == ord('q'):
        return False  # effectively pass your move
    if c == ord('m'):
        move(screen, gips, board, pid)
        return True
    if c == ord('c'):
        screen.update_actionbox("Enter your chat message")
        chat(screen, gips)
        screen.win4.clear()
        return False
    else:
        return False

def move(screen, gips, board, pid):
    screen.update_actionbox("Now you can move!")
    done = False
    move_v = []
    while not done:
        screen.win1.clear()
        logging.debug("Key: m")
        # Get the next move_v and send it.
        screen.game.edit()
        stuff = screen.game.gather()
        # Split the move_v into two components.
        if len(stuff) == 0:
            done = False
            continue
        move_v = (str(stuff)).split(' ')
        move_len = len(move_v)
        for x in range(0, move_len):
            if move_v[x] == '\n':
                move_v.remove(move_v[x])

        if not move_is_valid(move_v):
            screen.update_actionbox("Your move is not valid!!, move again")
            done = False
            continue
        done = True
    screen.win1.clear()
    # subtract 1 from moves
    move_v = list(map(int, move_v))
    move_v[0] -= 1
    move_v[1] -= 1
    # Otherwise:
    # Encode a GIPS
    gips.pack(pid, gips.is_win, move_v[0], move_v[1], 0)
    # Send the GIPS
    gips.send()
    board = update_board(gips, board)
    display_board(board, screen)
    # moves = []


def move_is_valid(move):
    try:
        move = list(map(int, move))
    except ValueError:
        return False
    logging.debug("Move: " + str(move))
    if 9 > move[0] > 0:
        if 9 > move[1] > 0:
            logging.debug("Valid")
            return True
        else:
            logging.debug("Invalid")
            return False
    else:
        logging.debug("Invalid.")
        return False

def chat(screen, gips):
    screen.board_mesg.edit()
    stuff = screen.board_mesg.gather()
    message = '\v' + str(len(str(gips.upid))) + str(gips.upid) + str(stuff)
    # Send message to the server as a bytestring.
    gips.sock.send(bytes(message, 'utf-8'))
    screen.refresh_windows()
    #the server should immediately send back a message if we send it
    gips.chat.recv_msg()

    return


def login(sock, upid, username):
    upid = int(upid)
    sock.send(struct.pack('!I', upid))
    send_string(sock, username)
    upid = sock.recv(4)
    upid = struct.unpack('@I', upid)
    upid = ntohl(upid[0])
    logging.debug(str(upid))
    return upid


def send_string(sock, string):
    string = bytes(string.encode('utf-8'))
    sent = sock.send(string)
    if sent != (len(string)):
        logging.warning("Bytes sent DID NOT MATCH")


def update_board(gips, board):
    logging.debug("Updating to the next board.")
    logging.debug("move_x: " + str(gips.move_a))
    logging.debug("move_y: " + str(gips.move_b))
    # because of the way curses displays the board
    # board[b][a] so not inverted
    if gips.pid == 1:
        board[int(gips.move_b)][int(gips.move_a)] = 'B'
    elif gips.pid == 2:
        board[int(gips.move_b)][int(gips.move_a)] = 'W'
    logging.debug("Returning updated board.")
    return board


def display_board(board, screen):
    x = 1
    y = 1
    logging.debug("board")
    for a in board:
        for b in a:
            screen.win3.addch(y, x, ord(b))
            y += 2
        y = 1
        x += 4
    screen.refresh_windows()

# noinspection PyUnusedLocal
def init_board():
    """
>>> board = init_board()
>>> board[0][0]
    'o'
    """
    return [['o' for x in range(8)] for y in range(8)]


def down(stdscr):
    logging.debug("Breaking down the application.")
    curses.nocbreak()
    stdscr.keypad(False)
    curses.echo()
    curses.endwin()
    import doctest
    doctest.testmod()


if __name__ == "__main__":
    main()
