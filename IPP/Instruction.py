import sys


class Instruction:

    def __init__(self):
        self.__op_code = None
        self.__arguments = dict()
        self.__order = None

    def set_op_code(self, value):
        self.__op_code = value

    def set_order(self, value):
        self.__order = value

    def add_argument(self, argument):
        if argument.get_order() not in self.__arguments:
            self.__arguments[argument.get_order()] = argument
        else:
            print("Instruction already has argument with this order", file=sys.stderr)
            exit(31)

    def get_order(self):
        return self.__order

    def get_arguments(self):
        return self.__arguments

    def get_op_code(self):
        return self.__op_code
