# define PY_SSIZE_T_CLEAN
# include <Python.h>
# include "symnmf.h"

double **extract_2d_array(PyObject *PyX) {
    double **X;
    int n, d, i, j;
    PyObject *row, *element;

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
    int len;

    if (!PyArg_ParseTuple(args, "O", &PyX)) {
        return NULL;
    }

    return extract_2d_array(PyX);
}

PyObject *create_2d_list(double** arr) {
    PyObject *row;
    int  i, j;
    int n = sizeof(arr) / sizeof(arr[0]);
    int dim = sizeof(arr[0]) / sizeof(arr[0][0]);
    PyObject *PyOut = PyList_New(n);

    for (i = 0; i < n; i++) {
        row = PyList_New(n);

        for (j = 0; j < dim; j++) {
            PyObject *elem = Py_BuildValue("d", arr[i][j]);
            PyList_SetItem(row, j, elem);
        }
        PyList_SetItem(PyOut, i, row);
    }
}

void free_2d_array(double **arr) {
    int i;
    int n = sizeof(arr) / sizeof(arr[0]);
    for (i = 0; i < n; i++) {
        free(arr[i]);
    }
    free(arr);
}

static PyObject *sym_py(PyObject *self, PyObject *args) {
    double **X = get_arg(args);
    double **A = sym(X);

    PyObject *PyOut = create_2d_list(A);

    free_2d_array(X);
    free_2d_array(A);

    return PyOut;
}

static PyObject *ddg_py(PyObject *self, PyObject *args) {
    double **X = get_arg(args);
    double **D = ddg(X);

    PyObject *PyOut = create_2d_list(D);

    free_2d_array(X);
    free_2d_array(D);

    return PyOut;
}

static PyObject *norm_py(PyObject *self, PyObject *args) {
    double **X = get_arg(args);
    double **W = norm(X);

    PyObject *PyOut = create_2d_list(W);

    free_2d_array(X);
    free_2d_array(W);

    return PyOut;
}

static PyObject *symnmf_py(PyObject *self, PyObject *args) {
    PyObject *PyX, *PyW, *PyH;
    double **W, **H;

    if (!PyArg_ParseTuple(args, "OO", &PyW, &PyH)) {
        return NULL;
    }
    W = extract_2d_array(PyW);
    H = extract_2d_array(PyH);

    H = symnmf(H, W);

    PyObject *PyOut = create_2d_list(H);

    free_2d_array(W);
    free_2d_array(H);

    return PyOut;
}

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

static struct PyModuleDef symnmfmodule = {
    PyModuleDef_HEAD_INIT,
    "symnmf",
    NULL,
    -1,
    symnmfMethods
};
