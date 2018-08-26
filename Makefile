cdjson: *.c
	clang -o $@ $^ -Wall -g

.PHONY: clean
clean:
	rm ./cdjson
