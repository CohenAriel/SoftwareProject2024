# define PY_SSIZE_T_CLEAN
# include <Python.h>
# include "symnmf.h"
# include <stdio.h>
# include <stdlib.h>

/*Extracts a 2D array from a Python object. Also sets n and m to dimensions*/
double **extract_2d_array(PyObject *PyX, int *n, int *m) {
    double **X;
    int i, j;
    PyObject *row, *element;

    *n = PyObject_Length(PyX);
    *m = PyObject_Length(PyList_GetItem(PyX, 0));

    /*Allocate memory for the 2D array*/
    X = (double **) malloc(*n * sizeof(double *));
    for (i = 0; i < *n; i++) {
        X[i] = (double *) malloc(*m * sizeof(double));
        row = PyList_GetItem(PyX, i);
        for (j = 0; j < *m; j++){
            element = PyList_GetItem(row, j);
            /*Convert Python object to double*/
            X[i][j] = PyFloat_AsDouble(element);
        }
    }
    return X;
}

/*Gets arguments and extracts 2D array. Also sets n and m to dimensions*/
double **get_arg(PyObject *args, int *n, int *m) {
    PyObject *PyX;

    if (!PyArg_ParseTuple(args, "O", &PyX)) {
        return NULL;
    }

    return extract_2d_array(PyX, n, m);
}

/*Creates a Python 2D list from a 2D array*/
PyObject *create_2d_list(double** arr, int n, int m) {
    PyObject *row;
    int  i, j;
    PyObject *PyOut = PyList_New(n);

    for (i = 0; i < n; i++) {
        row = PyList_New(n);

        for (j = 0; j < m; j++) {
            /*Convert double to Python object*/
            PyObject *elem = Py_BuildValue("d", arr[i][j]);
            PyList_SetItem(row, j, elem);
        }
        PyList_SetItem(PyOut, i, row);
    }
    return PyOut;
}

/* Python binding for sym function */
static PyObject *sym_py(PyObject *self, PyObject *args) {
    /* Extract input arguments */
    int n, d;
    double **X = get_arg(args, &n, &d);
    /*Calculate sym matrix*/
    double **A = sym(X, n, d);
    /*Convert result to Python object*/
    PyObject *PyOut = create_2d_list(A, n, n);
    /*Free memory*/
    free_2d_array(X, n);
    free_2d_array(A, n);

    return PyOut;
}

/*Python binding for ddg function*/
static PyObject *ddg_py(PyObject *self, PyObject *args) {
    /*Extract input arguments*/
    int n, d;
    double **X = get_arg(args, &n, &d);
    /*Calculate ddg matrix*/
    double **A = sym(X, n, d);
    double **D = ddg(A, n);
    /*Convert result to Python object*/
    PyObject *PyOut = create_2d_list(D, n, n);
    /*Free memory*/
    free_2d_array(X, n);
    free_2d_array(A, n);
    free_2d_array(D, n);

    return PyOut;
}

/*Python binding for norm function*/
static PyObject *norm_py(PyObject *self, PyObject *args) {
    /*Extract input arguments*/
    int n, d;
    double **X = get_arg(args, &n, &d);
    /*Calculate norm matrix*/
    double **A = sym(X, n, d);
    double **W = norm(A, n);
    /*Convert result to Python object*/
    PyObject *PyOut = create_2d_list(W, n, n);
    /*Free memory*/
    free_2d_array(X, n);
    free_2d_array(A, n);
    free_2d_array(W, n);

    return PyOut;
}

/* Python binding for symnmf function */
static PyObject *symnmf_py(PyObject *self, PyObject *args) {
    PyObject *PyW, *PyH;
    double **W, **H;

    /* Extract input arguments */
    if (!PyArg_ParseTuple(args, "OO", &PyW, &PyH)) {
        return NULL;
    }
    int n, d, k;
    W = extract_2d_array(PyW, &n, &d);
    H = extract_2d_array(PyH, &n, &k);

    /* Calculate symnmf */
    H = symnmf(H, W, n, k);

    /* Convert result to Python object */
    PyObject *PyOut = create_2d_list(H, n, d);

    /* Free memory */
    free_2d_array(W, n);
    free_2d_array(H, n);

    return PyOut;
}

/* Method definitions for the Python module */
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

/* Module definition */
static struct PyModuleDef symnmfmodule = {
    PyModuleDef_HEAD_INIT,
    "symnmfmodule",
    NULL,
    -1,
    symnmfMethods
};

PyMODINIT_FUNC PyInit_symnmfmodule(void)
{
    PyObject *m;
    m = PyModule_Create(&symnmfmodule);
    if (!m) {
        return NULL;
    }
    return m;
}
