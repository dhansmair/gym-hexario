#define PY_SSIZE_T_CLEAN
#define NPY_NO_DEPRECATED_API NPY_1_7_API_VERSION

#include <Python.h>
#include <numpy/arrayobject.h> /* numpy */
#include <numpy/ndarrayobject.h>
#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <ctime>
#include "include/py_module.h"
#include "include/controller.h"

/**
 * https://www.tutorialspoint.com/python/python_further_extensions.htm
 */

using namespace std;


const char* identifier = "val";

#ifdef __cplusplus
extern "C" {
#endif

/**
 * calls the destructor
 */
void destroy_controller(PyObject *controller)
{
    delete (Controller *) PyCapsule_GetPointer(controller, identifier);
}

/**
 * this is like a constructor
 */
static PyObject* setup(PyObject *self, PyObject *args)
{
    int mapRadius;
    int numPlayers;
    int localPerspective;
    int featureObservations;
    int discreteActionSpace;
    int observationRadius;
    int pxObservationWidth;
    int pxObservationHeight;
    float defaultVelocity;

    // parse the arguments
    if (!PyArg_ParseTuple(args, "iiiiiiiif", 
        &mapRadius, 
        &numPlayers, 
        &localPerspective,
        &featureObservations, 
        &discreteActionSpace, 
        &observationRadius, 
        &pxObservationWidth,
        &pxObservationHeight,
        &defaultVelocity)) {
        return NULL;
    }

    // initialize random number generator
    srand(time(NULL));
    
    // yes, this allocates space on the heap
    Controller *controller = new Controller(
        mapRadius,
        numPlayers,
        localPerspective == true,
        featureObservations == true, 
        discreteActionSpace == true,
        observationRadius,
        pxObservationWidth,
        pxObservationHeight,
        defaultVelocity
    );

    // return something
    return PyCapsule_New((void *) controller, identifier, destroy_controller);
}

/**
 * 
 */
static PyObject* reset_player(PyObject *self, PyObject *args)
{
    PyObject *ptr = NULL;
    int playerId;

    if (!PyArg_ParseTuple(args, "Oi", &ptr, &playerId)) return NULL;

    Controller *controller = (Controller *) PyCapsule_GetPointer(ptr, identifier);
    if (controller == NULL) {
        cout << "controller is Null, this must not happen!" << endl;
        return NULL;
    }

    controller->resetPlayer(playerId);

    // return something
    Py_RETURN_NONE;
}


/**
* apply a set of actions, one for each player.
* num of actions must match num of players.
*/
static PyObject* take_actions(PyObject *self, PyObject *args)
{
    PyObject *ptr = NULL;
    PyArrayObject *arr = NULL;

    if (!PyArg_ParseTuple(args, "OO", &ptr, &arr)) return NULL;

    // retrieve the controller from ptr object
    Controller *controller = (Controller *) PyCapsule_GetPointer(ptr, identifier);
    if (controller == NULL) {
        cout << "controller is Null, this must not happen!" << endl;
        return NULL;
    }

    // get the array
    std::vector<float> actions;

    for (int i = 0; i < PyArray_SIZE(arr); i++) {
        float *action = (float *) PyArray_GETPTR1(arr, i);
        actions.push_back(*action);
    }

    controller->takeActions(actions);
    
    // return something
    Py_RETURN_NONE;
}



/**
* just a test for returning a numpy array 
*/
static PyObject* get_observation(PyObject *self, PyObject *args)
{
    PyObject *ptr = NULL;
    int playerId;
   
    if (!PyArg_ParseTuple(args, "Oi", &ptr, &playerId)) return NULL;

    // retrieve the controller from ptr object
    Controller *controller = (Controller *) PyCapsule_GetPointer(ptr, identifier);
    if (controller == NULL) {
        cout << "controller is Null, this must not happen!" << endl;
        return NULL;
    }

    // construct numpy array
    long observationSize = controller->getObservationSize();
    int n_dims = 1;
    npy_intp mdims[] = {observationSize};
    PyArrayObject *observation;
    observation = (PyArrayObject *) PyArray_SimpleNew(n_dims, mdims, NPY_FLOAT32);
    controller->getObservation(playerId, (float *) PyArray_DATA(observation), observationSize);
    
    return PyArray_Return(observation);
}

/**
 * 
 */
static PyObject* is_dead(PyObject *self, PyObject *args)
{
    // parse arguments
    PyObject *ptr = NULL;
    int playerId;
   
    if (!PyArg_ParseTuple(args, "Oi", &ptr, &playerId)) return NULL;

    // retrieve the controller from ptr object
    Controller *controller = (Controller *) PyCapsule_GetPointer(ptr, identifier);
    if (controller == NULL) {
        cout << "controller is Null, this must not happen!" << endl;
        return NULL;
    }

    if (controller->isDead(playerId)) {
        Py_RETURN_TRUE;
    } else {
        Py_RETURN_FALSE;
    }
}


/**
 * 
 */
static PyObject* is_winner(PyObject *self, PyObject *args)
{
    // parse arguments
    PyObject *ptr = NULL;
    int playerId;
   
    if (!PyArg_ParseTuple(args, "Oi", &ptr, &playerId)) return NULL;

    // retrieve the controller from ptr object
    Controller *controller = (Controller *) PyCapsule_GetPointer(ptr, identifier);
    if (controller == NULL) {
        cout << "controller is Null, this must not happen!" << endl;
        return NULL;
    }

    if (controller->isWinner(playerId)) {
        Py_RETURN_TRUE;
    } else {
        Py_RETURN_FALSE;
    }
}

/**
 * 
 */
static PyObject* get_num_kills(PyObject *self, PyObject *args)
{
    // parse arguments
    PyObject *ptr = NULL;
    int playerId;
   
    if (!PyArg_ParseTuple(args, "Oi", &ptr, &playerId)) return NULL;

    // retrieve the controller from ptr object
    Controller *controller = (Controller *) PyCapsule_GetPointer(ptr, identifier);
    if (controller == NULL) {
        cout << "controller is Null, this must not happen!" << endl;
        return NULL;
    }

    int numKills = controller->getNumKills(playerId);

    return PyLong_FromLong((long) numKills);
}

static PyObject* get_num_captures(PyObject *self, PyObject *args)
{
    // parse arguments
    PyObject *ptr = NULL;
    int playerId;
   
    if (!PyArg_ParseTuple(args, "Oi", &ptr, &playerId)) return NULL;

    // retrieve the controller from ptr object
    Controller *controller = (Controller *) PyCapsule_GetPointer(ptr, identifier);
    if (controller == NULL) {
        cout << "controller is Null, this must not happen!" << endl;
        return NULL;
    }

    int numCaptures = controller->getNumCaptures(playerId);

    return PyLong_FromLong((long) numCaptures);
}

static PyObject* get_num_claims(PyObject *self, PyObject *args)
{
    // parse arguments
    PyObject *ptr = NULL;
    int playerId;
   
    if (!PyArg_ParseTuple(args, "Oi", &ptr, &playerId)) return NULL;

    // retrieve the controller from ptr object
    Controller *controller = (Controller *) PyCapsule_GetPointer(ptr, identifier);
    if (controller == NULL) {
        cout << "controller is Null, this must not happen!" << endl;
        return NULL;
    }

    int numCaptures = controller->getNumClaims(playerId);

    return PyLong_FromLong((long) numCaptures);
}

static PyObject* get_distances(PyObject *self, PyObject *args)
{
    // parse arguments
    PyObject *ptr = NULL;
    int playerId;

    if (!PyArg_ParseTuple(args, "Oi", &ptr, &playerId)) return NULL;

    // retrieve the controller from ptr object
    Controller *controller = (Controller *) PyCapsule_GetPointer(ptr, identifier);
    if (controller == NULL) {
        cout << "controller is Null, this must not happen!" << endl;
        return NULL;
    }

    std::vector<float> dists = controller->getDistancesToBorder(playerId);

    return Py_BuildValue("dddddd", dists[0], dists[1], dists[2], dists[3], dists[4], dists[5]);
}



/**
 * returns true if a quit event was fired from the demo window
 */
static PyObject* show(PyObject *self, PyObject *args) 
{
    // parse arguments
    PyObject *ptr = NULL;
    if (!PyArg_ParseTuple(args, "O", &ptr)) return NULL;

    // retrieve the controller from ptr object
    Controller *controller = (Controller *) PyCapsule_GetPointer(ptr, identifier);
    if (controller == NULL) {
        cout << "controller is Null, this must not happen!" << endl;
        return NULL;
    }

    bool quit = controller->show();

    if (quit) {
        Py_RETURN_TRUE;
    } else {
        Py_RETURN_FALSE;
    }
}

#ifdef __cplusplus
}
#endif


static PyMethodDef game_module_methods[] = { 
    {
        "setup", setup, METH_VARARGS,
        "calls constructor"
    },
    {
        "reset_player", reset_player, METH_VARARGS,
        "testing the reset player implementation"
    },
    {
        "take_actions", take_actions, METH_VARARGS,
        "taking actions for all players"
    },
    {
        "get_observation", get_observation, METH_VARARGS,
        "getting observation for a specific player"
    },
    {
        "is_dead", is_dead, METH_VARARGS,
        "check if one specific player is dead"
    },
    {
        "is_winner", is_winner, METH_VARARGS,
        "check if one specific player has captured the whole map"
    },
    {
        "get_distances", get_distances, METH_VARARGS,
        "get the distance from the border in each of the six directions"
    },
    {
        "get_num_kills", get_num_kills, METH_VARARGS,
        "get the number of kills a player made since he got spawned"
    },
    {
        "get_num_captures", get_num_captures, METH_VARARGS,
        "get the number of captures a player made since he got spawned"
    },
    {
        "get_num_claims", get_num_claims, METH_VARARGS,
        "get the number of claims a player currently has"
    },
    {
        "show", show, METH_VARARGS,
        "display current game on the screen"    
    },
    {NULL, NULL, 0, NULL}
};

// Module definition
// The arguments of this structure tell Python what to call your extension,
// what it's methods are and where to look for it's method definitions
static struct PyModuleDef game_module_definition = { 
    PyModuleDef_HEAD_INIT,
    "game_module",
    "A Python module that prints 'hello world' from C code.",
    -1, 
    game_module_methods
};

// Module initialization
// Python calls this function when importing your extension. It is important
// that this function is named PyInit_[[your_module_name]] exactly, and matches
// the name keyword argument in setup.py's setup() call.
PyMODINIT_FUNC PyInit_game_module(void) {
    Py_Initialize();
    import_array();
    return PyModule_Create(&game_module_definition);
}
