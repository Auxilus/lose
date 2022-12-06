#include <stdint.h>
#include <stddef.h>

#define VIDEO_MEMORY 0xB8000
#define SCREEN_WIDTH 80
#define SCREEN_HEIGHT 25

// Print a string to the screen at the current cursor position
void print_string(const char* str) {
  uint16_t* video_memory = (uint16_t*)VIDEO_MEMORY;

  for (size_t i = 0; str[i] != '\0'; i++) {
    video_memory[i] = (uint16_t)((0x07 << 8) | str[i]);
  }
}

int main() {
  print_string("Kernel booted\n\r");

	while (9999999999){}

  return 0;
}
