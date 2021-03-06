#!/usr/bin/env python3

import curses
import logging
import random
import socket
import struct
import sys
from socket import ntohl

from CursesClient import GIPS, Screen, Chat, Player


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
    chat_v = object
    pid = ''
    # screen = Screen(40, 40, 43, 120, 15, 72, 15, 121, player, chat)
    # Y (first) DOWN
    # X (sec) across -> that way
    # create player objects and windows
    player = Player(username, upid, 0, 0, 0, chat)
    player2 = Player("", 0, 0, 0, 0, chat)
    screen = Screen(chat)
    player.win = screen.windows["player_stats"]
    player2.win = screen.windows["player2_stats"]

    logging.debug("Game starting.")
    gips = GIPS
    try:
        keep_playing = True
        while keep_playing:
            screen.stdscr.clear()
            screen.windows["actionbox"].clear()
            screen.print_title()
            screen.stdscr.addstr(6, 70, "The game will be starting shortly....",
                                 curses.A_BLINK | curses.A_BOLD | curses.COLOR_RED)
            player.update_pwin()
            player2.update_pwin()
            screen.refresh_windows()
            sock = establish_connection(host, port)
            if(not sock):
                hard_shutdown(gips, screen)
            gips = GIPS(sock, chat_v)
            upid = login(sock, upid, username)
            gips.upid = upid
            # init chat_v thread
            chat_v = Chat(screen.windows["chat"], sock)
            init_chat(chat_v, screen, gips)
            screen.stdscr.clear()
            pid = gips.recv_pid()
            player.recv_player(sock)
            player2.recv_player(sock)
            screen.refresh_windows()
            player.update_pwin()
            player2.update_pwin()
            board = init_board()
            gips = game_loop(board, pid, screen, gips)
            # TODO : FIX WINDOWS, DEFINITELY OVERLAPPING GOING ON

            screen.refresh_windows()
            player.update_pwin()
            player2.update_pwin()
            keep_playing = reboot_game_seq(gips, screen)
        # end the game once the while loop dies
        end_game(gips, screen, pid)

    except Exception:
        logging.exception("Exception caught")
        hard_shutdown(gips, screen)
    except KeyboardInterrupt:
        logging.exception("SIGINT received")
        hard_shutdown(gips, screen)


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
        print("Couldn't connect to the server. Check your internet connection and try again.")
        logging.critical("Server could not be reached!")
        return False

# if the player so chooses, reboots the game w/ another player waiting
def reboot_game_seq(gips, screen):
    if gips.is_early_exit != 0 and gips.is_win == 0:
        screen.update_actionbox("Other client D/Ced. Do you want to play another game? [Y/n]")
        screen.refresh_windows()
        return prompt_endgame(screen, gips)

    else:
        screen.update_actionbox("Game has ended! Do you want to play again? [Y/n] ")
        screen.refresh_windows()
        return prompt_endgame(screen, gips)


def prompt_endgame(screen, gips):
    c = chr(screen.stdscr.getch())
    if c in ('y', 'Y'):
        gips.sock.shutdown(socket.SHUT_RDWR)
        gips.sock.close()
        return True
    else:
        screen.update_actionbox("Are you sure you want to quit? [Y/n]")
        c = chr(screen.stdscr.getch())
        if c in ('y', 'Y'):
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
    hard_shutdown(gips, screen)

def hard_shutdown(gips, screen):
    if(gips is not None and hasattr(gips, 'sock')):
        gips.sock.shutdown(socket.SHUT_RDWR)
        gips.sock.close()
    down(screen.stdscr)
    sys.exit(0)

def game_loop(board, pid, screen, gips):
    game_running = True
    while gips.is_win == 0 and gips.is_early_exit == 0 and game_running:
        logging.debug("Starting the loop.")
        gips.recv()

        if gips.is_win != 0 or gips.is_early_exit != 0:
            return gips
        elif gips.move_a == -1 and gips.move_b == -1:
            pass
        else:  # update board, get move
            board = update_board(gips, board)

        display_board(board, screen)
        screen.update_actionbox("Now you can move!")
        screen.refresh_windows()
        actions_taken = False
        while actions_taken is False:
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
        end_game(screen, gips, pid)
    if c == ord('m'):
        move(screen, gips, board, pid)
        return True
    if c == ord('c'):
        screen.update_actionbox("Enter your chat message")
        chat(screen, gips)
        screen.windows["current_message"].clear()
        return False
    else:
        return False


def move(screen, gips, board, pid):
    screen.update_actionbox("Now you can move!")
    done = False
    move_v = []
    while not done:
        screen.windows["game_commands"].clear()
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

        if not move_is_valid(move_v, board):
            screen.update_actionbox("Your move is not valid!!, move again")
            done = False
            continue
        done = True
    screen.windows["game_commands"].clear()
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


def move_is_valid(move_v, board):
    valid = False
    try:
        move_v = list(map(int, move_v))
    except ValueError:
        return False
    logging.debug("Move: " + str(move_v))
    if 9 > move_v[0] > 0:
        if 9 > move_v[1] > 0:
            logging.debug("Valid")
            valid = True
        else:
            logging.debug("Invalid")
            valid = False
    else:
        logging.debug("Invalid.")
        valid = False
    if board[move_v[0]-1][move_v[1]-1] is not "o":
        logging.debug("Invalid.")
        valid = False
    return valid


def chat(screen, gips):
    screen.board_mesg.edit()
    stuff = screen.board_mesg.gather()
    logging.debug(stuff)
    message = '\v' + str(len(str(gips.upid))) + str(gips.upid) + str(stuff)
    # Send message to the server as a bytestring.
    gips.sock.send(bytes(message.encode("utf-8")))
    screen.refresh_windows()
    # the server should immediately send back a message if we send it
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
            screen.windows["board"].addch(y, x, ord(b))
            y += 2
        y = 1
        x += 4
    screen.refresh_windows()


# noinspection PyUnusedLocal
def init_board():
    return [['o' for x in range(8)] for y in range(8)]


def down(stdscr):
    logging.debug("Breaking down the application.")
    curses.nocbreak()
    stdscr.keypad(False)
    curses.echo()
    curses.endwin()
    # import doctest
    # doctest.testmod()


if __name__ == "__main__":
    main()
