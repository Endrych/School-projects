class Variable:

    def __init__(self):
        self.__name = ""
        self.__type = None
        self.__value = None

    def set_name(self, value):
        self.__name = value

    def set_value(self, value):
        self.__value = value

    def set_type(self, value):
        self.__type = value

    def get_name(self):
        return self.__name

    def get_value(self):
        return self.__value

    def get_type(self):
        return self.__type
