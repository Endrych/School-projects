import sys
import re
import Variable
import DataStackItem
import MemoryManager

class IPPInterpret:

    def __init__(self, program):
        self.__memory_manager = MemoryManager.MemoryManager()
        self.__program = program
        self.__current_instruction_order = 1

    def __interpret_instruction(self, instruction):
        if instruction.get_op_code() == "MOVE":
            self.__intepret_move(instruction.get_arguments(), instruction.get_order())
        elif instruction.get_op_code() == "CREATEFRAME":
            self.__intepret_createframe(instruction.get_arguments(), instruction.get_order())
        elif instruction.get_op_code() == "PUSHFRAME":
            self.__intepret_pushframe(instruction.get_arguments(), instruction.get_order())
        elif instruction.get_op_code() == "POPFRAME":
            self.__intepret_popframe(instruction.get_arguments(), instruction.get_order())
        elif instruction.get_op_code() == "DEFVAR":
            self.__intepret_defvar(instruction.get_arguments(), instruction.get_order())
        elif instruction.get_op_code() == "CALL":
            return self.__intepret_call(instruction.get_arguments(), instruction.get_order())
        elif instruction.get_op_code() == "RETURN":
            return self.__intepret_return(instruction.get_arguments(), instruction.get_order())
        elif instruction.get_op_code() == "PUSHS":
            self.__intepret_pushs(instruction.get_arguments(), instruction.get_order())
        elif instruction.get_op_code() == "POPS":
            self.__intepret_pops(instruction.get_arguments(), instruction.get_order())
        elif instruction.get_op_code() == "ADD":
            self.__intepret_add(instruction.get_arguments(), instruction.get_order())
        elif instruction.get_op_code() == "SUB":
            self.__intepret_sub(instruction.get_arguments(), instruction.get_order())
        elif instruction.get_op_code() == "MUL":
            self.__intepret_mul(instruction.get_arguments(), instruction.get_order())
        elif instruction.get_op_code() == "IDIV":
            self.__intepret_idiv(instruction.get_arguments(), instruction.get_order())
        elif instruction.get_op_code() == "LT":
            self.__intepret_lt(instruction.get_arguments(), instruction.get_order())
        elif instruction.get_op_code() == "GT":
            self.__intepret_gt(instruction.get_arguments(), instruction.get_order())
        elif instruction.get_op_code() == "EQ":
            self.__intepret_eq(instruction.get_arguments(), instruction.get_order())
        elif instruction.get_op_code() == "AND":
            self.__intepret_and(instruction.get_arguments(), instruction.get_order())
        elif instruction.get_op_code() == "OR":
            self.__intepret_or(instruction.get_arguments(), instruction.get_order())
        elif instruction.get_op_code() == "NOT":
            self.__intepret_not(instruction.get_arguments(), instruction.get_order())
        elif instruction.get_op_code() == "INT2CHAR":
            self.__intepret_int2char(instruction.get_arguments(), instruction.get_order())
        elif instruction.get_op_code() == "STRI2INT":
            self.__intepret_stri2int(instruction.get_arguments(), instruction.get_order())
        elif instruction.get_op_code() == "READ":
            self.__intepret_read(instruction.get_arguments(), instruction.get_order())
        elif instruction.get_op_code() == "WRITE":
            self.__intepret_write(instruction.get_arguments(), instruction.get_order())
        elif instruction.get_op_code() == "CONCAT":
            self.__intepret_concat(instruction.get_arguments(), instruction.get_order())
        elif instruction.get_op_code() == "STRLEN":
            self.__intepret_strlen(instruction.get_arguments(), instruction.get_order())
        elif instruction.get_op_code() == "GETCHAR":
            self.__intepret_getchar(instruction.get_arguments(), instruction.get_order())
        elif instruction.get_op_code() == "SETCHAR":
            self.__intepret_setchar(instruction.get_arguments(), instruction.get_order())
        elif instruction.get_op_code() == "TYPE":
            self.__intepret_type(instruction.get_arguments(), instruction.get_order())
        elif instruction.get_op_code() == "JUMP":
            return self.__intepret_jump(instruction.get_arguments(), instruction.get_order())
        elif instruction.get_op_code() == "JUMPIFEQ":
            return self.__intepret_jumpifeq(instruction.get_arguments(), instruction.get_order())
        elif instruction.get_op_code() == "JUMPIFNEQ":
            return self.__intepret_jumpifneq(instruction.get_arguments(), instruction.get_order())
        elif instruction.get_op_code() == "DPRINT":
            self.__intepret_dprint(instruction.get_arguments(), instruction.get_order())
        elif instruction.get_op_code() == "BREAK":
            self.__intepret_break(instruction.get_arguments(), instruction.get_order())
        elif instruction.get_op_code() != "LABEL":
            print("Wrong instruction opcode", file=sys.stderr)
            exit(32)
        return int(instruction.get_order()) + 1

    def interpret_program(self):
        instructions = self.__program.get_instructions()
        if len(instructions) == 0:
            return
        if "1" not in instructions:
            print("Missing instruction with order 1", file=sys.stderr)
            exit(31)
        self.__get_labels(instructions)
        instruction = instructions[str(1)]
        while instruction:
            instruction_order = self.__interpret_instruction(instruction)
            if instruction_order > len(instructions):
                return
            if str(instruction_order) not in instructions:
                print("Missing instruction with order " + str(instruction_order), file=sys.stderr)
                exit(31)
            instruction = instructions[str(instruction_order)]
        return

    def __get_labels(self, instructions):
        for ins_order in instructions:
            ins = instructions[ins_order]
            if ins.get_op_code() == "LABEL":
                args = ins.get_arguments()
                self.__check_instruction_arguments_count(args, 1, ins.get_order())
                self.__program.add_label(args[1].get_value(), ins.get_order())

    def __check_instruction_arguments_count(self, args, correct_count, instruction_order):
        if len(args) != correct_count:
            print("Wrong arguments count in instruction with order " + str(instruction_order), file=sys.stderr)
            exit(32)

    def __intepret_move(self, arguments, instruction_order):
        self.__check_instruction_arguments_count(arguments, 2, instruction_order)
        self.__check_variable_name(arguments[1])
        self.__check_symbol(arguments[2])
        value, frame = self.__split_variable(arguments[1].get_value())
        arg_type, arg_value = self.__get_argument_type_value(arguments[2])
        self.__memory_manager.change_variable_value(value, arg_value, arg_type, frame)

    def __intepret_createframe(self, arguments, instruction_order):
        self.__check_instruction_arguments_count(arguments, 0, instruction_order)
        self.__memory_manager.create_frame()

    def __intepret_pushframe(self, arguments, instruction_order):
        self.__check_instruction_arguments_count(arguments, 0, instruction_order)
        self.__memory_manager.push_frame()

    def __intepret_popframe(self, arguments, instruction_order):
        self.__check_instruction_arguments_count(arguments, 0, instruction_order)
        self.__memory_manager.pop_frame()

    def __intepret_defvar(self, arguments, instruction_order):
        self.__check_instruction_arguments_count(arguments, 1, instruction_order)
        argument = arguments[1]
        if argument.get_type() != "var":
            print("Argument must be type of var in instruction with order " + str(instruction_order), file=sys.stderr)
            exit(32)
        self.__check_variable_name(argument)
        variable = Variable.Variable()
        variable_name, frame = self.__split_variable(argument.get_value())
        variable.set_name(variable_name)
        self.__memory_manager.add_variable(variable, frame)

    def __intepret_call(self, arguments, instruction_order):
        self.__check_instruction_arguments_count(arguments, 1, instruction_order)
        self.__memory_manager.save_pc_to_stack(int(instruction_order) + 1)
        return int(self.__program.get_label(arguments[1].get_value())) + 1

    def __intepret_return(self, arguments, instruction_order):
        self.__check_instruction_arguments_count(arguments, 0, instruction_order)
        return self.__memory_manager.get_pc_from_stack()

    def __intepret_pushs(self, arguments, instruction_order):
        if len(arguments) != 1:
            print("Wrong count of arguments for pushs instruction with order " + str(instruction_order),
                  file=sys.stderr)
            exit(32)
        self.__check_symbol(arguments[1])
        item = DataStackItem.DataStackItem()
        if arguments[1].get_type() != "var":
            item.set_value(arguments[1].get_value())
            item.set_type(arguments[1].get_type())
        else:
            variable_name, frame = self.__split_variable(arguments[1].get_value())
            variable = self.__memory_manager.get_variable(variable_name, frame)
            item.set_value(variable.get_value())
            item.set_type(variable.get_type())
        self.__memory_manager.stack_push(item)

    def __intepret_pops(self, arguments, instruction_order):
        self.__check_instruction_arguments_count(arguments, 1, instruction_order)
        stack_variable = self.__memory_manager.stack_pop()
        self.__check_variable_name(arguments[1])
        variable_name, frame = self.__split_variable(arguments[1].get_value())
        self.__memory_manager.change_variable_value(variable_name, stack_variable.get_value(),
                                                    stack_variable.get_type(), frame)

    def __intepret_add(self, arguments, instruction_order):
        value1, value2 = self.__get_arithmetic_operators_value(arguments, instruction_order, "add")
        res = int(value1) + int(value2)
        variable = arguments[1]
        self.__write_value_to_variable(res, variable, "int")

    def __write_value_to_variable(self, res, variable, type):
        self.__check_variable_name(variable)
        variable_name, frame = self.__split_variable(variable.get_value())
        self.__memory_manager.change_variable_value(variable_name, res, type, frame)

    def __get_arithmetic_operators_value(self, arguments, instruction_order, instruction_name):
        if len(arguments) != 3:
            print("Wrong count of arguments for " + instruction_name + " instruction with order " + str(
                instruction_order), file=sys.stderr)
            exit(32)
        self.__check_symbol(arguments[2])
        self.__check_symbol(arguments[3])
        value1 = self.__get_int_value(arguments[2], instruction_order)
        value2 = self.__get_int_value(arguments[3], instruction_order)
        return value1, value2

    def __get_int_value(self, argument, instruction_order):
        if argument.get_type() == "int":
            value = argument.get_value()
        elif argument.get_type() == "var":
            variable_name, frame = self.__split_variable(argument.get_value())
            variable = self.__memory_manager.get_variable(variable_name, frame)
            if variable.get_type() != "int":
                print("Argument must be type of int in instruction with order " + str(instruction_order),
                      file=sys.stderr)
                exit(53)
            value = variable.get_value()
        return value

    def __intepret_sub(self, arguments, instruction_order):
        value1, value2 = self.__get_arithmetic_operators_value(arguments, instruction_order, "sub")
        res = int(value1) - int(value2)
        variable = arguments[1]
        self.__write_value_to_variable(res, variable, "int")

    def __intepret_mul(self, arguments, instruction_order):
        value1, value2 = self.__get_arithmetic_operators_value(arguments, instruction_order, "mul")

        res = int(value1) * int(value2)
        variable = arguments[1]
        self.__write_value_to_variable(res, variable, "int")

    def __intepret_idiv(self, arguments, instruction_order):
        value1, value2 = self.__get_arithmetic_operators_value(arguments, instruction_order, "idiv")
        if int(value2) == 0:
            print("Division with zero in instruction with order " + str(instruction_order),
                  file=sys.stderr)
            exit(53)
        res = int(int(value1) / int(value2))
        variable = arguments[1]
        self.__write_value_to_variable(res, variable, "int")

    def __intepret_lt(self, arguments, instruction_order):
        if len(arguments) != 3:
            print("Wrong count of arguments for lt instruction with order " + str(
                instruction_order), file=sys.stderr)
            exit(32)
        value1, value2, type = self.__get_values_with_same_type(arguments, instruction_order, "lt")
        value = "false"
        if type == "string":
            value = "true" if (value1 < value2) else "false"
        elif type == "int":
            value = "true" if (int(value1) < int(value2)) else "false"
        elif type == "bool":
            value = "true" if (value1 == "false" and value2 == "true") else "false"
        self.__check_variable_name(arguments[1])
        variable_name, frame = self.__split_variable(arguments[1].get_value())
        self.__memory_manager.change_variable_value(variable_name, value, type, frame)

    def __intepret_gt(self, arguments, instruction_order):
        if len(arguments) != 3:
            print("Wrong count of arguments for gt instruction with order " + str(
                instruction_order), file=sys.stderr)
            exit(32)
        value1, value2, type = self.__get_values_with_same_type(arguments, instruction_order, "gt")
        value = "false"
        if type == "string":
            value = "true" if (value1 > value2) else "false"
        elif type == "int":
            value = "true" if (int(value1) > int(value2)) else "false"
        elif type == "bool":
            value = "true" if (value1 == "true" and value2 == "false") else "false"
        self.__check_variable_name(arguments[1])
        variable_name, frame = self.__split_variable(arguments[1].get_value())
        self.__memory_manager.change_variable_value(variable_name, value, type, frame)

    def __intepret_eq(self, arguments, instruction_order):
        if len(arguments) != 3:
            print("Wrong count of arguments for eq instruction with order " + str(
                instruction_order), file=sys.stderr)
            exit(32)
        value1, value2, type = self.__get_values_with_same_type(arguments, instruction_order, "lt")
        value = "false"
        if type == "string":
            value = "true" if (value1 == value2) else "false"
        elif type == "int":
            value = "true" if (int(value1) == int(value2)) else "false"
        elif type == "bool":
            value = "true" if (value1 == value2) else "false"
        self.__check_variable_name(arguments[1])
        variable_name, frame = self.__split_variable(arguments[1].get_value())
        self.__memory_manager.change_variable_value(variable_name, value, type, frame)

    def __intepret_and(self, arguments, instruction_order):
        value1, value2 = self.__get_logic_operators_value(arguments, instruction_order, "and")
        res = "true" if (value1 == "true" and value2 == "true") else "false"
        variable = arguments[1]
        self.__write_value_to_variable(res, variable, "bool")

    def __intepret_or(self, arguments, instruction_order):
        value1, value2 = self.__get_logic_operators_value(arguments, instruction_order, "and")
        res = "true" if (value1 == "true" or value2 == "true") else "false"
        variable = arguments[1]
        self.__write_value_to_variable(res, variable, "bool")

    def __intepret_not(self, arguments, instruction_order):
        if len(arguments) != 2:
            print("Wrong count of arguments for not instruction with order " + str(
                instruction_order), file=sys.stderr)
            exit(32)
        self.__check_symbol(arguments[2])
        value = self.__get_bool_value(arguments[2], instruction_order)
        res = "true" if (value == "false") else "false"
        self.__write_value_to_variable(res, arguments[1], "bool")

    def __intepret_int2char(self, arguments, instruction_order):
        self.__check_instruction_arguments_count(arguments, 2, instruction_order)
        self.__check_variable_name(arguments[1])
        type, value = self.__get_argument_type_value(arguments[2])
        if type == "int":
            variable_name, frame = self.__split_variable(arguments[1])
            self.__memory_manager.change_variable_value(variable_name, str(chr(value)), "string", frame)
        else:
            print("Wrong symbol type for int2char instruction with order " + str(instruction_order), file=sys.stderr)
            exit(53)

    def __intepret_stri2int(self, arguments, instruction_order):
        self.__check_instruction_arguments_count(arguments, 3, instruction_order)
        self.__check_variable_name(arguments[1])
        type1, value1 = self.__get_argument_type_value(arguments[2])
        type2, value2 = self.__get_argument_type_value(arguments[3])

        if type1 == "string" and type2 == "int":
            variable_name, frame = self.__split_variable(arguments[1])
            if int(value2) < len(value1):
                self.__memory_manager.change_variable_value(variable_name, int(value1[int(value2)]), "int", frame)
            else:
                print("Wrong index for str2int instruction with order " + str(instruction_order), file=sys.stderr)
                exit(58)
        else:
            print("Wrong symbol type for str2int instruction with order " + str(instruction_order), file=sys.stderr)
            exit(53)

        self.__check_symbol(arguments[3])

    def __intepret_read(self, arguments, instruction_order):
        if len(arguments) != 2:
            print("Wrong count of arguments for read instruction with order " + str(instruction_order), file=sys.stderr)
            exit(32)

        self.__check_variable_name(arguments[1])
        variable_name, frame = self.__split_variable(arguments[1].get_value())
        variable = self.__memory_manager.get_variable(variable_name, frame)
        if arguments[2].get_type() != "type":
            print("Second argument of read must be type in instruction with order " + str(instruction_order),
                  file=sys.stderr)
            exit(32)
        read_type = arguments[2].get_value()
        if read_type == "int":
            value = self.__read_int(instruction_order, variable)
            self.__memory_manager.change_variable_value(variable_name, value, "int", frame)
        elif read_type == "bool":
            value = self.__read_bool(instruction_order, variable)
            self.__memory_manager.change_variable_value(variable_name, value, "bool", frame)
        elif read_type == "string":
            value = self.__read_string(instruction_order, variable)
            self.__memory_manager.change_variable_value(variable_name, value, "string", frame)
        else:
            print("Wrong type", file=sys.stderr)
            exit(32)

    def __read_bool(self, instruction_order, variable):
        if variable.get_type() != "bool" and variable.get_type():
            print("Variable is not bool or noninicializate in instruction with order " + str(instruction_order),
                  file=sys.stderr)
            exit(32)
        i = input()
        value = "false"
        if i.lower() == "true":
            value = "true"
        return value

    def __read_int(self, instruction_order, variable):
        if variable.get_type() != "int" and variable.get_type():
            print("Variable is not int or noninicializate in instruction with order " + str(instruction_order),
                  file=sys.stderr)
            exit(32)
        i = input()
        value = 0
        try:
            value = int(i)
        except:
            value = 0
        return value

    def __intepret_write(self, arguments, instruction_order):
        if len(arguments) != 1:
            print("Wrong count of arguments for write instruction with order " + str(instruction_order),
                  file=sys.stderr)
            exit(32)
        argument = arguments[1]
        self.__check_symbol(argument)
        arg_type = argument.get_type()
        if arg_type == "bool":
            print(argument.get_value())
        elif arg_type == "int":
            print(int(argument.get_value()))
        elif arg_type == "string":
            print(argument.get_value())
        elif arg_type == "var":
            variable_name, frame = self.__split_variable(argument.get_value())
            variable = self.__memory_manager.get_variable(variable_name, frame)
            self.__print_variable(variable)

    def __intepret_concat(self, arguments, instruction_order):
        if len(arguments) != 3:
            print("Wrong count of arguments for concat instruction with order " + str(instruction_order),
                  file=sys.stderr)
            exit(32)
        value1, value2, type = self.__get_values_with_same_type(arguments, instruction_order, "concat")
        if type != "string":
            print("Arguments must be string in concat instruction with order " + str(instruction_order),
                  file=sys.stderr)
            exit(53)
        res = value1 + value2
        self.__write_value_to_variable(res, arguments[1], "string")

    def __intepret_strlen(self, arguments, instruction_order):
        self.__check_instruction_arguments_count(arguments, 2, instruction_order)
        value = self.__get_string_value(arguments[2], instruction_order)
        if type != "string":
            print("Arguments must be string in instruction with order " + str(instruction_order),
                  file=sys.stderr)
            exit(53)
        res = len(value)
        self.__write_value_to_variable(res, arguments[1].get_value(), "int")

    def __intepret_getchar(self, arguments, instruction_order):
        self.__check_instruction_arguments_count(arguments, 3, instruction_order)
        self.__check_variable_name(arguments[1])
        type1, value1 = self.__get_argument_type_value(arguments[2])
        type2, value2 = self.__get_argument_type_value(arguments[3])
        if type1 == "string" and type2 == "int":
            if len(value1) <= int(value2):
                print("Bad string index in instruction with order " + str(instruction_order),
                      file=sys.stderr)
                exit(58)
            res = value1[int(value2)]
            self.__write_value_to_variable(res, arguments[1].get_value(), "string")
        else:
            print("Wrong type of arguments for getchar instruction with order " + str(instruction_order),
                  file=sys.stderr)
            exit(53)

    def __intepret_setchar(self, arguments, instruction_order):
        self.__check_instruction_arguments_count(arguments, 3, instruction_order)
        self.__check_variable_name(arguments[1])
        type1, value1 = self.__get_argument_type_value(arguments[2])
        type2, value2 = self.__get_argument_type_value(arguments[3])
        if type1 == "int" and type2 == "string":
            variable_name, frame = self.__split_variable(arguments[1])
            variable = self.__memory_manager.get_variable(variable_name, frame)
            if variable.get_type() == "string":
                value = variable.get_value()
                value[value1] = value2[0]
                variable.set_value(value)
            else:
                print("Wrong variable type for variable in setchar instruction with order " + str(instruction_order),
                      file=sys.stderr)
                exit(53)
        else:
            print("Wrong type of arguments for setchar instruction with order " + str(instruction_order),
                  file=sys.stderr)
            exit(53)

    def __intepret_type(self, arguments, instruction_order):
        self.__check_instruction_arguments_count(arguments, 2, instruction_order)
        type, value = self.__get_argument_type_value(arguments[2])
        self.__write_value_to_variable(type, arguments[1], "string")

    def __intepret_jump(self, arguments, instruction_order):
        self.__check_instruction_arguments_count(arguments, 1, instruction_order)
        return int(self.__program.get_label(arguments[1].get_value())) + 1

    def __intepret_jumpifeq(self, arguments, instruction_order):
        self.__check_instruction_arguments_count(arguments, 3, instruction_order)
        value1, value2, type = self.__get_values_with_same_type(arguments, instruction_order, "jumpifeq")
        label = self.__program.get_label(arguments[1].get_value())
        if str(value1) == str(value2):
            return int(label) + 1
        else:
            return int(instruction_order) + 1

    def __intepret_jumpifneq(self, arguments, instruction_order):
        self.__check_instruction_arguments_count(arguments, 3, instruction_order)
        value1, value2, type = self.__get_values_with_same_type(arguments, instruction_order, "jumpifneq")
        label = self.__program.get_label(arguments[1].get_value())
        if str(value1) != str(value2):
            return int(label) + 1
        else:
            return int(instruction_order) + 1

    def __intepret_dprint(self, arguments, instruction_order):
        self.__check_instruction_arguments_count(arguments, 1, instruction_order)
        type, value = self.__get_argument_type_value(arguments[1])
        print(value, file=sys.stderr)

    def __intepret_break(self, arguments, instruction_order):
        print("Instruction order: " + instruction_order + "\n", file=sys.stderr)
        global_frame = self.__memory_manager.get_global_frame()
        print("Global Frame:\n------------------")
        for x in global_frame:
            act = global_frame[x]
            print("Name: " + act.get_name() + "\nType: " + act.get_type() + "\nValue: " + act.get_value())

        temporary_frame = self.__memory_manager.get_temporary_frame()
        print("Temporary Frame:\n------------------")
        if temporary_frame != None:
            for x in temporary_frame:
                act = temporary_frame[x]
                print("Name: " + act.get_name() + "\nType: " + act.get_type() + "\nValue: " + act.get_value())

        local_frames = self.__memory_manager.get_local_frames()
        for local_frame in local_frames:
            print("Local Frame:\n------------------")
            for var in local_frame:
                act = local_frame[var]
                print("Name: " + act.get_name() + "\nType: " + act.get_type() + "\nValue: " + act.get_value())

    def __check_symbol(self, argument):
        if argument.get_type() == "var":
            self.__check_variable_name(argument)
        elif argument.get_type() == "int":
            try:
                int(argument.get_value())
            except:
                print("Wrong int format", file=sys.stderr)
                exit(32)

        elif argument.get_type() == "string":
            pass
        elif argument.get_type() == "bool":
            if argument.get_value() != "true" and argument.get_value() != "false":
                print("Wrong bool value, must be true or false", file=sys.stderr)
                exit(32)
        else:
            print("Expected symbol", file=sys.stderr)
            exit(53)

    def __check_variable_name(self, argument):
        pattern = re.compile('(LF|GF|TF)@([a-z]|[A-Z]|-|_|&|%|[*]|[$])([a-z]|[A-Z]|-|_|&|%|[*]|[$]|[0-9])*')
        if not pattern.match(argument.get_value()):
            print("Wrong variable format", file=sys.stderr)
            exit(32)

    def __split_variable(self, argument):
        return argument[3:], argument[0:2]

    def __print_variable(self, variable):
        var_type = variable.get_type()
        if var_type == "bool":
            print(variable.get_value())
        elif var_type == "int":
            print(variable.get_value())
        elif var_type == "string":
            print(variable.get_value())

    def __check_variable_type(self, argument):
        variable_name, frame = self.__split_variable(argument)
        variable = self.__memory_manager.get_variable(variable_name, frame)
        return variable.get_type()

    def __get_logic_operators_value(self, arguments, instruction_order, instruction_name):
        if len(arguments) != 3:
            print("Wrong count of arguments for " + instruction_name + " instruction with order " + str(
                instruction_order), file=sys.stderr)
            exit(32)
        self.__check_symbol(arguments[2])
        self.__check_symbol(arguments[3])
        value1 = self.__get_bool_value(arguments[2], instruction_order)
        value2 = self.__get_bool_value(arguments[3], instruction_order)
        return value1, value2

    def __get_bool_value(self, argument, instruction_order):
        if argument.get_type() == "bool":
            value = argument.get_value()
        elif argument.get_type() == "var":
            variable_name, frame = self.__split_variable(argument.get_value())
            variable = self.__memory_manager.get_variable(variable_name, frame)
            if variable.get_type() != "bool":
                print("Argument must be type of bool in instruction with order " + str(instruction_order),
                      file=sys.stderr)
                exit(53)
            value = variable.get_value()
        return value

    def __get_string_value(self, argument, instruction_order):
        if argument.get_type() == "string":
            value = argument.get_value()
        elif argument.get_type() == "var":
            variable_name, frame = self.__split_variable(argument.get_value())
            variable = self.__memory_manager.get_variable(variable_name, frame)
            if variable.get_type() != "string":
                print("Argument must be type of string in instruction with order " + str(instruction_order),
                      file=sys.stderr)
                exit(53)
            value = variable.get_value()
        return value

    def __get_values_with_same_type(self, arguments, instruction_order, instruction_name):
        self.__check_symbol(arguments[2])
        self.__check_symbol(arguments[3])
        type1, value1 = self.__get_argument_type_value(arguments[2])
        type2, value2 = self.__get_argument_type_value(arguments[3])
        if type1 == type2:
            return value1, value2, type1
        print(
            "Arguments must be same type in instruction " + instruction_name + " with order " + str(instruction_order),
            file=sys.stderr)
        exit(53)

    def __get_argument_type_value(self, argument):
        if argument.get_type() != "var":
            if argument.get_type() == "string":
                argument.set_value(self.__convert_string(argument.get_value()))
            return argument.get_type(), argument.get_value()
        else:
            variable_name, frame = self.__split_variable(argument.get_value())
            variable = self.__memory_manager.get_variable(variable_name, frame)
            return variable.get_type(), variable.get_value()

    def __read_string(self, instruction_order, variable):
        if variable.get_type() != "string" and variable.get_type():
            print("Variable is not string or noninicializate in instruction with order " + str(instruction_order),
                  file=sys.stderr)
            exit(32)
        value = input()
        return value

    def __convert_string(self, value):
        state = 0
        count = 0
        res = ""
        number = 0

        for x in value:
            if x == '#' or x.isspace():
                print("Wrong string format", file=sys.stderr)
                exit()
            if state == 0:
                if x == '\\':
                    state = 1
                else:
                    res += x
            elif state == 1:
                if int(x) >= int('0') and int(x) <= int('9'):
                    number += int(x) * pow(10, 2 - count)
                    count += 1
                if count == 3:
                    res += chr(number)
                    count = 0
                    state = 0
                    number = 0
        return res;
