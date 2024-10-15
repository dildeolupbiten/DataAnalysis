from os import path, name
from ctypes import CDLL, POINTER, c_long, c_char_p, c_int, c_double, byref


class DataAnalysis:
    class DataFrame:
        def __init__(self, nrows, ncols, data):
            self.nrows = nrows
            self.ncols = ncols
            self.data = data

        def __str__(self):
            s = ""
            for i in range(self.nrows):
                for j in range(self.ncols):
                    end = "," if j + 1 != self.ncols else ""
                    s += self.data[i][j].decode("utf-8") + end
            return s
            
    class Matrix(DataFrame):
        def __str__(self):
            s = ""
            for i in range(self.nrows):
                for j in range(self.ncols):
                    end = " " if j + 1 != self.ncols else "\n"
                    s += str(self.data[i][j]) + end
            return s 
            
    def __init__(self):
        self.__lib = CDLL(
            path.split(path.abspath(__name__))[0] +
            f"/DataAnalysis/lib/data_analysis." +
            f"{'so' if name == 'posix' else 'dll'}"
        )
        self.__init__read_csv()
        self.__init__label_encoder()
        self.__init__one_hot_encoder()
        self.__init__to_matrix()
        self.__init__test_train_split()
        self.__init__linear_regression()
        self.__init__predict()
        
    def read_csv(self, filename: str, delimiter: str):
        nrows = c_long(0)
        ncols = c_long(0)
        data = self.__lib.read_csv(
            filename.encode("utf-8"), 
            byref(nrows), 
            byref(ncols),
            delimiter.encode("utf-8")
        )
        return self.DataFrame(nrows.value, ncols.value, data)
        
    def label_encoder(self, csv: 'Predictions.DataFrame', columns: list[int]):
        size = c_int(len(columns))
        columns = (c_int * size.value)(*columns)
        ncols = c_long(csv.ncols)
        data = self.__lib.label_encoder(
            c_long(csv.nrows),
            byref(ncols),
            csv.data,
            columns,
            size
        )
        return self.DataFrame(csv.nrows, ncols.value, data)
        
    def one_hot_encoder(
        self, csv: 'Predictions.DataFrame', 
        columns: list[int]
    ):
        size = c_int(len(columns))
        columns = (c_int * size.value)(*columns)
        ncols = c_long(csv.ncols)
        data = self.__lib.one_hot_encoder(
            c_long(csv.nrows),
            byref(ncols),
            csv.data,
            columns,
            size
        )
        return self.DataFrame(csv.nrows, ncols.value, data)
        
    def to_matrix(self, csv: 'Predictions.DataFrame'):
        data = self.__lib.to_matrix(
            csv.data, 
            c_long(csv.nrows), 
            c_long(csv.ncols)
        )
        return self.Matrix(csv.nrows, csv.ncols, data)
        
    def test_train_split(
        self, 
        matrix: 'Predictions.Matrix', 
        columns: list[int], 
        ratio: float
    ):
        size = c_int(len(columns))
        columns = (c_int * size.value)(*columns)
        splitted = self.__lib.test_train_split(
            matrix.data,
            c_long(matrix.nrows), 
            c_long(matrix.ncols), 
            columns, 
            size, 
            c_double(ratio)
        )
        train_size = int(matrix.nrows * (1 - ratio))
        test_size = matrix.nrows - train_size
        return (
            self.Matrix(train_size, matrix.ncols - size.value, splitted[0]),
            self.Matrix(train_size, size.value, splitted[1]),
            self.Matrix(test_size, matrix.ncols - size.value, splitted[2]),
            self.Matrix(test_size, size.value, splitted[3]),
        )
        
    def linear_regression(
        self, 
        x_train: 'Predictions.Matrix', 
        y_train: 'Predictions.Matrix', 
        bias_term: int
    ):
        data = self.__lib.linear_regression(
            x_train.data, 
            y_train.data,  
            c_long(x_train.nrows), 
            (c_long * 2)(*[x_train.ncols, y_train.ncols]),
            c_int(bias_term)
        )
        return self.Matrix(x_train.ncols, y_train.ncols, data)
        
    def predict(self, x_test, y_test, beta, bias_term):
        data = self.__lib.predict(
            x_test.data, 
            beta.data,  
            c_long(x_test.nrows), 
            (c_long * 2)(*[x_test.ncols, y_test.ncols]),
            c_int(bias_term)
        )
        return self.Matrix(x_test.nrows, y_test.ncols, data)
        
    def __init__read_csv(self):
        self.__lib.read_csv.argtypes = [
            c_char_p, 
            POINTER(c_long), 
            POINTER(c_long),
            c_char_p
        ]
        self.__lib.read_csv.restype = POINTER(POINTER(c_char_p))
        
    def __init__label_encoder(self):
        self.__lib.label_encoder.argtypes = [
            c_long, 
            POINTER(c_long), 
            POINTER(POINTER(c_char_p)), 
            POINTER(c_int), 
            c_int
        ]
        self.__lib.label_encoder.restype = POINTER(POINTER(c_char_p))
        
    def __init__one_hot_encoder(self):
        self.__lib.one_hot_encoder.argtypes = [
            c_long, 
            POINTER(c_long), 
            POINTER(POINTER(c_char_p)), 
            POINTER(c_int), 
            c_int
        ]
        self.__lib.one_hot_encoder.restype = POINTER(POINTER(c_char_p))
        
    def __init__to_matrix(self):
        self.__lib.to_matrix.argtypes = [
            POINTER(POINTER(c_char_p)), 
            c_long, 
            c_long
        ]
        self.__lib.to_matrix.restype = POINTER(POINTER(c_double))
        
    def __init__test_train_split(self):
        self.__lib.test_train_split.argtypes = [
            POINTER(POINTER(c_double)), 
            c_long, 
            c_long, 
            POINTER(c_int), 
            c_int, 
            c_double
        ]
        self.__lib.test_train_split.restype = \
            POINTER(POINTER(POINTER(c_double)))
            
    def __init__linear_regression(self):
        self.__lib.linear_regression.argtypes = [
            POINTER(POINTER(c_double)), 
            POINTER(POINTER(c_double)), 
            c_long, 
            POINTER(c_long), 
            c_int
        ]
        self.__lib.linear_regression.restype = POINTER(POINTER(c_double))
        
    def __init__predict(self):
        self.__lib.predict.argtypes = [
            POINTER(POINTER(c_double)), 
            POINTER(POINTER(c_double)), 
            c_long, 
            POINTER(c_long), 
            c_int
        ]
        self.__lib.predict.restype = POINTER(POINTER(c_double))

