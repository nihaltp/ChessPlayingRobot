"""
    Code for reading from Arduino uno
    
    Reed switch code that goes through all 64 sensors placed in a 8x8 matrix
    Using 4 x 74HC4067 multiplexers
    one for two rows since they have 16 input pins
    the other end of each row provides GND and one takes signal
    
    The output from the arduino will be a 8 x 8 matrix or 1s and 0s
    display them on pygame window
    
    Made by: NIHAL T P
    GitHub: https://github.com/nihaltp
    LinkedIn: https://www.linkedin.com/in/nihal-tp
    
    Circuit:
    
     V_in
      |
      R1 (2kΩ)
      |
      -----> V_out (to ESP8266 A0)
      |
      R2 (1kΩ)
      |
     GND
    
"""

import os

# Hide the Pygame support prompt
os.environ["PYGAME_HIDE_SUPPORT_PROMPT"] = "1"

import sys
import serial
import pygame
# from time import sleep

import random # TODO: Remove this line

# MARK: sensorDisplay
class sensorDisplay:
    def __init__(self):
        try:
            pygame.init() # Initialize Pygame
        except pygame.error as e:
            print(f"Error initializing Pygame: {e}")
            print("Please install pygame and try again.")
            sys.exit(1)
        
        
        self.sensorsize = 8
        self.square_width = 50
        self.screen_height = self.square_width * self.sensorsize + 2 * self.square_width
        self.screen_width = self.screen_height
        
        # create a list of 64 0s
        self.values: list = [0 for _ in range(self.sensorsize * self.sensorsize)]
        
        self.background = (135, 206, 250)
        self.board_border = (0, 0, 0)
        self.yellow = (255, 255, 0)
        self.white = (255, 255, 255)
        self.chess_x = (self.screen_width - self.sensorsize * self.square_width) / 2
        self.chess_y = (self.screen_height - self.sensorsize * self.square_width) / 2
        
        # Create the Pygame window
        self.screen = pygame.display.set_mode((self.screen_width, self.screen_height))
        pygame.display.set_caption("Sensor Display")
        
        # Pygame font setup
        pygame.font.init()
        self.font = pygame.font.SysFont(None, 28)
        
        # TODO : Update the serial connection
        # Set up the serial connection (replace with your serial port)
        self.arduino_port = 'COM6'  # '/dev/ttyACM0' or '/dev/ttyUSB0' or 'COM6'
        self.baud_rate = 9600       # 9600 or 115200
        
        self.showSensor()
    
    # MARK: showSensor
    def showSensor(self):
        while True:
            self.drawBoard()
            self.handleEvents()
        self.quitGame()
    
    # MARK: drawBoard
    def drawBoard(self):
        """Draw the Sensor Values on the screen."""
        self.screen.fill(self.background)
        square_rect = pygame.Rect(self.chess_x, self.chess_y, self.square_width, self.square_width)
        pygame.draw.rect(self.screen, self.board_border, square_rect, width = 2 )
        
        self.getValues()
        
        # Draw each square independently
        for row in range(self.sensorsize):
            for column in range(self.sensorsize):
                self.drawSquare(row, column, self.values[( ( row * 8 ) + column )])
        pygame.display.flip()
    
    # MARK: getValues
    def getValues(self):
        # This function should return the Sensor values as a list of 8x8 values
        self.values = self.readSerialData()
        # self.values = [random.randint(0,1) for _ in range(64)]
        # sleep(0.1)
    
    # MARK: readSerialData
    def readSerialData(self):
        # Read and return the 8x8 matrix via serial.
        matrix = []
        should_break = False
        
        try:
            with serial.Serial(self.arduino_port, self.baud_rate) as sensor:
                # sleep(2)
                while True:
                    # Read a line of data from the serial port
                    line = sensor.readline().decode('utf-8').strip()
                    
                    if not line:
                        # sleep(1)
                        continue
                    
                    if len(line.split(',')) != 8:
                        print(f"Skipping line: '{line}'")  # Debugging
                        print("Resetting Matrix")
                        matrix = []
                        continue
                    
                    # Check for the period at the end of the last line to stop reading
                    if line.endswith('.') or line == '.' or line == '':
                        # print(f"line is {line}")
                        should_break = True
                        line = line.replace('.', '')
                    
                    if self.checkLine(line):
                        should_break = False
                        matrix = []
                        continue
                    # else:
                    #     print(matrix)
                    
                    # Split the line by commas and convert to a list of integers
                    row = list(map(int, line.split(',')))
                    matrix.append(row)
                    # print(matrix)
                    
                    if should_break and len(matrix) == 8:
                        if self.checkMatrix(matrix):
                            should_break = False
                            matrix = []
                        else:
                            break
                    
                    elif should_break:
                        should_break = False
                        matrix = []
                    # print(matrix)
            
            return matrix
        
        except Exception as e:
            print(f"Error reading from serial: {e}")
    
    # MARK: checkLine
    def checkLine(self, line : str):
        try:
            # print(f"line: {line}")
            line_list : list = line.split(',')
            for x in range(len(line_list)):
                y = int(line_list[x])
                # check if y is a 0 or 1
                if y != 0 and y != 1:
                    print(f'Error in: "{line_list}"')
                    print(f"Error in [{x}]: {y}")
                    return True
            
            return False
        
        except Exception as e:
            print(f"Error here: {e}")
    
    # MARK: checkMatrix
    def checkMatrix(self, matrix : list):
        try:
            for x in range(len(matrix)):
                for y in range(len(matrix[x])):
                    # check if y is a 0 or 1
                    if matrix[x][y] != 0 and matrix[x][y] != 1:
                        print(f'Error in: "{matrix}"')
                        print(f"Error in [{x}][{y}]: {matrix[x][y]}")
                        return True
            
            return False
        
        except Exception as e:
            print(f"Error here: {e}")
    
    # MARK: drawSquare
    def drawSquare(self, row: int = 0, column: int = 0, value: int = 0):
        # Draw square
        color = self.yellow if value == 1 else self.white
        square_rect = pygame.Rect(self.chess_x + column * self.square_width, self.chess_y + row * self.square_width, self.square_width, self.square_width)
        pygame.draw.rect(self.screen, color, square_rect)
    
    # MARK: handleEvents
    def handleEvents(self):
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                self.quitGame()
    
    # MARK: quitGame
    def quitGame(self, status_code: int = 0):
        pygame.quit()
        sys.exit(status_code)

if __name__ == "__main__":
    sensorDisplay()
