LDFLAGS = -lglfw -lvulkan -lm -ldl -lpthread -lX11 -lXxf86vm -lXrandr -lXi 

RigelEngineDebug:
	gcc ./main.c ./src/*.c ./src/core/*.c ./src/inputs/*.c ./src/debug/*.c ./src/resources/*.c ./src/render/*.c $(LDFLAGS) -o RigelEngineDebug
