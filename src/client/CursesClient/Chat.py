import threading
import time


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
