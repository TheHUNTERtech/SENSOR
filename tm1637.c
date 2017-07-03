#include "tm1637.h"
#include "tm1637_conf.h"

/*
#define TM1637_DIO_HIGH()       (PORTB |= _BV(TM1637_DIO_PIN))
#define TM1637_DIO_LOW()        (PORTB &= ~_BV(TM1637_DIO_PIN))
#define TM1637_DIO_OUTPUT()     (DDRB |= _BV(TM1637_DIO_PIN))
#define TM1637_DIO_INPUT()      (DDRB &= ~_BV(TM1637_DIO_PIN))
#define TM1637_DIO_READ()       (((PINB & _BV(TM1637_DIO_PIN)) > 0) ? 1 : 0)
#define TM1637_CLK_HIGH()       (PORTB |= _BV(TM1637_CLK_PIN))
#define TM1637_CLK_LOW()        (PORTB &= ~_BV(TM1637_CLK_PIN))
*/

#define TM1637_FLAG_ENABLED     (1 << 0)
#define TM1637_FLAG_SHOWCOLON       (1 << 1)


static void TM1637_configure(void);
static void TM1637_cmd(uint8_t value);
static void TM1637_start(void);
static void TM1637_stop(void);
static uint8_t TM1637_write_byte(uint8_t value);

static const uint8_t _digit2segments[] =
{
    0x3F, // 0
    0x06, // 1
    0x5B, // 2
    0x4F, // 3
    0x66, // 4
    0x6D, // 5
    0x7D, // 6
    0x07, // 7
    0x7F, // 8
    0x6F  // 9
};

static uint8_t _brightness = TM1637_DEFAULT_BRIGHTNESS;
static uint8_t _digit = 0xff;
static uint8_t _flags = 0x00;

static void delay_us(__IO uint32_t nCount)
{
  uint32_t index = 0;
  for(index = (10 * nCount); index != 0; index--);
}

void TM1637_DIO_INPUT(void) {
    GPIO_InitTypeDef    GPIOInit;

    GPIOInit.GPIO_Pin   = TM1637_DIO_PIN;
    GPIOInit.GPIO_Mode  = GPIO_Mode_IN;
    GPIOInit.GPIO_OType = GPIO_OType_PP;
    GPIOInit.GPIO_Speed = GPIO_Speed_100MHz;
    GPIOInit.GPIO_PuPd  = GPIO_PuPd_UP;
    GPIO_Init(TM1637_GPIO_PORT, &GPIOInit);
}

void TM1637_DIO_OUTPUT(void) {
    GPIO_InitTypeDef    GPIOInit;

    GPIOInit.GPIO_Pin   = TM1637_DIO_PIN;
    GPIOInit.GPIO_Mode  = GPIO_Mode_OUT;
    GPIOInit.GPIO_OType = GPIO_OType_PP;
    GPIOInit.GPIO_Speed = GPIO_Speed_100MHz;
    GPIOInit.GPIO_PuPd  = GPIO_PuPd_NOPULL;
    GPIO_Init(TM1637_GPIO_PORT, &GPIOInit);
}

void TM1637_init(void)
{

    GPIO_InitTypeDef        GPIO_InitStructure;

    TM1637_GPIO_PORT_ENCLK();
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;    
    GPIO_InitStructure.GPIO_Pin = TM1637_CLK_PIN | TM1637_DIO_PIN;
    GPIO_Init(TM1637_GPIO_PORT, &GPIO_InitStructure);

    _flags |= TM1637_FLAG_ENABLED;
    TM1637_clear();
}

void
TM1637_display_digit(const uint8_t addr, const uint8_t digit)
{
    uint8_t segments = digit < 10 ? _digit2segments[digit] : 0x00;

    if (addr == TM1637_SET_ADR_01H) {
        _digit = digit;
        if (_flags & TM1637_FLAG_SHOWCOLON) {
            segments |= 0x80;
        }
    }

    TM1637_display_segments(addr, segments);
}

void
TM1637_display_segments(const uint8_t addr, const uint8_t segments)
{

    TM1637_cmd(TM1637_CMD_SET_DATA | TM1637_SET_DATA_F_ADDR);
    TM1637_start();
    TM1637_write_byte(TM1637_CMD_SET_ADDR | addr);
    TM1637_write_byte(segments);
    TM1637_stop();  
    TM1637_configure(); 
}

void
TM1637_display_colon(bool value)
{

    if (value) {
        _flags |= TM1637_FLAG_SHOWCOLON;
    } else {
        _flags &= ~TM1637_FLAG_SHOWCOLON;
    }
    TM1637_display_digit(TM1637_SET_ADR_01H, _digit);
}

void
TM1637_clear(void)
{   

    TM1637_display_colon(false);
    TM1637_display_segments(TM1637_SET_ADR_00H, 0x00);
    TM1637_display_segments(TM1637_SET_ADR_01H, 0x00);
    TM1637_display_segments(TM1637_SET_ADR_02H, 0x00);
    TM1637_display_segments(TM1637_SET_ADR_03H, 0x00);
}

void
TM1637_set_brightness(const uint8_t brightness)
{

    _brightness = brightness & 0x07;
    TM1637_configure();
}

void
TM1637_enable(bool value)
{

    if (value) {
        _flags |= TM1637_FLAG_ENABLED;
    } else {
        _flags &= ~TM1637_FLAG_ENABLED;
    }
    TM1637_configure();
}

void
TM1637_configure(void)
{
    uint8_t cmd;

    cmd = TM1637_CMD_SET_DSIPLAY;
    cmd |= _brightness;
    if (_flags & TM1637_FLAG_ENABLED) {
        cmd |= TM1637_SET_DISPLAY_ON;
    }

    TM1637_cmd(cmd);
}

void
TM1637_cmd(uint8_t value)
{

    TM1637_start();
    TM1637_write_byte(value);
    TM1637_stop();
}

void
TM1637_start(void)
{

    TM1637_DIO_PIN_HIGH();
    TM1637_CLK_PIN_HIGH();
    delay_us(TM1637_DELAY_US);
    TM1637_DIO_PIN_LOW();
}

void
TM1637_stop(void)
{

    TM1637_CLK_PIN_LOW();
    delay_us(TM1637_DELAY_US);

    TM1637_DIO_PIN_LOW();
    delay_us(TM1637_DELAY_US);

    TM1637_CLK_PIN_HIGH();
    delay_us(TM1637_DELAY_US);

    TM1637_DIO_PIN_HIGH();
}

uint8_t
TM1637_write_byte(uint8_t value)
{
    uint8_t i, ack;

    for (i = 0; i < 8; ++i, value >>= 1) {
        TM1637_CLK_PIN_LOW();
        delay_us(TM1637_DELAY_US);

        if (value & 0x01) {
            TM1637_DIO_PIN_HIGH();
        } else {
            TM1637_DIO_PIN_LOW();
        }

        TM1637_CLK_PIN_HIGH();
        delay_us(TM1637_DELAY_US);
    }

    TM1637_CLK_PIN_LOW();
    TM1637_DIO_INPUT();
    TM1637_DIO_PIN_HIGH();
    delay_us(TM1637_DELAY_US);

    ack = TM1637_DIO_READ();
    if (ack) {
        TM1637_DIO_OUTPUT();
        TM1637_DIO_PIN_LOW();
    }
    delay_us(TM1637_DELAY_US);

    TM1637_CLK_PIN_HIGH();
    delay_us(TM1637_DELAY_US);

    TM1637_CLK_PIN_LOW();
    delay_us(TM1637_DELAY_US);

    TM1637_DIO_OUTPUT();

    return ack;
}