# Имя конечного исполняемого файла
TARGET = program

# Компилятор и флаги
CC = gcc
CFLAGS = -Wall -Wextra -g

# Исходные файлы
SRCS = parent.c child.c child_2.c

# Объектные файлы (создаются из исходных)
OBJS = $(SRCS:.c=.o)

# Правило сборки
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

# Правило для компиляции .c файлов в .o
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Очистка
clean:
	rm -f $(OBJS) $(TARGET)

# Фиктивные цели
.PHONY: all clean

all: $(TARGET)
