CC = gcc
CFLAGS = -Wall -Wextra -I./include

SRCS = core/main.c core/poggi_string.c core/poggi_mem.c core/poggi_fs.c core/poggi_task.c core/poggi_ipc.c hal/hosted/poggi_hal_hosted.c

OBJS = $(SRCS:.c=.o)
TARGET = poggi_os_bin

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET)

clean:
	rm -f $(OBJS) $(TARGET)