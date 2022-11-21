# MiniDisplay_UI

Work in progress...

Allows the display and editing of parameters organized as pages on OLED or LCD screens communicating via an i2C bus.
Navigation between pages and parameters is provided by either 3 digital inputs (reading only) or 5 digital inputs (reading / editing).
Supported platform: Bela.
- Uses the library [u8g2](https://github.com/olikraus/u8g2) adapted by [Giulio Moro to be usable on Bela](https://github.com/giuliomoro/O2O)
- Example of a page displayed on an OLED screen:
<picture>
  <source media="(prefers-color-scheme: dark)" srcset="https://github.com/PascalFaivre/MiniDisplay_UI/blob/master/doc/exscreen.jpg">
  <source media="(prefers-color-scheme: light)" srcset="https://github.com/PascalFaivre/MiniDisplay_UI/blob/master/doc/exscreen.jpg">
  <img alt="Example of an OLED screen page" src="https://github.com/PascalFaivre/MiniDisplay_UI/blob/master/doc/exscreen.jpg">
</picture>


##Features:
- the dimensions of the screen are configurable (in characters) (the size of the font is determined by this parameter)
- adding/removing pages
- adding/removing parameters. The value of a parameter is of type double. It can be displayed as decimal, integer or boolean
- navigation with 3 keys in reading and with 5 keys in reading and editing 
- vertical scrolling for pages with a lot of parameters
- editing the parameter being selected via the navigation interface
- possible binding of a parameter to an external value via the provision of a pointer on it to the creation of the parameter
- dynamic management of the refresh of the screen (is refreshed only when changes have taken place on the display window (value, selection, page, edit))
    

## Usage :
    see sample : render.cpp (allows you to see the different features)


## TODO :
- deletion functions (pages and parameters)
- type of page: Information (no parameters but just lines of text)
- type of parameter: drop-down list
- page type: Boot (self-defined page by a file found in the project folder of bela containing the different bootable projects. 
             On a selection and validation of one of these bela booting on the project in question)
- page type: Bargraph (the value of the parameters is presented in vertical bargraph)
- type of page: Trill_Setup (self-defined page allowing to modify the settings of the trill sensors (prescaler, threshold ...) present on the system)
- possible to use of a trill sensor for parameter editing
- integration in Faust2bela
   
    
    
## Notes :
- Currently 3 types of screens are supported:
    "U8G2_SH1106_128X64_NONAME_F_HW_I2C_LINUX", "U8G2_SSD1306_128X64_NONAME_F_HW_I2C_LINUX", "U8G2_SSD1309_128X64_NONAME2_F_HW_I2C_LINUX".
    It is easy to add some because the U8G2 library supports a lot.
- The refresh of the screen buffer via i2c is very slow (approx 30ms for a screen of 128x64 pixels) 
    So if you use i2c sensors it is preferable that the screens and sensors are on a different I2C bus.
 