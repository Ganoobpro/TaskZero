#ifndef COLOR_H
#define COLOR_H



#define BACK_TO_NORMAL      "\033[0m"
#define START_COUT_COLOR    "\033["



// STANDARN COLOR
#define _BLACK         "30"
#define _RED           "31"
#define _GREEN         "32"
#define _YELLOW        "33"
#define _BLUE          "34"
#define _MAGETA        "35"
#define _CYAN          "36"
#define _WHITE         "37"

#define _BLACK_BG      "40"
#define _RED_BG        "41"
#define _GREEN_BG      "42"
#define _YELLOW_BG     "43"
#define _BLUE_BG       "44"
#define _MAGETA_BG     "45"
#define _CYAN_BG       "46"
#define _WHITE_BG      "47"



#define _ADD_COLOR(_message, TEXT_COLOR) \
  START_COUT_COLOR TEXT_COLOR "m" _message BACK_TO_NORMAL

#define _ADD_COLOR_TEXT_BG(_message, BG_COLOR, TEXT_COLOR) \
  START_COUT_COLOR BG_COLOR ";" TEXT_COLOR "m" _message BACK_TO_NORMAL

#define COUT_RAINBOW(_message, _length) \
  do { \
    for (size_t _message_index = 0; _message_index < _length; _message_index++) { \
      std::cout << "\033[3" << ((_message_index % 7) + 1) << "m" << _message[_message_index]; \
    } \
    std::cout << BACK_TO_NORMAL; \
  } while (0)



#endif // !COLOR_H
