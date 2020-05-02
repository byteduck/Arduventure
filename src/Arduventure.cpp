/**
 * Byteduck 2020
 * 
 * This is a simple "Adventure" (1979) - like game for the Arduino Mega 2560.
 * It uses no libraries for VGA output.
 * Check Arduventure.h for pin mappings for the display output.
 * For each color, PIN1 and PIN2 are hooked up with resistors such that PIN1 will be darker, PIN2 will be brighter, and both will be the full color.
 * The "controller" is hooked up to pins 37-34 (UP, DOWN, LEFT, RIGHT).
 */

#include "Arduventure.h"
#include "game.h"

//6-bit color (2 bits per R/G/B). This takes up almost all of the dynamic memory on
//the Arduino Mega 2560 with a 120x60 framebuffer. In BGR format: 0bBBGGRR__
byte displayBuffer[PIXELS_WIDTH * PIXELS_HEIGHT];

int blankLinesLeft; //How many lines are left in the blanking interval
byte line; //What line we're drawing in the framebuffer (NOT THE ACTUAL SCREEN LINE)
byte sLine; //What subline we're on

void setup() {
  //Setup pins
  pinMode(VSYNC_PIN, OUTPUT);
  pinMode(HSYNC_PIN, OUTPUT);
  pinMode(RED_PIN1, OUTPUT);
  pinMode(RED_PIN2, OUTPUT);
  pinMode(GREEN_PIN1, OUTPUT);
  pinMode(GREEN_PIN2, OUTPUT);
  pinMode(BLUE_PIN1, OUTPUT);
  pinMode(BLUE_PIN2, OUTPUT);

  //Set up controller pins
  for(int i = 30; i < 37; i++){
    pinMode(i, INPUT);
  }

  //Clear interrupts
  cli();

  //disable timer 0 because it interferes with the others that we need to be precise
  TIMSK0=0;
  TCCR0A=0;
  TCCR0B=(1 << CS00); //enable 16MHz counter
  OCR0A=0;
  OCR0B=0;
  TCNT0=0;

  /**
   * Setup timer for fast PWM for pulsing vsync.
   * This needs to be used because the built-in PWM functions are not precise enough.
   * This is pin 11 on the Arduino Mega 2560.
   * The way this works is that there's a counter which starts at 0 and ends at a predetermined value.
   * The counter goes up by one every (Clock speed (16Mhz) / Prescaler) microseconds.
   * We apply a prescaler of 1024 here, which means that the timer will count up every 64us.
   * Once it reaches the value we want, the pin is switched on. It stays on for another arbitrary amount of counts.
   * The timer also calls an interrupt (defined above) everytime it fires.
   * https://www.arduino.cc/en/Tutorial/SecretsOfArduinoPWM
  **/
  TCCR1A = bit(WGM11) | bit(COM1A1); //9-bit counter, fast PWM, clear A when counter reaches target (This means a 64us duty cycle)
  TCCR1B = bit(WGM12) | bit(WGM13) | bit(CS12) | bit(CS10); //Prescaler of 1024

  // These next two lines set up the counter
  ICR1 = 259; // Generate a pulse after the timer counts to 259, which is approximately every 260*64us, which is 60Hz
  OCR1A = 0;   // Keep the pulse on for 1 count (64us)
  TIFR1 = bit(TOV1); // Clear the overflow flag just in case
  TIMSK1 = bit(TOIE1); // Call interrupt ISR (TIMER1_OVF_vect) when counter reaches target

  /**
   * Same deal, but for horizontal sync.
   * We want this one to go off every 32us, so we use a prescaler of 8 to count up every 0.5us this time.
   * This is pin 9 on the Mega 2560.
   */
  TCCR2A = bit(WGM20) | bit(WGM21) | bit(COM2B1); //Fast PWM, 8-bit, pin 5
  TCCR2B = bit(WGM22) | bit(CS21); //Prescaler of 8
  OCR2A = 63; // Generate pulse when counter gets to 63
  OCR2B = 7; // Keep the pulse on for 8 counts (8us)
  TIFR2 = bit(TOV2); // Clear overflow flag just in case
  TIMSK2 = bit(TOIE2); // Call interrupt ISR (TIMER2_OVF_vect) when counter reaches target

  sei(); //Set interrupts
  set_sleep_mode (SLEEP_MODE_IDLE); //Processor should sleep in between interrupts so timing is right

  initGame();
}

void loop() {
  // Wait for interrupt. This way the CPU starts drawing the scanline at the same time every time
  sleep_mode();
  if(line == PIXELS_HEIGHT) {
    line++;
    gameTick();
  }
}

//Vertical sync interrupt. Called every time a new frame starts.
ISR (TIMER1_OVF_vect) {
  //Set blankLinesLeft to VERTICAL_SKIP because we need to not draw anything during veritcal blanking
  blankLinesLeft = VERTICAL_SKIP;
  //Set line to 0 since we're back at the first line
  sLine = -1;
  line = 0;
}

//Horizontal sync interrupt. Called every time a new line starts.
ISR (TIMER2_OVF_vect) {
  //If we're still in the vertical blanking interval or past the screen area, do nothing.
  //Can't add more if statements than this over here because the compiler gets really
  //sensitive about relative conditional branching around long assembly code like below
  if(blankLinesLeft || line >= PIXELS_HEIGHT) {
    blankLinesLeft--;
    return;
  }

  /**
   * Here, we write to the PORTA register directly. The last 3 bits on
   * PORTA correspond to pins 27, 28, and 29, so by writing to it we
   * can change the state of those pins all at the same time. This is
   * also done with assembly to avoid C++ overhead, since we need to
   * have this be as fast as possible. The below assembly code is approx.
   * equal to this pseudocode:
   * for(i = 0..PIXELS_WIDTH){
   *   pixels = displayBuffer[line/8][i]
   *   PORTA = pixels
   * }
   */
  asm volatile (
    ".rept 32                   \n\t" // Rept is just a macro that actually copies the below code x amount of times
    "  nop                      \n\t" // Do nothing for 30 cycles to align the picture
    ".endr                      \n\t" // End repeat
    ".rept " STR(PIXELS_WIDTH) "\n\t" // Repeat the below code PIXELS_WIDTH times
    "  ld r16, Z+               \n\t" // Load the byte in the current position in the display buffer to r16 and add one to the display buffer pointer
    "  out %[port], r16         \n\t" // Write r16 to PORTA
    ".endr                      \n\t" // End repeat
    "nop                        \n\t" // Do nothing for one clock cycle to expand the last pixel
    "ldi r16,0                  \n\t" // Load 0 into r16
    "out %[port], r16           \n\t" // Write that to PORTA to turn off the pins
    :
    : [port] "I" (_SFR_IO_ADDR(PORTA)),                // This specifies that %[port] will correspond to PORTA
      "z" "I" (displayBuffer + line * PIXELS_WIDTH)    // This specifies that the display buffer pointer at the current line will be stored in Z
    : "r16", "memory"                           // This specifies r16 for use
  );
  
  if(++sLine == SUBLINES_PER_LINE - 1) {
    sLine = -1;
    line++;
  }
}