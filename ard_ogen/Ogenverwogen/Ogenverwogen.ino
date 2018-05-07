#include "ssd1306.h"
#include "nano_gfx.h"
#include "Ogenbmp.h"

#ifdef SSD1306_SPI_SUPPORTED
    #include <SPI.h>
#endif

static void show_bmp()
{
    ssd1306_drawBitmap(0, 0, 128, 64, Oog);
    ssd1306_invertMode();
}

void setup()
{
    /* Select the font to use with menu and all font functions */
    ssd1306_setFixedFont(ssd1306xled_font6x8);

    ssd1306_128x64_i2c_init();

    ssd1306_fillScreen( 0x00 );
}

void loop()
{
  show_bmp();
}
