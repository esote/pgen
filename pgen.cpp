#include <cctype>
#include <cstdlib>
#include <iostream>
#include <random>
#include <string>

bool getOptions(const std::string & option);
long long unsigned getLength();

int main() {

	bool lower, upper, nums, syms;

	while(true) {
		lower 	= getOptions("Lowercase Chars");
		upper 	= getOptions("Uppercase Chars");
		nums 	= getOptions("Numbers");
		syms 	= getOptions("Symbols");

		if(syms || nums || lower || upper) break;

		std::cout << "\n\tERR: No options selected.\n\n";
	}

	const std::string str_lower	 = "abcdefghijklmnopqrstuvwxyz";
	const std::string str_upper	 = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	const std::string str_nums	 = "1234567890";
	const std::string str_syms	 = "`~!@#$%^&*()-=_+[]{}\\|;:\'\",<.>/\?";

	std::string allowedChars;

	if(lower) 	allowedChars += str_lower;
	if(upper) 	allowedChars += str_upper;
	if(nums) 	allowedChars += str_nums;
	if(syms) 	allowedChars += str_syms;

	long long unsigned passLength = getLength();

	std::string password;

	std::random_device rd;
	std::mt19937 generator(rd());
	std::uniform_int_distribution<int> dis(0, allowedChars.length());
	
	for(long long unsigned i = 0; i < passLength; ++i) {
		password += allowedChars[dis(generator)];
	}

	std::cout << '\n' + password + '\n';

	return 0;
}

bool getOptions(const std::string & option) {

	while(true) {

		std::string choice;

		std::cout << option + " (y/N): ";

		std::getline(std::cin, choice);

		if(tolower(choice[0]) == 'y') {
			return true;
		} else if(tolower(choice[0]) == 'n') {
			return false;
		}

	}

}

long long unsigned getLength() {

	long long unsigned passLength;

	while(true) {

		std::string str_passLength;

		std::cout << "Password Length: ";
		std::getline(std::cin, str_passLength);

		try {
			passLength = std::stoll(str_passLength);
		} catch(const std::invalid_argument & e) {
			std::cerr << "\n\tERR: Invalid argument (noninteger).\n\n";
		} catch(const std::out_of_range & e) {
			std::cerr << "\n\tERR: Out of range (long long unsigned).\n\n";
		} catch(...) {
			std::cerr << "\n\tERR: Unknown, something messed up.\n\n";
		}

		if(passLength > 0) {
			return passLength;
		}
		
	}

}
