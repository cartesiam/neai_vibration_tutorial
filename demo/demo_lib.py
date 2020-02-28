import argparse
import serial
import time
import sys
import pygame
from pygame.locals import *

# Initialization of pygame
pygame.init()

# Define screen and font
screen = pygame.display.set_mode((1280, 720))
info_screen = pygame.display.Info()
myfont = pygame.font.Font("Gelion-Regular.ttf", 36)

# Define anomaly threshold
ALARM_THRESHOLD = 190.0

# Functions
# Read argument for serial port
def define_args():
    parser = argparse.ArgumentParser()
    parser.add_argument('--port', type=str, required=True)
    return parser.parse_args()

# Define exit events
def exit_event():
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            pygame.quit()
            exit()
        elif event.type == KEYDOWN:
            if event.key == K_ESCAPE:
                pygame.quit()
                exit()

# Print learning progress bar
def draw_learning_bar(value):
    rect_width = 4.4 * (value + 1)
    pygame.draw.rect(screen,(162,208,218),(740,200,440,65))
    if value != 0:
        pygame.draw.rect(screen,(0,148,197),(740,200,rect_width,65))
    pygame.draw.circle(screen, (162,208,218), (886,200), 3)
    pygame.draw.circle(screen, (162,208,218), (886,265), 3)
    pygame.draw.circle(screen, (162,208,218), (1033,200), 3)
    pygame.draw.circle(screen, (162,208,218), (1033,265), 3)
    pygame.display.flip()

# Print bar of similarity in detection process
def draw_detect_bar(value):
    rect_width = 4.4 * (ALARM_THRESHOLD - 100)
    pygame.draw.rect(screen,(200,230,200),(740,200,440,65))
    pygame.draw.rect(screen,(230,200,200),(740,200,rect_width,65))
    rect_width = 4.4 * (value - 100)
    if value > ALARM_THRESHOLD:
        pygame.draw.rect(screen,(0,220,0),(740,200,rect_width,65))
    if value <= ALARM_THRESHOLD:
        pygame.draw.rect(screen,(220,0,0),(740,200,rect_width,65))
    pygame.display.flip()

# Status screen
def draw_status(value):
    if value >= 100 and value <= ALARM_THRESHOLD:
        image_background = pygame.image.load("background_postit.png").convert_alpha()
        label = myfont.render("Anomaly detected !", 1, (0,0,0))
        screen.blit(image_background, (0,0))
        screen.blit(label, (756, 100))
        draw_detect_bar(value)
        pygame.display.flip()
        pygame.draw.circle(screen, Color('green'), (1124,526), 6)
        pygame.display.flip()
        time.sleep(0.15)
        screen.blit(image_background, (0,0))
        screen.blit(label, (756, 100))
        draw_detect_bar(value)
        pygame.display.flip()
    if value > ALARM_THRESHOLD:
        image_background = pygame.image.load("background_on.png").convert_alpha()
        label = myfont.render("Usual behaviour", 1, (0,0,0))
        screen.blit(image_background, (0,0))
        screen.blit(label, (756, 100))
        draw_detect_bar(value)        
        pygame.display.flip()
    if value > 0 and value < 100:
        image_background = pygame.image.load("background_on.png").convert_alpha()
        label1 = myfont.render("Embedded learning", 1, (0,0,0))
        label2 = myfont.render("in progress...", 1, (0,0,0))
        screen.blit(image_background, (0,0))
        screen.blit(label1, (756, 70))
        screen.blit(label2, (756, 120))
        draw_learning_bar(value)
        pygame.display.flip()

# The application entry point
def main():
    # First screen
    image_background = pygame.image.load("background_off.png").convert_alpha()
    label1 = myfont.render("Your smart device is ready !", 1, (0,0,0))
    label2 = myfont.render("Press User button to start.", 1, (0,0,0))
    screen.blit(image_background, (0,0))
    screen.blit(label1, (750, 70))
    screen.blit(label2, (750, 120))
    pygame.display.set_caption("Cartesiam")
    pygame.display.flip()

    # Connection serial
    args = define_args()
    param_dict = args.__dict__
    serial_port = param_dict['port']
    ser = serial.Serial(serial_port, 115200, timeout=1)
    serial_output = False

    # Main loop
    while True:
        exit_event()
        try:
            line = ser.readline().decode('utf-8').rstrip()
            line = float(line)
            draw_status(line)
            serial_output = True
        except ValueError:
            if serial_output:
                label1 = myfont.render("Connection with Serial lost !", 1, (0,0,0))
                label2 = myfont.render("Press User button to restart.", 1, (0,0,0))
            else:
                label1 = myfont.render("Your smart device is ready !", 1, (0,0,0))
                label2 = myfont.render("Press User button to start.", 1, (0,0,0))
            image_background = pygame.image.load("background_off.png").convert_alpha()
            screen.blit(image_background, (0,0))                
            screen.blit(label1, (750, 70))
            screen.blit(label2, (750, 120))
            pygame.draw.circle(screen, Color('navy'), (1063,478), 8)
            pygame.display.flip()
            time.sleep(0.25)
            screen.blit(image_background, (0,0))
            screen.blit(label1, (750, 70))
            screen.blit(label2, (750, 120))                
            pygame.display.flip()

#-------------------------------------------------------------------------------
# Execution from shell
if __name__ == "__main__":
    sys.exit(main())
