#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>

#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))
#define ACK 0x7E

static void pabort(const char *s)
{
  perror(s);
  abort();
}

static const char* device = "/dev/spidev0.0";
static uint8_t mode;
static uint8_t bits_per_word = 8;
static uint32_t speed_hz = 500000;
static uint16_t delay_us;

static void transfer(int fd, unsigned char degrees)
{
  uint8_t tx[] = {
    degrees
  };
  uint8_t rx[ARRAY_SIZE(tx)] = {0, };
  struct spi_ioc_transfer tr = {
    .tx_buf = (unsigned long)tx,
    .rx_buf = (unsigned long)rx,
    .len = ARRAY_SIZE(tx),
    .delay_usecs = delay_us,
    .speed_hz = speed_hz,
    .bits_per_word = bits_per_word,
  };

  int ret = ioctl(fd, SPI_IOC_MESSAGE(1), &tr);
  if (ret < 1)
    pabort("can't send spi message");

  for (ret = 0; ret < ARRAY_SIZE(tx); ret++) {
    if (!(ret % 6))
      puts("");
    printf("%.2X ", rx[ret]);
  }
  puts("");
}

static void print_usage(const char *prog)
{
  printf("Usage: %s [-DsdR]\n", prog);
  puts("  -D --device   device to use (default /dev/spidev0.0)\n"
       "  -s --speed    max speed (Hz)\n"
       "  -d --delay    delay (usec)\n"
       "  -R --ready    slave pulls low to pause\n");
  exit(1);
}

static void parse_opts(int argc, char *argv[])
{
  while (1) {
    static const struct option lopts[] = {
      { "device",  1, 0, 'D' },
      { "speed",   1, 0, 's' },
      { "delay",   1, 0, 'd' },
      { "ready",   0, 0, 'R' },
      { NULL, 0, 0, 0 },
    };

    int c = getopt_long(argc, argv, "D:s:d:R", lopts, NULL);

    if (c == -1)
      break;

    switch (c) {
    case 'D':
      device = optarg;
      break;
    case 's':
      speed_hz = atoi(optarg);
      break;
    case 'd':
      delay_us = atoi(optarg);
      break;
    case 'R':
      mode |= SPI_READY;
      break;
    default:
      print_usage(argv[0]);
      break;
    }
  }
}

int main(int argc, char* argv[]) {
  int ret = 0;

  mode |= SPI_NO_CS;
  parse_opts(argc, argv);

  int fd = open(device, O_RDWR);
  if (fd < 0)
    pabort("can't open device");

  printf("spi mode: %d\n", mode);
  printf("bits per word: %d\n", bits_per_word);
  printf("max speed: %d Hz (%d KHz)\n", speed_hz, speed_hz/1000);

  /*
   * spi mode
   */
  ret = ioctl(fd, SPI_IOC_WR_MODE, &mode);
  if (ret == -1)
    pabort("can't set spi mode");

  ret = ioctl(fd, SPI_IOC_RD_MODE, &mode);
  if (ret == -1)
    pabort("can't get spi mode");

  /*
   * bits per word
   */
  ret = ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &bits_per_word);
  if (ret == -1)
    pabort("can't set bits per word");

  ret = ioctl(fd, SPI_IOC_RD_BITS_PER_WORD, &bits_per_word);
  if (ret == -1)
    pabort("can't get bits per word");

  /*
   * max speed hz
   */
  ret = ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed_hz);
  if (ret == -1)
    pabort("can't set max speed hz");

  ret = ioctl(fd, SPI_IOC_RD_MAX_SPEED_HZ, &speed_hz);
  if (ret == -1)
    pabort("can't get max speed hz");

  while (1) {
    for (unsigned char degrees = 0; degrees <= 180; degrees += 15) {
      transfer(fd, degrees);
    }
  }

  close(fd);

  return ret;
}
