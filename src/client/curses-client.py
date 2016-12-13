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
    def __init__(self, name, upid, wins, losses, ties):
        self.name = name
        self.upid = upid
        self.wins = wins
        self.losses = losses
        self.ties = ties

    def print_player(self):
        print(str(self.name) + ' UPID: ' + str(self.upid) + ' has ' + str(self.wins) + ' wins ' + str(
            self.losses) + ' losses ' + ' and ' + str(self.ties) + ' ties.')

    def recv_player(self, sock):
        player = sock.recv(40)
        player = struct.unpack('!IccccccccccccccccccccIIII', player)
        self.upid = int(player[0])
        self.name = ''
        for x in range(1, 21):
            self.name += player[x].decode("utf-8")
        self.wins = int(player[21])
        self.losses = int(player[22])
        self.ties = int(player[23])

    def update_win(self, screen):
        screen.win5.clear()
        screen.win5.addstr(0, 0, "Player: " + str(self.name))
        screen.win5.addstr(1, 0, "Unique PID: " + str(self.upid))
        screen.win5.addstr(2, 0, "Wins: " + str(self.wins))
        screen.win5.addstr(3, 0, "Losses: " + str(self.losses))
        screen.win5.addstr(4, 0, "Ties:" + str(self.ties))
        screen.win5.refresh()

class Chat(threading.Thread):
    def __init__(self, win, sock):
        # noinspection PyArgumentList
        super().__init__()
        self.sock = sock
        self.win = win
        self.row = 0
        self.col = 0

    def update(self):
        # Get a chat message.
        msg = self.sock.recv(1024)
        # This next bit just implements some kind of half-working auto-scrolling
        if self.row >= 40:
            self.win.clear()
            self.row = 0
        self.win.addstr(self.row, self.col, msg)
        self.row += 1
        # Add the chat message to win.
        self.win.refresh()

    def run(self):
        while True:
            time.sleep(1)
            self.update()


# define screen variables

class Screen(object):
    def __init__(self, height, width, one_begin_x,
                 one_begin_y, two_begin_x, two_begin_y,
                 thr_begin_x, thr_begin_y, player):
        # init for screen in main
        # screen = Screen(40, 40, 1, 15, 70, 15, 121, 15, player)

        self.stdscr = self.initialize()  # Starts the Curses application.
        # Window 1 takes commands for the game.
        self.win1 = curses.newwin(height, width, one_begin_y, one_begin_x)
        # self.win1_sub = self.win1.derwin(1, 1)
        # self.win1.box()

        # Window 2 carries the chat.
        self.win2 = curses.newwin(((3 * height) // 4), width, two_begin_y, two_begin_x)
        # self.win2_sub = self.win2.derwin(2, 1)
        # self.win2.box()

        # Window 3 displays the current game board.
        self.win3 = curses.newwin(66, 66, thr_begin_y, thr_begin_x)
        # self.win3_sub = self.win2.derwin(2, 1)
        # self.win3.box()

        # Window 4 displays the message that the player is currently typing out.
        self.win4 = curses.newwin((height // 4), width,
                                  (two_begin_y + ((3 * height) // 4)), two_begin_x)
        # self.win4_sub = self.win4.derwin(2, 1)
        # self.win4.box()

        #win5 is the textbox for player stats
        self.win5 = curses.newwin(10, 16, 0, 70)

        # takes messages for the game
        self.game = Textbox(self.win1)
        # takes messages for current gameboard
        self.board_mesg = Textbox(self.win4)
        # commands for game
        self.chat = Chat(self.win2, self.stdscr)

        self.player = player
    @staticmethod
    def initialize():
        stdscr = curses.initscr()
        stdscr.clear()
        curses.noecho()
        curses.cbreak()
        stdscr.keypad(True)
        return stdscr

    def halt(self):  # Just to cut down on a few lines in main()
        down(self.stdscr)

    #75-> that way
    #12 down
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
        self.stdscr.addstr(14, 70, "Chat")
        self.stdscr.addstr(14, 1, "Game Window")
        self.stdscr.addstr(14, 120, "The Board")


class GIPS(object):
    def __init__(self, sock):
        logging.debug("GIPS.sock is being defined.")
        self.sock = sock
        logging.debug(self.sock)
        self.pid = 0
        self.is_win = 0
        self.move_a = -1
        self.move_b = -1
        self.isEarlyExit = 0

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

    player = Player(username, upid, 0, 0, 0)

    # height/width/one_begin_x/one_begin_y/etc
    # GOOD UP TO HERE (With send/recv)
    screen = Screen(40, 40, 1, 15, 70, 15, 121, 15, player)
    screen.print_title()
    screen.player.update_win(screen)
    logging.debug("Game starting.")
    screen.stdscr.addstr(5, 70, "The game will be starting shortly...", curses.A_BLINK | curses.A_BOLD | curses.COLOR_RED)
    screen.stdscr.refresh()
    gips = GIPS
    try:
        logging.debug("The GIPS is defined.")
        keep_playing = True
        while keep_playing:
            sock = establish_connection(host, port)
            gips = GIPS(sock)
            login(sock, upid, username)

            screen.stdscr.clear()
            screen.print_title()
            screen.stdscr.refresh()

            pid = ord(sock.recv(1))
            screen.player.recv_player(sock)
            screen.player.update_win(screen)
            board = init_board()
            gips = game_loop(board, pid, username, screen, gips)
            if gips.isEarlyExit == 1 and gips.is_win == 0:
                screen.halt()
                print("Thanks for playing!!!")
                gips.sock.shutdown(socket.SHUT_RDWR)
                gips.sock.close()
                sys.exit(0)
            else:
                screen.halt()
                print("Thanks for playing!!!")
                # end sequence
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
        print("Couldn't connect to the server. Check your internet connection and try again.")
        sys.exit(0)


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


def game_loop(board, pid, username, screen, gips):
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

        screen.stdscr.refresh()  # This line begins the interface logic.
        display_board(board, screen.win3)
        screen.stdscr.refresh()  # This begins the user interaction
        c = screen.stdscr.getch()
        game_running = check_keys(c, screen, gips, board, pid, username)
        is_win = gips.sock.recv(4)
        is_win = struct.unpack('!i', is_win)
        is_win = ntohl(is_win[0])
        gips.is_win = is_win
    return gips


def check_keys(c, screen, gips, board, pid, username):
    logging.debug("checkKeys")
    if c == ord('q'):
        logging.debug("Key: q")
        return False
    if c == ord('m'):
        done = False
        while not done:
            screen.win1.clear()
            logging.debug("Key: m")
            # Get the next move and send it.
            screen.game.edit()
            stuff = screen.game.gather()
            # Split the move into two components.
            if len(stuff) == 0:
                done = False
                continue
            move = (str(stuff)).split(' ')
            
            move.remove('\n')  # kill the newline=
            # for m in move:
            #    m = int(m)
            # If the move is not valid:
            # make moves ints
            move = list(map(int, move))
            if not move_is_valid(move):
                send_to_chat(gips.sock, "server: Invalid move, " + str(username) + "!")
                done = False
                continue
            done = True
        screen.win1.clear()
        # subtract 1 from moves
        move[0] -= 1
        move[1] -= 1
        # Otherwise:
        # Encode a GIPS
        gips.pack(pid, gips.is_win, move[0], move[1], 0)
        # Send the GIPS
        gips.send()
        board = update_board(gips, board)
        display_board(board, screen.win3)
        # moves = []
        return True
    if c == ord('c'):
        screen.board_mesg.edit()
        stuff = screen.board_mesg.gather()
        message = "\v" + str(username) + ": " + str(stuff)
        # Send message to the server as a bytestring.
        send_to_chat(gips.sock, message)
        screen.stdscr.refresh()  # Redraws the screen.
        return True


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
    string = bytes(string, 'utf-8')
    sent = sock.send(string)
    if sent != (len(string)):
        logging.warning("Bytes sent DID NOT MATCH")


def send_to_chat(sock, message):
    logging.debug(str(message) + " to chat")
    sock.send(bytes(message, 'utf-8'))


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


def move_is_valid(move):
    logging.debug("Move: " + str(move))
    if 9 > int(move[0]) > 0:
        if 9 > int(move[1]) > 0:
            logging.debug("Valid")
            return True
        else:
            logging.debug("Invalid")
            return False
    else:
        logging.debug("Invalid.")
        return False


def display_board(board, win):
    x = 1
    y = 1
    logging.debug("board")
    for a in board:
        for b in a:
            win.addch(y, x, ord(b))
            y += 2
        y = 1
        x += 4
    win.refresh()


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
