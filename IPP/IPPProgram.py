import sys
import re

class IPPProgram:

    def __init__(self):
        self.__name = None
        self.__description = None
        self.__instructions = dict()
        self.__labels = dict()

    def set_name(self, value):
        self.__name = value

    def set_description(self, value):
        self.__description = value

    def add_instruction(self, instruction):
        if instruction.get_order() in self.__instructions:
            print("More instruction with same order", file=sys.stderr)
            exit(21)
        self.__instructions[instruction.get_order()] = instruction

    def get_name(self):
        return self.__name

    def get_description(self):
        return self.__description

    def get_instructions(self):
        return self.__instructions

    def add_label(self, label_name, instruction_order):
        pattern = re.compile('([a-z]|[A-Z]|-|_|&|%|\*|\$)([a-z]|[A-Z]|[-]|[_]|[&]|[%]|[*]|[$]|[0-9])*')
        if not pattern.match(label_name):
            print("Wrong label name in instruction " + instruction_order, file=sys.stderr)
            exit(52)
        if label_name in self.__labels:
            print("Label name already exists " + instruction_order, file=sys.stderr)
            exit(52)
        self.__labels[label_name] = instruction_order

    def get_label(self, label_name):
        if label_name not in self.__labels:
            print("Label name doesnt exists ", file=sys.stderr)
            exit(52)
        return self.__labels[label_name]
