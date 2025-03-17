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
        # I²C device address (commonly 0x27 or 0x3F for your LCD backpack)
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
        
        try:
            # Initialize I²C (SMBus)
            self.bus = smbus.SMBus(self.LCD_BUS)
            self._lcd_init()
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
    def lcd_string(self, message, line) -> None:
        """Send string to display"""
        message = message.ljust(self.LCD_WIDTH, " ")
        self._lcd_byte(line, self.LCD_CMD)
        for char in message:
            self._lcd_byte(ord(char), self.LCD_CHR)
    
    # MARK: test
    def test(self) -> None:
        try:
            while True:
                self.lcd_string("Hello, World!", self.LCD_LINE_1)
                self.lcd_string("Raspberry Pi 4", self.LCD_LINE_2)
                sleep(3)
                
                self.lcd_string("I2C LCD Example", self.LCD_LINE_1)
                self.lcd_string("Keep Coding!", self.LCD_LINE_2)
                sleep(3)
        
        except KeyboardInterrupt:
            # Clear display on Ctrl+C exit
            self._lcd_byte(0x01, self.LCD_CMD)

if __name__ == '__main__':
    Display = LCDDisplay()
    Display.test()
