# define PY_SSIZE_T_CLEAN
# include <Python.h>
# include "symnmf.h"

// Extracts a 2D array from a Python object
double **extract_2d_array(PyObject *PyX) {
    double **X;
    int n, d, i, j;
    PyObject *row, *element;

    n = PyObject_Length(PyX);
    d = PyObject_Length(PyList_GetItem(PyX, 0));

    // Allocate memory for the 2D array
    X = (double **) malloc(n * sizeof(double *));
    for (i = 0; i < n; i++) {
        X[i] = (double *) malloc(d * sizeof(double));
        row = PyList_GetItem(PyX, i);
        for (j = 0; j < d; j++){
            element = PyList_GetItem(row, j);
            // Convert Python object to double
            X[i][j] = PyFloat_AsDouble(element);
        }
    }
    return X;
}

// Gets arguments and extracts 2D array
double **get_arg(PyObject *args) {
    PyObject *PyX;

    if (!PyArg_ParseTuple(args, "O", &PyX)) {
        return NULL;
    }

    return extract_2d_array(PyX);
}

// Creates a Python 2D list from a 2D array
PyObject *create_2d_list(double** arr) {
    PyObject *row;
    int  i, j;
    int n = sizeof(arr) / sizeof(arr[0]);
    int dim = sizeof(arr[0]) / sizeof(arr[0][0]);
    PyObject *PyOut = PyList_New(n);

    for (i = 0; i < n; i++) {
        row = PyList_New(n);

        for (j = 0; j < dim; j++) {
            // Convert double to Python object
            PyObject *elem = Py_BuildValue("d", arr[i][j]);
            PyList_SetItem(row, j, elem);
        }
        PyList_SetItem(PyOut, i, row);
    }
    return PyOut;
}

// Python binding for sym function
static PyObject *sym_py(PyObject *self, PyObject *args) {
    // Extract input arguments
    double **X = get_arg(args);
    // Calculate sym matrix
    double **A = sym(X);
    // Convert result to Python object
    PyObject *PyOut = create_2d_list(A);
    // Free memory
    free_2d_array(X);
    free_2d_array(A);

    return PyOut;
}

// Python binding for ddg function
static PyObject *ddg_py(PyObject *self, PyObject *args) {
    // Extract input arguments
    double **X = get_arg(args);
    // Calculate ddg matrix
    double **D = ddg(X);
    // Convert result to Python object
    PyObject *PyOut = create_2d_list(D);
    // Free memory
    free_2d_array(X);
    free_2d_array(D);

    return PyOut;
}

// Python binding for norm function
static PyObject *norm_py(PyObject *self, PyObject *args) {
    // Extract input arguments
    double **X = get_arg(args);
    // Calculate norm matrix
    double **W = norm(X);
    // Convert result to Python object
    PyObject *PyOut = create_2d_list(W);
    // Free memory
    free_2d_array(X);
    free_2d_array(W);

    return PyOut;
}

// Python binding for symnmf function
static PyObject *symnmf_py(PyObject *self, PyObject *args) {
    PyObject *PyW, *PyH;
    double **W, **H;

    // Extract input arguments
    if (!PyArg_ParseTuple(args, "OO", &PyW, &PyH)) {
        return NULL;
    }
    W = extract_2d_array(PyW);
    H = extract_2d_array(PyH);

    // Calculate symnmf
    H = symnmf(H, W);

    // Convert result to Python object
    PyObject *PyOut = create_2d_list(H);

    // Free memory
    free_2d_array(W);
    free_2d_array(H);

    return PyOut;
}

// Method definitions for the Python module
static PyMethodDef symnmfMethods[] = {
    {"sym",
      (PyCFunction) sym_py,
      METH_VARARGS,
      PyDoc_STR("Calculate and output the similarity matrix as described in 1.1")
    },
    {"ddg",
      (PyCFunction) ddg_py,
      METH_VARARGS,
      PyDoc_STR("Calculate and output the Diagonal Degree Matrix as described in 1.2")
    },
    {"norm",
      (PyCFunction) norm_py,
      METH_VARARGS,
      PyDoc_STR("Calculate and output the normalized similarity matrix as described in 1.3")
    },
    {"symnmf",
      (PyCFunction) symnmf_py,
      METH_VARARGS,
      PyDoc_STR("Perform full the symNMF as described in 1 and output H")
    },
    {NULL, NULL, 0, NULL}
};

// Module definition
static struct PyModuleDef symnmfmodule = {
    PyModuleDef_HEAD_INIT,
    "symnmf",
    NULL,
    -1,
    symnmfMethods
};