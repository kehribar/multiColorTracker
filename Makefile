###############################################################################
# Example makefile for openCV projects.
# ihsan@kehribar.me wrote this file.
###############################################################################
CFLAGS = `pkg-config --cflags opencv4`
LIBS = `pkg-config --libs opencv4`

TARGET = main

.PHONY: clean

$(TARGET):
	g++ $(TARGET).cpp multiColorTracker.c -o $(TARGET) $(CFLAGS) $(LIBS)

clean:
	rm -f *.o *~ ./$(TARGET) 
