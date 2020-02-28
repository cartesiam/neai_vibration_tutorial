import argparse
import json
import subprocess
import serial
import time
import sys
import pygame
from pygame.locals import *

# Initialization of pygame
pygame.init()

# Define screen and font
screen = pygame.display.set_mode((1280, 720))
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
    pygame.draw.rect(screen,(255,255,255),(735,55,465,235))
    if value >= 100 and value <= ALARM_THRESHOLD:
        image_background = pygame.image.load("background_postit.png").convert_alpha()
        label = myfont.render("Anomaly detected !", 1, (0,0,0))
        screen.blit(image_background, (0,0))
        screen.blit(label, (756, 100))
        draw_detect_bar(value)
        pygame.draw.circle(screen, Color('green'), (1124,526), 6)
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
    # Usefull variables for learning
    learning_cpt = 0
    learning_progress = 0
    learning_signals = 0
    learning_number = False

    # First screen
    image_background = pygame.image.load("background_off.png").convert_alpha()
    label1 = myfont.render("Your smart device is ready !", 1, (0,0,0))
    label2 = myfont.render("Press Reset button (black) then", 1, (0,0,0))
    label3 = myfont.render("User button (blue) to start.", 1, (0,0,0))
    screen.blit(image_background, (0,0))
    screen.blit(label1, (745, 80))
    screen.blit(label2, (725, 170))
    screen.blit(label3, (725, 220))
    pygame.display.set_caption("Cartesiam")
    pygame.display.flip()

    # Connection serial
    args = define_args()
    param_dict = args.__dict__
    serial_port = param_dict['port']
    ser = serial.Serial(serial_port, 115200, timeout=1)
    serial_output = False

    # NanoEdge AI initialization
    subprocess.run("./NanoEdgeAI_Emulator NanoEdgeAI_initialize", shell = True)

    # Main loop
    while True:
        exit_event()
        while learning_number == False:
            exit_event()            
            try:
                learning_signals_string = ser.readline().decode('utf-8').rstrip()
                learning_signals = float(learning_signals_string)
                learning_number = True
            except ValueError:
                Learning_number = False
   
        if learning_progress < 100:
            try:
                line = ser.readline().decode('utf-8').rstrip()
                message = subprocess.check_output("./NanoEdgeAI_Emulator NanoEdgeAI_learn --array "+ line, shell = True)
                status = json.loads(message)['status']
                learning_progress = learning_cpt * 100 / learning_signals
                learning_cpt = learning_cpt + 1
                draw_status(learning_progress - 1)
            except subprocess.CalledProcessError:
                label1 = myfont.render("Connection with Serial lost !", 1, (0,0,0))
                label2 = myfont.render("Press User button to restart.", 1, (0,0,0))
                image_background = pygame.image.load("background_off.png").convert_alpha()
                screen.blit(image_background, (0,0))                
                screen.blit(label1, (750, 70))
                screen.blit(label2, (750, 120))
                pygame.display.flip()
                learning_number = False
                learning_progress = 0
                learning_cpt = 0
            except ValueError:
                label1 = myfont.render("Connection with Serial lost !", 1, (0,0,0))
                label2 = myfont.render("Press User button to restart.", 1, (0,0,0))
                image_background = pygame.image.load("background_off.png").convert_alpha()
                screen.blit(image_background, (0,0))                
                screen.blit(label1, (750, 70))
                screen.blit(label2, (750, 120))
                pygame.display.flip()
                learning_number = False
                learning_progress = 0
                learning_cpt = 0
        else:            
            try:
                line = ser.readline().decode('utf-8').rstrip()
                message = subprocess.check_output("./NanoEdgeAI_Emulator NanoEdgeAI_detect --array "+ line, shell = True)                
                similarity = json.loads(message)['results'][0]['similarity']
                similarity = float(similarity + 100)
                draw_status(similarity)
            except subprocess.CalledProcessError:
                label1 = myfont.render("Connection with Serial lost !", 1, (0,0,0))
                label2 = myfont.render("Press User button to restart.", 1, (0,0,0))
                image_background = pygame.image.load("background_off.png").convert_alpha()
                screen.blit(image_background, (0,0))                
                screen.blit(label1, (750, 70))
                screen.blit(label2, (750, 120))                
                pygame.display.flip()
                learning_number = False
                learning_progress = 0
                learning_cpt = 0
            except ValueError:
                label1 = myfont.render("Connection with Serial lost !", 1, (0,0,0))
                label2 = myfont.render("Press User button to restart.", 1, (0,0,0))
                image_background = pygame.image.load("background_off.png").convert_alpha()
                screen.blit(image_background, (0,0))                
                screen.blit(label1, (750, 70))
                screen.blit(label2, (750, 120))
                pygame.display.flip()
                learning_number = False
                learning_progress = 0
                learning_cpt = 0

#-------------------------------------------------------------------------------
# Execution from shell
if __name__ == "__main__":
    sys.exit(main())
