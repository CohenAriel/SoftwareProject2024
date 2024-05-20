# define PY_SSIZE_T_CLEAN
# include <Python.h>
# include "symnmf.h"

double **extract_2d_array(PyObject *PyX) {
    double **X;
    int n, d, i, j;

    n = PyObject_Length(PyX);
    d = PyObject_Length(PyList_GetItem(PyX, 0));

    X = (double **) malloc(n * sizeof(double *));
    for (i = 0; i < n; i++) {
        X[i] = (double *) malloc(d * sizeof(double));
        row = PyList_GetItem(PyX, i);
        for (j = 0; j < d; j++){
            element = PyList_GetItem(row, j);
            //Py_INCREF(element); I'm not sure if this is needed. Try uncommenting if there is a memory leak. Otherwise delete this line.
            X[i][j] = PyFloat_AsDouble(element);
        }
    }
    return X;
}

double **get_arg(PyObject *args) {
    PyObject *PyX;

    if (!PyArg_ParseTuple(args, "O", &PyX)) {
        return NULL;
    }

    return extract_2d_array(PyX);
}

PyObject *create_2d_list(double** arr) {
    int n = PyList_New(PyObject_Length(PyX));
    PyObject *PyOut = PyList_New(n);
    for (i = 0; i < k; i++) {
        row = PyList_New(n);

        for (j = 0; j < dim; j++) {
            PyObject *elem = Py_BuildValue("d", centroids[i][j]);
            PyList_SetItem(row, j, elem);
        }
        PyList_SetItem(PyOut, i, row);
    }
}

void free_2d_array(double **arr) {
    for (i = 0; i < n; i++) {
        free(arr[i]);
    }
    free(arr);
}

static PyObject *sym(PyObject *self, PyObject *args) {
    double **X = get_arg(args);
    double **A = symnmf(X);

    PyObject *PyOut = create_2d_list(A);

    free_2d_array(X);
    free_2d_array(A);

    return PyOut;
}

static PyObject *ddg(PyObject *self, PyObject *args) {
    double **X = get_arg(args);
    double **D = ddg(X);

    PyObject *PyOut = create_2d_list(D);

    free_2d_array(X);
    free_2d_array(D);

    return PyOut;
}

static PyObject *norm(PyObject *self, PyObject *args) {
    double **X = get_arg(args);
    double **W = norm(X);

    PyObject *PyOut = create_2d_list(W);

    free_2d_array(X);
    free_2d_array(W);

    return PyOut;
}

static PyObject *symnmf(PyObject *self, PyObject *args) {
    PyObject *PyX, *PyW, *PyH;
    double **X = get_arg(args);
    double **W = sym(X);
    double **H = symnmf(H, W);

    PyObject *PyOut = create_2d_list(H);

    free_2d_array(X);
    free_2d_array(W);
    free_2d_array(H);

    return PyOut;
}
