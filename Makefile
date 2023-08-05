
all: bin/main

bin/main: obj/main.o
	cc -o $@ $^

obj/%.o: src/%.c
	cc -c $< -I include -o $@

clean: 
	rm -f bin/* obj/*.o