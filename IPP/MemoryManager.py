import sys


class MemoryManager:
    def __init__(self):
        self.__global_frame = dict()
        self.__temporary_frame = None
        self.__local_frames = list()
        self.__stack = list()
        self.__pc_stack = list()

    def get_global_frame(self):
        return self.__global_frame

    def get_temporary_frame(self):
        return self.__temporary_frame

    def get_local_frames(self):
        return self.__local_frames

    def save_pc_to_stack(self, value):
        self.__pc_stack.append(value)

    def get_pc_from_stack(self):
        if len(self.__pc_stack) > 0:
            return self.__pc_stack.pop()
        else:
            print("PC Stack is empty", file=sys.stderr)
            exit(56)

    def stack_top(self):
        if self.__stack.count() >= 1:
            return self.__stack[-1]
        else:
            return None

    def stack_pop(self):
        if len(self.__stack) > 0:
            return self.__stack.pop()
        else:
            print("Stack is empty", file=sys.stderr)
            exit(56)

    def stack_push(self, value):
        self.__stack.append(value)

    def create_frame(self):
        self.__temporary_frame = dict()

    def push_frame(self):
        if self.__temporary_frame != None:
            self.__local_frames.append(dict(self.__temporary_frame))
            self.__temporary_frame = None
        else:
            print("Access to undefined temporary frame", file=sys.stderr)
            exit(55)

    def pop_frame(self):
        if 1 <= len(self.__local_frames):
            self.__temporary_frame = self.__local_frames.pop()
        else:
            print("No local frame in stack", file=sys.stderr)
            exit(55)

    def add_variable(self, variable, frame):
        if frame == "GF":
            if variable.get_name() not in self.__global_frame:
                self.__global_frame[variable.get_name()] = variable
            else:
                print("Variable is already defined in global frame", file=sys.stderr)
                exit()
        elif frame == "LF":
            if variable.get_name() not in self.__local_frames[-1]:
                self.__local_frames[-1][variable.get_name()] = variable
            else:
                print("Variable is already defined", file=sys.stderr)
                exit()
        elif frame == "TF":
            if variable.get_name() not in self.__temporary_frame:
                self.__temporary_frame[variable.get_name()] = variable
            else:
                print("Variable is already defined in temporary frame", file=sys.stderr)
                exit()
        else:
            print("Wrong frame", file=sys.stderr)
            exit()

    def get_variable(self, variable_name, frame):
        if frame == "GF":
            if variable_name in self.__global_frame:
                return self.__global_frame[variable_name]
            else:
                print("Variable " + variable_name + " is not in global frame", file=sys.stderr)
                exit(54)
        elif frame == "LF":
            if variable_name in self.__local_frames[-1]:
                return self.__local_frames[-1][variable_name]
            else:
                print("Variable " + variable_name + " is not in local frame", file=sys.stderr)
                exit(54)
        elif frame == "TF":
            if variable_name in self.__temporary_frame:
                return self.__temporary_frame[variable_name]
            else:
                print("Variable " + variable_name + " is not in temporary frame", file=sys.stderr)
                exit(54)
        else:
            print("Wrong frame", file=sys.stderr)
            exit()

    def change_variable_value(self, variable_name, value, type, frame):
        if frame == "GF":
            self.__global_frame[variable_name].set_value(value)
            self.__global_frame[variable_name].set_type(type)
        elif frame == "LF":
            self.__local_frames[-1][variable_name].set_value(value)
            self.__local_frames[-1][variable_name].set_type(type)
        elif frame == "TF":
            self.__temporary_frame[variable_name].set_value(value)
            self.__temporary_frame[variable_name].set_type(type)
        else:
            print("Wrong frame", file=sys.stderr)
            exit(99)
