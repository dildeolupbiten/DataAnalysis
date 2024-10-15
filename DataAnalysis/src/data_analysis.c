#include <stdio.h>
#include <stdlib.h>

int greater(int a, int b) { return a == b ? -1 : a > b; }

int less(int a, int b) { return a == b ? -1 : a < b; }

int type_compare(void *a, void *b, long type, int (*func)(int, int)) {
    if (type == sizeof(int)) {
        return func(*((int *)a), *((int *)b));
    } else if (type == sizeof(double)) {
        return func(*((double *)a), *((double *)b));
    } else {
        for (size_t i = 0; i < type; i++) {
            int compared = func(
                *((unsigned char *)a + i), 
                *((unsigned char *)b + i)
            );
            if (compared == 0) { return 0; }
            else if (compared == 1) { return 1; }
        }
        return -1;
    }
}

long resize(long type, long *dims, int dim) {
    long size = type;
    for (int i = 1; i < dim; i++) { size *= dims[i]; }
    return size;
}

int compare(
    void *a, 
    void *b, 
    long type, 
    long *dims, 
    int dim, 
    int (*func)(int, int)
) {
    if (!dim) { return type_compare(a, b, type, func); }
    long size = resize(type, dims, dim);
    for (long i = 0; i < dims[0]; i++) {
        int compared = compare(
            a + i * size,
            b + i * size,
            type,
            dims + 1,
            dim - 1,
            func
        );
        if (compared == 0) { return 0; }
        else if (compared == 1) { return 1; }
    }
    return -1;
}

void copy(void *dst, void *src, long size, long length) {
    for (long i = 0; i < length; i++) {
        void *a = (unsigned char *)dst + i * size;
        void *b = (unsigned char *)src + i * size;
        for (long j = 0; j < size; j++) {
            *((unsigned char *)a + j) = *((unsigned char *)b + j);
        }
    }
}

int in(void **array, void *item, long type, long *dims, int dim) {
    for (int k = 0; k < dims[0]; k++) {
        int compared = compare(
            item, 
            array[k], 
            type, 
            dims + 1, 
            dim - 1, 
            greater
        );
        if (compared == -1) { return k; }
    }
    return -1;
}

void swap(void *dst, void *src, long type, long *dims, int dim) {
    long size = resize(type, dims, dim);
    for (long i = 0; i < dims[0]; i++) {
        void *a = (unsigned char *)dst + size * i;
        void *b = (unsigned char *)src + size * i;
        for (long j = 0; j < size; j++) {
            char temp = *((unsigned char *)a + j);
            *((unsigned char *)a + j) = *((unsigned char *)b + j);
            *((unsigned char *)b + j) = temp;  
        }
    }
}

double power(double n, int r) {
    double result = 1;
    while (r) { result *= n; r--; }
    return result;
}

long str_len(char *str) {
    long i = 0;
    while (str[i] != '\0') { i++; }
    return i;
}

int is_null(void *p, const char *var_name, const char *func_name) {
    if (p == NULL) { 
        printf(
            "Memory allocation failed for %s in %s function.\n", 
            var_name, func_name
        );
        return 1; 
    }
    return 0;
}

double *divmod(double x, double y) {
    double *result = malloc(2 * sizeof(double));
    if (is_null(result, "result", "divmod")) { return NULL; }
    double d = x / y;
    result[0] = (d < 0 && d != (int)d) ? (int)d - 1 : (int)d;
    result[1] = x - result[0] * y;
    return result;
}

double str2n(char *str) {
    int int_size = 0;
    int float_size = 0;
    int is_float = 0;
    int i = 0;
    int negative = 0;
    while (str[i] != '\0') {
        if (str[i] == '\n') {
            i += 1;
            continue;
        }
        if (str[i] == '.' || str[i] == ',') { 
            is_float++;
            i += 1;
            continue;
        }
        if (str[i] == '-') {
            negative = 1;
            i += 1;
            continue;
        }
        if (is_float) { float_size++; } else { int_size++; }
        i += 1;
    }
    double result = 0;
    int start = negative ? 1 : 0;
    for (int i = 0; i < int_size; i++) {
        int s = *((unsigned char *)str + i + start) - 48;
        result += power(10, int_size - i - 1) * s;
    }
    if (is_float) {
        for (int i = 0; i < float_size; i++) {
            int s = *((unsigned char *)str + int_size + i + 1 + start) - 48;
            result += s / power(10, i + 1);
        }
    }
    return result * (negative ? -1 : 1);
}

char *n2str(double n) {
    if (!n) {
        char *zero_str = malloc(2 * sizeof(char));
        if (is_null(zero_str, "zero_str", "n2str")) { return NULL; }
        zero_str[0] = '0';
        zero_str[1] = '\0';
        return zero_str;
    }
    int neg = n < 0 ? 1 : 0;
    if (neg) { n *= -1; };
    char *i_part = malloc(sizeof(char));
    if (is_null(i_part, "i_part", "n2st2")) { return NULL; }
    int i_count = 0;
    double f = n - (int)n;
    while (n) {
        double *qr = divmod(n, 10);
        char *i_temp = realloc(i_part, (i_count + 1) * sizeof(char));
        if (is_null(i_temp, "i_temp", "n2str")) { 
            free(i_part);
            return NULL; 
        }
        i_part = i_temp;
        i_part[i_count] = qr[1] + '0';
        i_count++;
        n = qr[0];
        free(qr);
    }
    int f_count = 0;
    char *f_part = malloc(sizeof(char));
    if (is_null(f_part, "f_part", "n2str")) {
        free(i_part);
        return NULL;
    }
    if (f) {
        char *f_temp = realloc(f_part, 2 * sizeof(char));
        if (is_null(f_temp, "f_temp", "n2str")) {
            free(i_part);
            free(f_part);
            return NULL;
        }
        f_part = f_temp;
        f_part[0] = '.';
        f_count++;
        double f_total = 0;
        double save = f;
        while (f_count - 1 < 15) {
            f *= 10;
            char *re_f_temp = realloc(f_part, (f_count + 1) * sizeof(char));
            if (is_null(re_f_temp, "re_f_temp", "n2str")) {
                free(i_part);
                free(f_part);
                return NULL;
            }
            f_part = re_f_temp;
            f_part[f_count] = ((int)f + '0');
            f_total += (int)f * power(.1, f_count);
            f -= (int)f;
            f_count++;
        }
    }
    char *str = malloc((neg + i_count + f_count + 1) * sizeof(char));
    if (is_null(str, "str", "n2str")) {
        free(i_part);
        free(f_part);
        return NULL;
    }
    if (neg) { str[0] = '-'; }
    for (int i = 0; i < i_count; i++) {
        str[i + neg] = i_part[i_count - 1 - i];
    }
    for (int i = i_count; i < i_count + f_count; i++) {
        str[i + neg] = f_part[i - i_count];
    }
    str[i_count + neg + f_count] = '\0';
    free(i_part);
    free(f_part);
    return str;
}

double **to_matrix(char ***data, long rows, long cols) {
    double **matrix = malloc(rows * sizeof(double *));
    if (is_null(matrix, "matrix", "to_matrix")) { return NULL; }
    for (long row = 0; row < rows; row++) {
        matrix[row] = malloc(cols * sizeof(double));
        if (is_null(matrix[row], "matrix[row]", "to_matrix")) {
            for (long i = 0; i < row; i++) {
                free(matrix[i]);
            }
            free(matrix);
            return NULL;
        }
        for (long col = 0; col < cols; col++) {
            matrix[row][col] = str2n(data[row][col]);
        }
    }
    return matrix;
}

double **transpose(double **mat, long rows, long cols) {
    double **transposed = malloc(cols * sizeof(double *));
    if (is_null(transposed, "transposed", "transpose")) { return NULL; }
    for (long col = 0; col < cols; col++) {
        transposed[col] = malloc(rows * sizeof(double));
        if (is_null(transposed[col], "transposed[col]", "transpose")) {
            for (long i = 0; i < col; i++) {
                free(transposed[i]);
            }
            free(transposed);
            return NULL;
        }
    }
    for (long row = 0; row < rows; row++) {
        for (long col = 0; col < cols; col++) { 
            transposed[col][row] = mat[row][col]; 
        }
    }
    return transposed;
}

double **matmul(double **mat1, double **mat2, long rows[2], long cols[2]) {
    double **result = malloc(rows[0] * sizeof(double *));
    if (is_null(result, "result", "matmul")) { return NULL; }
    for (long row = 0; row < rows[0]; row++) {
        result[row] = malloc(cols[1] * sizeof(double));
        if (is_null(result[row], "result[row]", "matmul")) {
            for (long i = 0; i < row; i++) {
                free(result[row]);
            }
            free(result);
            return NULL;
        }
        for (long col = 0; col < cols[1]; col++) {
            result[row][col] = 0;
            for (long k = 0; k < rows[1]; k++) {
                result[row][col] += mat1[row][k] * mat2[k][col];
            }
        }
    }
    return result;
}

int gauss_jordan_elimination(double **mat, long size) {
    for (long i = 0; i < size; i++) {
        int pivot = -1; 
        for (long j = i; j < size; j++) {
            if (mat[j][i]) { pivot = j; break; }
        }
        if (pivot == -1) { return 0; } 
        if (pivot != i) {
            swap(mat[i], mat[pivot], sizeof(double), (long[]){size}, 1);
        }
        double temp = mat[i][i];
        for (long j = 0; j < 2 * size; j++) { mat[i][j] /= temp; }
        for (long j = 0; j < size; j++) {
            temp = mat[j][i];
            if (i != j) {
                for (long k = 0; k < 2 * size; k++) {
                    mat[j][k] -= temp * mat[i][k];
                }
            }
        }
    }
    return 1;
}

double **augment(double **mat, long rows, long cols) {
    double **augmented = malloc(rows * sizeof(double **));
    if (is_null(augmented, "augmented", "augment")) { return NULL; }
    for (long row = 0; row < rows; row++) {
        augmented[row] = malloc(cols * 2 * sizeof(double));
        if (is_null(augmented[row], "augmented[row]", "augment")) {
            for (long i = 0; i < row; i++) {
                free(augmented[i]);
            }
            free(augmented);
            return NULL;
        }
        for (long j = 0; j < cols * 2; j++) {
            if (j < cols) {
                augmented[row][j] = mat[row][j];
            } else {
                augmented[row][j] = row == (j - cols) ? 1 : 0;
            }
        }
    }
    return augmented;
}

double **inverse(double **mat, long rows, long cols) {
    double **augmented = augment(mat, rows, cols);
    if (!gauss_jordan_elimination(augmented, rows)) {
        return NULL;
    }
    double **inversed = malloc(rows * sizeof(double *));
    if (is_null(inversed, "inversed", "inverse")) { return NULL; }
    for (long row = 0; row < rows; row++) {
        inversed[row] = malloc(cols * sizeof(double));
        if (is_null(inversed[row], "inversed[row]", "inverse")) {
            for (long i = 0; i < row; i++) {
                free(inversed[i]);
            }
            free(inversed);
            return NULL;
        }
        for (long col = 0; col < cols; col++) {
            inversed[row][col] = augmented[row][cols + col];
        }
    }
    return inversed;
}

double **add_bias_term(double **mat, long rows, long cols) {
    double **added = malloc(rows * sizeof(double *));
    if (is_null(added, "added", "add_bias_term")) { return NULL; }
    for (long row = 0; row < rows; row++) {
        added[row] = malloc((cols + 1) * sizeof(double));
        if (is_null(added[row], "added[row]", "add_bias_term")) {
            for (long i = 0; i < row; i++) {
                free(added[i]);
            }
            free(added);
            return NULL;
        }
        added[row][0] = 1;
        for (long col = 0; col < cols; col++) { 
            added[row][col + 1] = mat[row][col]; 
        }
    }
    return added;
}

double **linear_regression(
    double **X, 
    double **y, 
    long rows, 
    long *cols,
    int bias_term
) {
    double **Xt, **XtX;
    long c0 = cols[0];
    long c1 = cols[1];
    if (bias_term) {
        double **Xbt = add_bias_term(X, rows, c0);
        c0 += 1;
        Xt = transpose(Xbt, rows, c0);
        XtX = matmul(Xt, Xbt, (long[]){c0, rows}, (long[]){rows, c0});
    } else {
        Xt = transpose(X, rows, c0);
        XtX = matmul(Xt, X, (long[]){c0, rows}, (long[]){rows, c0});
    }
    double **XtY = matmul(Xt, y, (long[]){c0, rows}, (long[]){rows, c1});
    double **XtX_inv = inverse(XtX, c0, c0);
    if (!XtX_inv) { return NULL; }
    double **beta = matmul(XtX_inv, XtY, (long[]){c0, c0}, (long[]){c0, c1});
    return beta;
}

double **predict(
    double **X, 
    double **beta, 
    long rows, 
    long *cols, 
    int bias_term
) {
    double **predictions;
    if (bias_term) {
        double **X_bt = add_bias_term(X, rows, cols[0]);
        cols[0] += 1;
        predictions = matmul(X_bt, beta, (long[]){rows, cols[0]}, cols);
        free(X_bt);
    } else { predictions = matmul(X, beta, (long[]){rows, cols[0]}, cols);}
    return predictions;
}

char **split(char *str, char *sep) {
    int assign(char ***splitted, long count, long start, long end) {
        long len = (end - start + 1);
        char **temp = realloc(*splitted, (count + 1) * sizeof(char *));
        if (is_null(temp, "temp", "split.assign")) {
            for (long i = 0; i < count; i++) {
                free((*splitted)[i]);
            }
            free(*splitted);
            return 0;
        }
        *splitted = temp;
        (*splitted)[count] = malloc(len * sizeof(char));
        if (is_null((*splitted)[count], "(*splitted)[count]", "split")) {
            for (long i = 0; i < count; i++) {
                free((*splitted)[i]);
            }
            free(*splitted);
            return 0;
        }
        copy((*splitted)[count], &str[start], sizeof(char), len - 1);
        (*splitted)[count][len - 1] = '\0';
        return 1;
    }
    long sep_len = str_len(sep);
    int type = sizeof(char);
    long i = 0;
    long start = 0, end = 0;
    char **splitted = malloc(sizeof(char *));
    if (is_null(splitted, "splitted", "split")) { return NULL; }
    long count = 0;
    while (str[i] != '\0') {
        char *temp = malloc(sep_len * type);
        for (long j = 0; j <  sep_len; j++) {
            temp[j] = str[i + j];
        }
        if (compare(temp, sep, type, (long[]){sep_len}, 1, greater) == -1) {
            start = end ? end + sep_len : end;
            end = i;
            if (!assign(&splitted, count, start, end)) { return NULL; }
            count++;
        }
        i++;
    }
    start = end ? end + sep_len : end;
    end = i;
    if (!assign(&splitted, count, start, end)) { return NULL; }
    return splitted;
}

char ***label_encoder(
    long rows,
    long *cols,
    char ***array,
    int *columns, 
    int columns_size
) {
    char ***arr = malloc(rows * sizeof(char **));
    if (is_null(arr, "arr", "label_encoder")) { return NULL; }
    for (long row = 0; row < rows; row++) {
        arr[row] = malloc(*cols * sizeof(char *));
        if (is_null(arr[row], "arr[row]", "label_encoder")) {
            for (long i = 0; i < row; i++) {
                free(arr[i]);
            }
            free(arr);
            return NULL;
        }
        copy(arr[row], array[row], sizeof(char *), *cols);     
    }
    for (long i = 0; i < columns_size; i++) {
        int col = columns[i];
        char **unique = malloc(sizeof(char *));
        if (is_null(unique, "unique", "label_encoder")) {
            for (long row = 0; row < rows; row++) {
                free(arr[i]);
            }
            free(arr[i]);
        }
        int len = 0;
        for (int row = 0; row < rows; row++) {
            char *item = arr[row][col];
            long size = str_len(item);
            int index = in(
                (void **)unique, 
                item, 
                sizeof(char), 
                (long[]){len, size}, 
                2
            );
            if (index == -1) {
                unique = realloc(unique, (len + 1) * sizeof(char *));
                unique[len] = item;
                index = len;
                len++;
            }
            arr[row][col] = n2str(index);
        }
        free(unique);
    }
    return arr;
}

char ***one_hot_encoder(
    long rows,
    long *cols,
    char ***array,
    int *columns, 
    int columns_size
) {
    long col = 0;
    long size = 0;
    char ***arr = malloc(rows * sizeof(char **));
    for (long row = 0; row < rows; row++) {
        arr[row] = malloc(*cols * sizeof(char *));
        if (is_null(arr[row], "arr[row]", "label_encoder")) {
            for (long i = 0; i < row; i++) {
                free(arr[i]);
            }
            free(arr);
            return NULL;
        }
        copy(arr[row], array[row], sizeof(char *), *cols); 
    }
    for (long i = 0; i < columns_size; i++) {
        long c = col + columns[i];
        char **unique = malloc(sizeof(char *));
        if (is_null(unique, "unique", "one_hot_encoder")) {
            for (long row = 0; row < rows; row++) {
                free(arr[row]);
            }
            free(arr);
            return NULL;
        }
        int len = 0;
        for (int row = 0; row < rows; row++) {
            char *item = arr[row][c];
            long size = str_len(item);
            int index = in(
                (void **)unique, 
                item, 
                sizeof(char), 
                (long[]){len, size}, 
                2
            );
            if (index == -1) {
                char **u_tmp = realloc(unique, (len + 1) * sizeof(char *));
                if (is_null(u_tmp, "u_tmp", "one_hot_encoder")) {
                    for (long r = 0; r < rows; r++) {
                        free(arr[r]);
                    }
                    free(arr);
                    for (long l = 0; l < len; l++) {
                        free(unique[l]);
                    }
                    free(unique);
                    return NULL; 
                }
                unique = u_tmp;
                unique[len] = item;
                len++;
            }
        }
        size = len + *cols - 1;
        for (int row = 0; row < rows; row++) {
            char **tmp = malloc(size * sizeof(char *));
            if (is_null(tmp, "tmp", "one_hot_encoder")) {
                for (long r = 0; r < rows; r++) {
                    free(arr[r]);
                }
                free(arr);
                for (long l = 0; l < len; l++) {
                    free(unique[l]);
                }
                free(unique);
                return NULL; 
            }
            for (int k = 0; k < c; k++) {
                tmp[k] = arr[row][k];
            }
            for (int k = 0; k < len; k++) {
                int compared = compare(
                    arr[row][c],
                    unique[k],
                    sizeof(char),
                    (long[]){str_len(unique[k])},
                    1,
                    greater
                );
                tmp[k + c] = n2str(compared == -1);
            }
            for (int k = 0; k < *cols - c - 1; k++) {
                tmp[c + len + k] = arr[row][c + 1 + k];
            }
            char **arr_row_temp = realloc(arr[row], size * sizeof(char *));
            if (is_null(arr_row_temp, "arr_row_temp", "one_hot_encoder")) {
                for (long r = 0; r < rows; r++) {
                    free(arr[r]);
                }
                free(arr);
                for (long l = 0; l < len; l++) {
                    free(unique[l]);
                }
                free(unique);
                for (long k = 0; k < size; k++) {
                    free(tmp[k]);
                }
                free(tmp);
            }
            arr[row] = arr_row_temp;
            copy(arr[row], tmp, sizeof(char *), size);
            free(tmp);
        }
        col += len - 1;
        *cols = size;
        free(unique);
    }
    return arr;
}

double ***test_train_split(
    double **mat, 
    long rows, 
    long cols, 
    int *columns, 
    int columns_size, 
    double ratio
) {
    long train_size = (long)(rows * (1 - ratio));
    long test_size = rows - train_size;
    double **x_train = malloc(train_size * sizeof(double *));
    if (is_null(x_train, "x_train", "test_train_split")) { return NULL; };
    double **y_train = malloc(train_size * sizeof(double *));
    if (is_null(y_train, "y_train", "test_train_split")) { 
        free(x_train);
        return NULL; 
    };
    double **x_test = malloc(test_size * sizeof(double *));
    if (is_null(x_test, "x_test", "test_train_split")) { 
        free(x_train);
        free(y_train);
        return NULL; 
    };
    double **y_test = malloc(test_size * sizeof(double *));
    if (is_null(y_test, "y_test", "test_train_split")) { 
        free(x_train);
        free(y_train);
        free(x_test);
        return NULL; 
    };
    for (long i = 0; i < test_size; i++) {
        x_test[i] = malloc((cols - columns_size) * sizeof(double));
        if (is_null(x_test[i], "x_test[i]", "test_train_split")) {
            for (int j = 0; j < i; j++) {
                free(x_test[j]);
                free(y_test[j]);
            }
            free(x_train);
            free(y_train);
            free(x_test);
            free(y_test);
            return NULL;
        }
        y_test[i] = malloc(columns_size * sizeof(double));
        if (is_null(y_test[i], "y_test[i]", "test_train_split")) {
            for (int j = 0; j < i; j++) {
                free(x_test[j]);
                free(y_test[j]);
            }
            free(x_train);
            free(y_train);
            free(x_test);
            free(y_test);
            return NULL;
        }
    }
    for (long i = 0; i < train_size; i++) {
        x_train[i] = malloc((cols - columns_size) * sizeof(double));
        if (is_null(x_train[i], "x_train[i]", "test_train_split")) {
            for (int j = 0; j < test_size; j++) {
                free(x_test[j]);
                free(y_test[j]);
            }
            for (int j = 0; j < i; j++) {
                free(x_train[j]);
                free(y_train[j]);
            }
            free(x_train);
            free(y_train);
            free(x_test);
            free(y_test);
            return NULL;
        }
        y_train[i] = malloc(columns_size * sizeof(double));
        if (is_null(y_train[i], "y_train[i]", "test_train_split")) {
            for (int j = 0; j < test_size; j++) {
                free(x_test[j]);
                free(y_test[j]);
            }
            for (int j = 0; j < i; j++) {
                free(x_train[j]);
                free(y_train[j]);
            }
            free(x_train);
            free(y_train);
            free(x_test);
            free(y_test);
            return NULL;
        }
    }
    for (long row = 0; row < rows; row++) {
        long c1 = 0;
        long c2 = 0;
        for (int col = 0; col < cols; col++) {
            int is_in_columns = 0;
            for (int j = 0; j < columns_size; j++) {
                if (columns[j] == col) {
                    is_in_columns = 1;
                    break;
                }
            }
            if (row < train_size) {
                if (is_in_columns) {
                    y_train[row][c1] = mat[row][col];
                    c1 += 1;
                } else {
                    x_train[row][col - c1] = mat[row][col];
                }
            }
            else {
                if (is_in_columns){
                    y_test[row - train_size][c2] = mat[row][col];
                    c2 += 1;
                } else {
                    x_test[row - train_size][col - c2] = mat[row][col];
                }
            }
        }
    }
    double ***splitted = malloc(4 * sizeof(double **));
    if (is_null(splitted, "splitted", "test_train_split")) {
        for (int j = 0; j < test_size; j++) {
            free(x_test[j]);
            free(y_test[j]);
        }
        for (int j = 0; j < train_size; j++) {
            free(x_train[j]);
            free(y_train[j]);
        }
        free(x_train);
        free(y_train);
        free(x_test);
        free(y_test);
        return NULL;
    }
    splitted[0] = x_train;
    splitted[1] = y_train;
    splitted[2] = x_test;
    splitted[3] = y_test;
    return splitted;
}

char ***read_csv(
    char *filename, 
    long *rows, 
    long *cols, 
    char *delimiter
) {
    FILE *file;
    char buffer[256];
    file = fopen(filename, "r");
    char ***csv = malloc(sizeof(char **));
    if (is_null(csv, "csv", "read_csv")) { return NULL; }
    long index = 0;
    while (fgets(buffer, sizeof(buffer), file)) {
        if (!index) {
            index = 1;
            continue;
        }
        *cols = 0;
        char ***temp = realloc(csv, (*rows + 1) * sizeof(char **));
        if (is_null(temp, "temp", "read_csv")) {
            for (long i = 0; i < *rows; i++) {
                for (long j = 0; j < *cols; j++) {
                    free(csv[i][j]);
                }
                free(csv[i]);
            }
            free(csv);
            return NULL;
        }
        csv = temp;
        for (long i = 0; buffer[i] != '\0'; i++) {
            if (buffer[i] == ',') {
                (*cols)++;
            }
        }
        *cols += 1;
        csv[*rows] = malloc(*cols * sizeof(char *));
        if (is_null(csv[*rows], "csv[*rows]", "read_csv")) {
            for (long i = 0; i < *rows; i++) {
                for (long j = 0; j < *cols; j++) {
                    free(csv[i][j]);
                }
                free(csv[i]);
            }
            free(csv);
            return NULL;
        }
        char **splitted = split(buffer, delimiter);
        csv[*rows] = splitted;
        (*rows)++;
    }
    fclose(file);
    return csv;
}

int main() {
    long rows = 0, cols = 0;
    char ***csv = read_csv("sample.csv", &rows, &cols, ",");

    char ***encoded = label_encoder(rows, &cols, csv, (int[]){0, 1}, 2);
    
    double **mat = to_matrix(encoded, rows, cols);

    int columns[] = {2};
    int columns_size = 1;
    double ratio = .01;
    
    double ***splitted = test_train_split(
        mat, 
        rows, 
        cols, 
        columns, 
        columns_size, 
        ratio
    );
    
    double **x_train = splitted[0];
    double **y_train = splitted[1];
    double **x_test = splitted[2];
    double **y_test = splitted[3];
    
    long train_size = rows * (1 - ratio);
    long test_size = rows - train_size;
    
    int bias_term = 1;
    
    double **beta = linear_regression(
        x_train, 
        y_train,
        train_size, 
        (long[]){cols - columns_size, columns_size},
        bias_term
    );
    
    if (beta) {
        double **y_pred = predict(
            x_test, 
            beta, 
            test_size, 
            (long[]){cols - columns_size, columns_size}, 
            bias_term
        );
        for (int i = 0; i < test_size; i++) {
            printf("%f %f\n", y_pred[i][0], y_test[i][0]);
        }
    }
    return 0;
}
