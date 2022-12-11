build: main.out
	@./main.out < tests.txt
	@echo "\nEND"

main.out : main.cpp
	@g++ main.cpp -o main.out