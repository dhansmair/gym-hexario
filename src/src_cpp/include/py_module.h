#ifndef HEXAR_PYMODULE_H
#define HEXAR_PYMODULE_H

#ifdef __cplusplus
extern "C" {
#endif

void destroy_interface(PyObject *interface);

static PyObject* setup(PyObject *self, PyObject *args);

static PyObject* reset_player(PyObject *self, PyObject *args);

static PyObject* take_actions(PyObject *self, PyObject *args);

static PyObject* get_observation(PyObject *self, PyObject *args);

static PyObject* is_dead(PyObject *self, PyObject *args);

static PyObject* get_num_kills(PyObject *self, PyObject *args);

static PyObject* get_num_captures(PyObject *self, PyObject *args);

static PyObject* get_num_claims(PyObject *self, PyObject *args);

static PyObject* show(PyObject *self, PyObject *args);

#ifdef __cplusplus
}
#endif

#endif