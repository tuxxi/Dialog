from __future__ import print_function
import logging
import time
import os
import signal
import serial
from threading import Thread, Event
from respeaker import Microphone
from azure_api import BingSpeechAPI


# get a key from https://www.microsoft.com/cognitive-services/en-us/speech-api
BING_KEY = open("../key").read().rstrip()
TTY_FILE = "/dev/ttyUSB0"
SER = serial.Serial(TTY_FILE, 115200)

def task(quit_event):
    mic = Microphone(quit_event=quit_event)
    bing = BingSpeechAPI(key=BING_KEY)

    while not quit_event.is_set():
        print("\nListening ... ")
        data = mic.listen()
        try:
            print("\nSending to buzzword AI ML MODEL")
            text = bing.recognize(data)
            if text:
                print("\nWriting {}".format(text))
                print_to_display(text)
        except Exception as e:
            print(e.message)


def main():
    logging.basicConfig(level=logging.DEBUG)
    quit_event = Event()

    def signal_handler(sig, frame):
        quit_event.set()
        print('quit')
    signal.signal(signal.SIGINT, signal_handler)

    thread = Thread(target=task, args=(quit_event,))
    thread.daemon = True
    thread.start()
    while not quit_event.is_set():
        time.sleep(1)

    time.sleep(1)

def print_to_display(text):
    # a = 'stty 115200 -F {tty} -hupcl; echo "{text}" > {tty}'.format(text=text, tty=TTY_FILE)
    # print("Attempting to write: ", a)
    # os.system(a)
    SER.write("{}\r\n".format(text))

if __name__ == '__main__':
    main()
