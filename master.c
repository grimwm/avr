#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <termios.h>

static void pabort(const char *s)
{
  perror(s);
  abort();
}

static const char* device = "/dev/ttyAMA0";
static uint8_t bits_per_word = 8;
static uint32_t baudrate = 9600;

static void print_usage(const char *prog) {
  printf("Usage: %s [-DbB]\n", prog);
  puts("  -D --device   device to use (default /dev/ttyAMA0)\n"
       "  -b --baud     baud rate (default 9600)\n"
       "  -B --bpw      bits per word (default 8)\n");
  exit(1);
}

static void parse_opts(int argc, char *argv[]) {
  while (1) {
    static const struct option lopts[] = {
      { "device",  1, 0, 'D' },
      { "baud",    1, 0, 'b' },
      { "bpw",     1, 0, 'B' },
      { NULL, 0, 0, 0 },
    };

    int c = getopt_long(argc, argv, "D:b:B:", lopts, NULL);

    if (-1 == c) {
      break;
    }

    switch (c) {
    case 'D':
      device = optarg;
      break;
    case 'b':
      baudrate = atoi(optarg);
      break;
    case 'B':
      bits_per_word = atoi(optarg);
      break;
    default:
      print_usage(argv[0]);
      break;
    }
  }
}

int main(int argc, char* argv[]) {
  parse_opts(argc, argv);

  int fd = open(device, O_RDWR | O_NOCTTY | O_NONBLOCK);
  if (-1 == fd) {
    pabort("can't open device");
  }

  if (-1 == fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK)) {
    pabort("Setting stdin to nonblocking.");
  }

  if (!isatty(fd)) {
    pabort("isatty");
  }

  printf("bits per word: %d\n", bits_per_word);
  printf("baud rate: %d\n", baudrate);

  struct termios options;
  if (-1 == tcgetattr(fd, &options)) {
    pabort("Couldn't get current port options");
  }

  options.c_oflag &= ~OPOST;

  /* Enable local line (no owner change) and enable receiver. */
  options.c_cflag |= CLOCAL | CREAD;
  /* Enable odd parity */
  options.c_cflag |= PARENB | PARODD;

  options.c_cflag &= ~CSIZE;
  switch (bits_per_word) {
  case 5:
    options.c_cflag |= CS5;
    break;
  case 6:
    options.c_cflag |= CS6;
    break;
  case 7:
    options.c_cflag |= CS7;
    break;
  default:
  case 8:
    options.c_cflag |= CS8;
    break;
  }

  speed_t speed;
  switch (baudrate) {
  default:
  case 9600:
    speed = B9600;
    break;
  case 19200:
    speed = B19200;
    break;
  case 57600:
    speed = B57600;
    break;
  case 115200:
    speed = B115200;
    break;
  }
  if (-1 == cfsetispeed(&options, speed)) {
    pabort("Error setting input speed");
  }
  if (-1 == cfsetospeed(&options, speed)) {
    pabort("Error setting output speed");
  }

  /* Only send data after 1 chars are in the buffer.  Don't use a timer. */
  options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);

  /* options.c_iflag &= ~(IGNBRK | BRKINT | ICRNL | INLCR | PARMRK | INPCK | ISTRIP | IXON); */
  /* options.c_oflag = 0; */
  /* options.c_lflag &= ~(ECHO | ECHONL | ICANON | IEXTEN | ISIG); */
  /* options.c_cflag &= ~(CSIZE | PARENB); */
  /* options.c_cflag |= CS8; */
  /* if (-1 == cfsetispeed(&options, B9600)) { */
  /*   pabort("Error setting input speed"); */
  /* } */
  /* if (-1 == cfsetospeed(&options, B9600)) { */
  /*   pabort("Error setting output speed"); */
  /* } */
  
  if (-1 == tcsetattr(fd, TCSAFLUSH, &options)) {
    pabort("Error setting serial line options");
  }

  printf("\nEnter input, and it will be sent to %s until EOF is encountered on stdin.\n\n\n", device);
  while (1) {
    unsigned char b=0;
    if (read(fd, &b, 1) > 0) {
      if (write(STDOUT_FILENO, &b, 1) < 1) {
        pabort("writing to standard out");
      }
    }

    if (read(STDIN_FILENO, &b, 1) > 0) {
      if (write(fd, &b, 1) < 1) {
        pabort("writing to tty");
      }
    }
  }

  return close(fd);
}
