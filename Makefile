
all: bin/main bin/output

bin/main: obj/main.o obj/inputHMI.o obj/utils.o
	cc -o $@ $^

bin/output: obj/outputHMI.o obj/utils.o
	cc -o $@ $^

obj/%.o: src/%.c
	cc -c $< -I include -o $@

clean: 
	rm -f bin/* obj/*.o log/*