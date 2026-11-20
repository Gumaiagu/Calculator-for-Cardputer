// Uses M5Cardputer library and board

#include "M5Cardputer.h"
#include <math.h>
#include <deque>

#define TITLE "Calculator"
#define TitleMarginY 5

#define InputMarginBottom 5
#define InputMarginLeft 10

#define BufferMarginY 3

#define UsedFont &fonts::Font0

#define DecimalPrecision 4

#define BufferValuesInDisplay 4

static String expretion = "";

static std::deque<double> buffer;

static int32_t text_height;
static int32_t display_height;
static int32_t display_width;
static int32_t input_size;


void draw_title() {
  M5Cardputer.Display.setTextSize(2);

  M5Cardputer.Display.fillRect(0, 0, display_width, 2*(text_height + TitleMarginY), TFT_ORANGE);  // Make the text background orange
  M5Cardputer.Display.drawString(TITLE, (display_width - M5Cardputer.Display.textWidth(TITLE))/2, TitleMarginY, UsedFont);
}


void draw_input() {
  M5Cardputer.Display.setTextSize(1.5);
  
  M5Cardputer.Display.fillRect(0, display_height - (input_size + InputMarginBottom), display_width, 2, TFT_ORANGE);   // Draws the line between buffers and the input
  M5Cardputer.Display.drawString("> " + expretion, InputMarginLeft, display_height - input_size, UsedFont);
}


void draw_error(char error[]) {
  clear_input();
  M5Cardputer.Display.setTextSize(1.25);
  
  M5Cardputer.Display.fillRect(0, display_height - (input_size + 5), display_width, 2, TFT_RED);
  M5Cardputer.Display.drawString(error, InputMarginLeft, display_height - input_size, UsedFont);
  delay(1500);    // Delay to allow the user to read the text
}


void clear_input() {
  M5Cardputer.Display.fillRect(InputMarginLeft, display_height - input_size, display_width, input_size, TFT_BLACK);
}


void update_buffer_value(float value, size_t index) {
  M5Cardputer.Display.setTextSize(1.25);
  int32_t char_width = M5Cardputer.Display.fontWidth(UsedFont);
  int32_t y_position = 2*(text_height + TitleMarginY)*(BufferValuesInDisplay-index) + BufferMarginY;
  if (value > 0)
    M5Cardputer.Display.drawFloat(value, DecimalPrecision, display_width - char_width*(2 + DecimalPrecision + max(floor(log10(value)), 0.0f)), y_position, UsedFont);
  else if (value < 0)
    M5Cardputer.Display.drawFloat(value, DecimalPrecision, display_width - char_width*(3 + DecimalPrecision + max(floor(log10(-value)), 0.0f)), y_position, UsedFont);
  else
    M5Cardputer.Display.drawString("0.0", display_width - char_width*3, y_position, UsedFont);  // I didn't see nothing in the documentation telling that "drawFloat" can't draw 0.0f, but it happens
}


void draw_buffer() {
  M5Cardputer.Display.setTextSize(1.25);

  for (size_t i=1; i<=BufferValuesInDisplay; i++) {
    if (i-1 < buffer.size())
      update_buffer_value(buffer[i-1], i-1);
    M5Cardputer.Display.drawNumber(i, 0, 2*(text_height + TitleMarginY)*(BufferValuesInDisplay-i+1) + BufferMarginY, UsedFont);
    if (i != 1)
      M5Cardputer.Display.fillRect(0, 2*(text_height + TitleMarginY)*(BufferValuesInDisplay-i+1) + 3*BufferMarginY + text_height, display_width, 2, TFT_WHITE);
  }
}


void clear_buffers() {
  M5Cardputer.Display.fillRect(0, 2*(text_height + TitleMarginY), display_width, (display_height - (InputMarginBottom + text_height + 5)) - 2*(text_height + TitleMarginY), TFT_BLACK);
}

void setup() {
  Serial.begin(9600);
  Serial.println("Working");
  m5::M5Unified::config_t cfg = M5.config();
  M5Cardputer.begin(cfg, true);

  // Define useful variables
  text_height = M5Cardputer.Display.fontHeight(UsedFont);
  display_height = M5Cardputer.Display.height();
  display_width = M5Cardputer.Display.width();
  input_size = InputMarginBottom + text_height;

  M5Cardputer.Display.setTextColor(TFT_WHITE);

  // Makes the first draws
  draw_title();
  draw_input();

  draw_buffer();
}

void loop() {
  M5Cardputer.update();
  if (M5Cardputer.Keyboard.isChange() && M5Cardputer.Keyboard.isPressed()) {
    Keyboard_Class::KeysState status = M5Cardputer.Keyboard.keysState();
    size_t next_delay;  // Wait an certain amount of time to allow the user to stop holding the button. 

    if (status.enter) {
      interpret_expretion();
      next_delay = 400;   // More delay because it can negativily impact the calculations
      expretion.clear();
    } else if (status.del) {
      expretion.remove(expretion.length() - 1);
      next_delay = 100;
    } else {
      for (char i: status.word)
        expretion.concat(i);
      next_delay = 100;
    }
    clear_input();
    draw_input();
    delay(next_delay);
  }
}


float factorial(double value) {
  return std::tgamma(value + 1);
}


// Just simplify the do_operator function
#define has_needed_buffer_size(nededed) if (buffer.size() < nededed) {draw_error("There isn't enough values in buffer.");return;}


void do_operator() {
  if (expretion.compareTo("+/-") == 0) {
    has_needed_buffer_size(1);
    buffer.front() = -buffer.front();
  }
  else if (expretion.compareTo("+") == 0) {
    has_needed_buffer_size(2);
    buffer[1] = buffer.front() + buffer[1];
    buffer.pop_front();
  }
  else if (expretion.compareTo("-") == 0) {
    has_needed_buffer_size(2);
    buffer[1] = buffer[1] - buffer.front();
    buffer.pop_front();
  }
  else if (expretion.compareTo("*") == 0) {
    has_needed_buffer_size(2);
    buffer[1] = buffer.front() * buffer[1];
    buffer.pop_front();
  }
  else if (expretion.compareTo("/") == 0) {
    if (buffer.front() == 0) {
      draw_error("Can't divide by 0!");
      return;
    }
    has_needed_buffer_size(2);
    buffer[1] = buffer[1] / buffer.front();
    buffer.pop_front();
  }
  else if (expretion.compareTo("d") == 0) {
    has_needed_buffer_size(1);
    buffer.pop_front();
  }
  else if ((expretion.compareTo("**") == 0) || (expretion.compareTo("^") == 0)) {
    has_needed_buffer_size(2);
    buffer[1] = pow(buffer[1], buffer.front());
    buffer.pop_front();
  }
  else if (expretion.compareTo("sqrt") == 0) {
    has_needed_buffer_size(1);
    buffer.front() = sqrt(buffer.front());
  }
  else if (expretion.compareTo("!") == 0) {
    has_needed_buffer_size(1);
    buffer.front() = factorial(buffer.front());
  }
  else if (expretion.compareTo("cos") == 0) {
    has_needed_buffer_size(1);
    buffer.front() = cos(buffer.front());
  }
  else if (expretion.compareTo("tan") == 0) {
    has_needed_buffer_size(1);
    buffer.front() = tan(buffer.front());
  }
  else if (expretion.compareTo("sin") == 0) {
    has_needed_buffer_size(1);
    buffer.front() = sin(buffer.front());
  } else {
    draw_error("This input isn't valid.");
  }
  clear_buffers();
  draw_buffer();
}


void interpret_expretion() {
  if (expretion.length() > 0) {
    char arr[expretion.length() + 1];
    strcpy(arr, expretion.c_str());

    char* end = NULL;
    double f = strtod(arr, &end);
    if ((arr == NULL) || (arr == end) || (*end != '\0'))
      do_operator();
    else {
      buffer.push_front(f);
      if (buffer.size()<=BufferValuesInDisplay) {
        clear_buffers();
        draw_buffer();
      }
    }
  }
}
