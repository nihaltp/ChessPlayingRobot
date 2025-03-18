"""
    Test Code for checking LCD display using Raspberry
    
    Author: NIHAL T P
    GitHub: https://github.com/nihaltp
    LinkedIn: https://www.linkedin.com/in/nihal-tp
"""

import smbus
from time import sleep

# MARK: LCDDisplay
class LCDDisplay:
    def __init__(self, address=0x27, bus_num: int = 1, width: int = 16) -> None:
        """ Define some device parameters """
        # I²C device address (commonly 0x27 or 0x3F)
        # Use `i2cdetect -y 1` to find your LCD's I2C address
        self.LCD_ADDR = address
        
        self.LCD_BUS = bus_num    # Raspberry Pi 4 uses bus 1
        self.LCD_WIDTH = width    # Characters per line
        
        # Define some device constants
        self.LCD_CMD = 0          # Mode - Sending command
        self.LCD_CHR = 1          # Mode - Sending data
        
        self.LCD_LINE_1 = 0x80    # LCD RAM address for the 1st line
        self.LCD_LINE_2 = 0xC0    # LCD RAM address for the 2nd line
        
        self.LCD_BACKLIGHT = 0x08 # On
        self.ENABLE = 0b00000100  # Enable bit
        
        # Timing constants
        self.E_PULSE = 0.0005
        self.E_DELAY = 0.0005
        
        self.pieceNames = ["pawn", "rook", "knight", "bishop", "queen", "king"]
        
        try:
            # Initialize I²C (SMBus)
            self.bus = smbus.SMBus(self.LCD_BUS)
            self._lcd_init()
            self._lcd_char_init()
        except Exception as e:
            raise RuntimeError(f"I2C initialization failed: {str(e)}")
    
    # MARK: _lcd_init
    def _lcd_init(self) -> None:
        """Initialize display"""
        self._lcd_byte(0x33, self.LCD_CMD)  # Initialize
        self._lcd_byte(0x32, self.LCD_CMD)  # Set to 4-bit mode
        self._lcd_byte(0x06, self.LCD_CMD)  # Cursor move direction
        self._lcd_byte(0x0C, self.LCD_CMD)  # Turn cursor off and display on
        self._lcd_byte(0x28, self.LCD_CMD)  # 2 line display, 5x7 font
        self._lcd_byte(0x01, self.LCD_CMD)  # Clear display
        sleep(self.E_DELAY)
    
    # MARK: _lcd_byte
    def _lcd_byte(self, bits, mode: int) -> None:
        """ Send byte to LCD pins (split into two nibbles) """
        # High nibble
        bits_high = mode | (bits & 0xF0) | self.LCD_BACKLIGHT
        # Low nibble
        bits_low = mode | ((bits << 4) & 0xF0) | self.LCD_BACKLIGHT
        
        # Write high nibble
        self.bus.write_byte(self.LCD_ADDR, bits_high)
        self._toggle_enable(bits_high)
        
        # Write low nibble
        self.bus.write_byte(self.LCD_ADDR, bits_low)
        self._toggle_enable(bits_low)
    
    # MARK: _toggle_enable
    def _toggle_enable(self, bits) -> None:
        """Toggle enable pin on LCD"""
        sleep(self.E_DELAY)
        self.bus.write_byte(self.LCD_ADDR, (bits | self.ENABLE))
        sleep(self.E_PULSE)
        self.bus.write_byte(self.LCD_ADDR, (bits & ~self.ENABLE))
        sleep(self.E_DELAY)
    
    # MARK: lcd_string
    def lcd_string(self, message, line, clear: bool = True) -> None:
        """
        Send string to display
        :param message: string to display
        :param line: line to display on (1 or 2)
        :param clear: clear line before writing (default: True)
        """
        if clear:
            # Clear the screen or trim the message to LCD width
            message = message.ljust(self.LCD_WIDTH, " ")[:self.LCD_WIDTH]
        else:
            # Only trim the message if it is too long; don't clear the screen.
            message = message[:self.LCD_WIDTH]
        
        self._lcd_byte(line, self.LCD_CMD)
        for char in message:
            self._lcd_byte(ord(char), self.LCD_CHR)
    
    # MARK: lcd_create_char
    def lcd_create_char(self, location: int, charmap: list) -> None:
        """
        Create a custom character.
        'location' should be 0-7.
        'charmap' is a list of 8 integers, each representing a row.
        """
        location &= 0x7  # Only 0-7 are valid
        # Set CGRAM address: 0x40 + (location << 3)
        self._lcd_byte(0x40 | (location << 3), self.LCD_CMD)
        for i in range(8):
            self._lcd_byte(charmap[i], self.LCD_CHR)
    
    # MARK: _lcd_char_init
    def _lcd_char_init(self) -> None:
        """ Initialize the LCD character set """
        # Example custom characters (chess pieces)
        pawn   = [0b00100, 0b01110, 0b00100, 0b00100, 0b00100, 0b01110, 0b11111, 0b00000]
        rook   = [0b10101, 0b10101, 0b11111, 0b01110, 0b01110, 0b11111, 0b11111, 0b00000]
        knight = [0b00000, 0b01100, 0b01110, 0b01111, 0b01110, 0b01100, 0b01000, 0b11111]
        bishop = [0b00100, 0b01010, 0b00100, 0b01010, 0b10001, 0b10001, 0b01110, 0b00000]
        queen  = [0b01010, 0b11111, 0b01010, 0b11111, 0b10101, 0b11111, 0b11111, 0b00000]
        king   = [0b00100, 0b01010, 0b00100, 0b01110, 0b10101, 0b11111, 0b11111, 0b00000]
        
        # Create custom characters
        self.lcd_create_char(0, pawn)
        self.lcd_create_char(1, rook)
        self.lcd_create_char(2, knight)
        self.lcd_create_char(3, bishop)
        self.lcd_create_char(4, queen)
        self.lcd_create_char(5, king)
    
    # MARK: test
    def test(self) -> None:
        """
        Updated test routine:
        Cycles through chess pieces every 5 seconds. For each cycle, it displays:
          - A line with "Piece: <name>" on the first line.
          - Clears any extra characters (using lcd_string's padding).
          - Displays the custom character corresponding to the piece at column 15.
        """
        # Variables for the timing and piece rotation logic
        pieceIndex = 0
        
        try:
            while True:
                # Update piece index
                pieceIndex = (pieceIndex + 1) % 6
                
                # Prepare and display the message on the first line.
                line = f"Piece: {self.pieceNames[pieceIndex]}"
                self.lcd_string(line, self.LCD_LINE_1)
                
                # Clear remaining characters up to column 14 is handled by lcd_string's ljust.
                # Now display the custom character at column 15 (16th position) of the first line.
                # Set cursor to column 15 of line 1:
                self._lcd_byte(self.LCD_LINE_1 + 15, self.LCD_CMD)
                # Display the custom character (the chess piece) stored in CGRAM at location pieceIndex:
                self._lcd_byte(pieceIndex, self.LCD_CHR)
                
                sleep(5)
        
        except KeyboardInterrupt:
            # Clear display on Ctrl+C exit
            self._lcd_byte(0x01, self.LCD_CMD)

if __name__ == '__main__':
    try:
        Display = LCDDisplay()
        Display.test()
    except KeyboardInterrupt:
        # In case of an interrupt, clear LCD display.
        Display._lcd_byte(0x01, Display.LCD_CMD)
        print("\033[91mUser abort\033[0m")
