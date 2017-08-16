#!/usr/bin/python

import random


def getOptions(option):
	
	while True:

		choice = ""

		print("{} (y/N): ".format(option), end="")
		choice = input()
		if len(choice) == 0:
			pass
		elif choice[0].lower() == 'y':
			return True
		elif choice[0].lower() == 'n':
			return False


def getLength():

	passLength = 0

	while True:
		
		str_passLength = input("Password Length: ")

		try:
			passLength = int(str_passLength)
		except ValueError:
			print("\n\tERR: Invalid argument (noninteger).\n")
		except OverflowError:
			print("\n\tERR: Out of range (overflow) (int)")
		except:
			print("\n\tERR: Unknown, something messed up.\n\t")

		if passLength > 0:
			return passLength


def main():

	lower, upper, nums, syms = [True for i in range(4)]

	while True:
		lower 	= getOptions("Lowercase Chars")
		upper 	= getOptions("Uppercase Chars")
		nums 	= getOptions("Numbers")
		syms 	= getOptions("Symbols")

		if syms or nums or lower or upper: break

		print("\n\tERR: No options selected.\n\n")

	str_lower 	= "abcdefghijklmnopqrstuvwxyz"
	str_upper 	= "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
	str_nums 	= "1234567890"
	str_syms 	= "`~!@#$%^&*()-=_+[]{}\\|;:\'\",<.>/?"

	allowedChars = ""

	if lower: 	allowedChars += str_lower
	if upper: 	allowedChars += str_upper
	if nums: 	allowedChars += str_nums
	if syms: 	allowedChars += str_syms

	passLength = getLength()

	password = ""

	for i in range(passLength):
		password += random.choice(allowedChars)

	print('\n' + password + '\n')


if __name__ == '__main__':
	main()
