# DataAnalysis

A Python wrapper for a simple linear regression library written in C.

```
from matplotlib import pyplot as plt
from DataAnalysis import DataAnalysis

data_analysis = DataAnalysis()

csv = data_analysis.read_csv("sample.csv", ",")

# Encode the first and the second columns of the sample dataset.
encoded = data_analysis.label_encoder(csv, [0, 1])
matrix = data_analysis.to_matrix(encoded)

# There are totally 4944 rows in the dataset.
# Every 24 rows correspond to one day. A row corresponds to a hour.
# If we want to predict the 2'nd columns of the last day,
# we should give a proportion like 24/4944 for the test data 
# and 1 - (24 / 4944) for the train data.
splitted = data_analysis.test_train_split(matrix, [2], 24 / 4944)

x_train, y_train, x_test, y_test = splitted[:]

bias_term = 1
beta = data_analysis.linear_regression(x_train, y_train, bias_term)
y_pred = data_analysis.predict(x_test, y_test, beta, bias_term)

y_test = [
    [y_test.data[i][j] for j in range(y_test.ncols)] 
    for i in range(y_test.nrows)
]
y_pred = [
    [y_pred.data[i][j] for j in range(y_pred.ncols)] 
    for i in range(y_pred.nrows)
]

x = range(len(y_test))
plt.plot(x, y_test, color="red", label="y_test")
plt.plot(x, y_pred, color="blue", label="y_pred")
plt.xticks(x) 
plt.legend()
plt.title("sample.csv")
plt.show()
```

![Figure_1](https://github.com/user-attachments/assets/895c8d71-90c4-428f-9ee8-45d0d44e858b)
