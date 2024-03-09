all: choices

choices: choices.C
	c++ -Wall -O9 -o choices choices.C

clean:
	rm choices
