class DataStackItem:

    def __init__(self):
        self.__type = None
        self.__value = None

    def set_type(self, value):
        self.__type = value

    def set_value(self, value):
        self.__value = value

    def get_type(self):
        return self.__type

    def get_value(self):
        return self.__value
