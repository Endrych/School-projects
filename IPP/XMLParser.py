import xml.etree.ElementTree as ET
import Instruction
import Argument
import IPPProgram
import sys


class XMLParser:
    def __init__(self, filename):
        tree = ET.parse(filename)
        self.__root = tree.getroot()
        self.__program = IPPProgram.IPPProgram()

    def parse_document(self):
        if not self.__check_root():
            print("Root element must have correct attributes", file=sys.stderr)
            exit(31)

        for instruction in self.__root:
            ins = self.__parse_instruction(instruction)
            self.__program.add_instruction(ins)
        return self.__program

    def __check_root(self):
        if not self.__root.tag == "program":
            print("Root element must have tag program", file=sys.stderr)
            exit(31)
        has_language = False
        for key, value in self.__root.items():
            if key == "name":
                self.__program.set_name(value)
            elif key == "description":
                self.__program.set_description(value)
            elif key == "language" and value == "IPPcode18":
                has_language = True
            else:
                return False
        if not has_language:
            print("Root element must have language attribute", file=sys.stderr)
            return False
        return True

    def __parse_instruction(self, element):
        instruction = Instruction.Instruction()
        if element.tag != "instruction":
            print("Child element must has tag instruction", file=sys.stderr)
            exit(31)
        for key, value in element.items():
            if key == "order":
                instruction.set_order(value)
            elif key == "opcode":
                instruction.set_op_code(value)
            else:
                print("Instruction element must have correct attributes", file=sys.stderr)
                exit(31)
        if not instruction.get_op_code() or not instruction.get_order():
            print("Instruction element must have correct attributes", file=sys.stderr)
            exit(31)
        for arg in element:
            argument = self.__parse_argument(arg)
            instruction.add_argument(argument)
        return instruction

    def __parse_argument(self, element):
        argument = Argument.Argument()
        if element.tag[0:3] != "arg":
            print("Argument element tag must start with arg", file=sys.stderr)
            exit(31)
        if not element.tag[3:].isdigit():
            print("Argument element tag must end with digit", file=sys.stderr)
            exit(31)
        argument.set_order(int(element.tag[3:]))
        for key, value in element.items():
            if key == "type":
                argument.set_type(value)
            else:
                print("Argument element must have correct attributes", file=sys.stderr)
                exit(31)
        if not argument.get_type():
            print("Argument element must have type attribute", file=sys.stderr)
            exit(31)
        argument.set_value(element.text)
        return argument
