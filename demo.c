
//Basic examples to show how to embed and extend Python in C including: creation of module in C with functions handling and building Python objects; importing and calling python functions from C.
//test.c
/* Example of embedding Python in another program */
// to compile run:
//  gcc -o test $(python-config --cflags) test.c $(python-config --ldflags) && ./test

#include<stdio.h>
#include "Python.h"

void initxyzzy(void); /* Forward */

main(int argc, char **argv)
{
    /* Pass argv[0] to the Python interpreter */
    Py_SetProgramName(argv[0]);

    /* Initialize the Python interpreter.  Required. */
    Py_Initialize();

    /* Add a static module */
    initxyzzy();

    /* Define sys.argv.  It is up to the application if you
       want this; you can also leave it undefined (since the Python
       code is generally not a main program it has no business
       touching sys.argv...) 
       If the third argument is true, sys.path is modified to include
       either the directory containing the script named by argv[0], or
       the current working directory.  This can be risky; if you run
       an application embedding Python in a directory controlled by
       someone else, attackers could put a Trojan-horse module in the
       directory (say, a file named os.py) that your application would
       then import and run.
    */
    PySys_SetArgvEx(argc, argv, 0);

    // Define an interger object:
    PyObject* pyIntObject = NULL;
    pyIntObject = PyInt_FromLong(5);

    /* Do some application specific code */
    printf("Hello, brave new world\n\n");

    /* Execute some Python statements (in module __main__) */
    PyRun_SimpleString("import sys\n");
    PyRun_SimpleString("print sys.builtin_module_names\n");
    PyRun_SimpleString("print sys.modules.keys()\n");
    PyRun_SimpleString("print sys.executable\n");
    
    PyRun_SimpleString("print sys.argv\n");


    PyRun_SimpleString("import xyzzy\n");
    PyRun_SimpleString("print 'xyzzy\\n', dir(xyzzy)\n");
    PyRun_SimpleString("print xyzzy.foo()\n");

    PyRun_SimpleString("print xyzzy.bar('hello',42)\n");

    PyRun_SimpleString("print xyzzy.baz()\n");

    PyRun_SimpleString("print xyzzy.qux()\n");

    PyRun_SimpleString("print xyzzy.list()\n");

    PyRun_SimpleString("print xyzzy.args([1,42,'hello',[33,'world']])\n");


    /* Note that you can call any public function of the Python
       interpreter here, e.g. call_object(). */

    if (pyIntObject == NULL) {}; // Error
    

    call_time_time();

    call_module_func();

    /* Some more application specific code */
    printf("\nGoodbye, cruel world\n");

    Py_DECREF(pyIntObject);

    Py_Finalize();

    /* Exit, cleaning up the interpreter */
    Py_Exit(0);
    /*NOTREACHED*/
}

/* A static module */

/* 'self' is not used */
static PyObject* xyzzy_foo(PyObject *self, PyObject* args)
{
    printf("# Called xyzzy_foo\n");
    return PyInt_FromLong(42L);
}

static PyObject* xyzzy_bar(PyObject* pSelf, PyObject* pArgs)
{
    // Building and Parsing Arguments: 
    printf("# Called xyzzy_bar\n");

    char* str = NULL;
    int num = 0;
    char output[100];
    
    if(!PyArg_ParseTuple(pArgs, "si", &str, &num)) {
        PyErr_SetString(PyExc_StandardError, "Your own error message!");
        // returning NULL is that Python interprets this as an indication that an exception was raised.
        return NULL;
    }
    sprintf(output, "str: %s; num: %i\n", str, num);
    // printing from C
    printf(output);

    // returning the output to Python
    return Py_BuildValue("s", output);
}

static PyObject* xyzzy_baz(PyObject* pSelf, PyObject* pArgs)
{
    printf("# Called xyzzy_baz\n");

    // returning None
    Py_INCREF(Py_None);
    return Py_None;
}

// Tuples:
static PyObject* xyzzy_qux(PyObject* pSelf, PyObject* pArgs)
{
    printf("# Called xyzzy_qux (Tuples)\n");

    // We create a Tuple with two objects in it.
    PyObject* MyTuple = PyTuple_New(2);
    if (MyTuple == NULL) { // Error
        PyErr_SetString(PyExc_StandardError, "Couldn't create a tuple!");
        return NULL;
    }; 

    PyObject* MyInt = PyInt_FromLong(100);
    if (MyInt == NULL) {
        // Don't forget to clean up
        Py_DECREF(MyTuple);
        // Error
        PyErr_SetString(PyExc_StandardError, "Couldn't store an integer in the tuple...");
        return NULL;
    }
    
    PyTuple_SetItem(MyTuple, 0, MyInt);

    PyObject* MyStr = PyString_FromString("Arkon");
    if (MyStr == NULL) {
        // Don't forget to clean up
        Py_DECREF(MyTuple);
        // We don't need to free the int object, because the Tuple will take care of it.
        // Error
        PyErr_SetString(PyExc_StandardError, "Couldn't store a string in the tuple...");
        return NULL;
    }
    PyTuple_SetItem(MyTuple, 1, MyStr);

    return MyTuple;
}

// Lists:
static PyObject* xyzzy_list(PyObject* pSelf, PyObject* pArgs)
{
    int i;
    
    printf("# Called xyzzy_list ... \n");
    PyObject* MyList = PyList_New(2);
    PyList_SetItem(MyList, 0, PyInt_FromLong(2014));
    PyList_SetItem(MyList, 1, PyString_FromString("Hello World"));

    for (i=1; i < 10; i++) {
        PyList_Append(MyList, PyInt_FromLong((long)i));
    }

    PyList_Insert(MyList, 0, PyString_FromString("An inserted string"));

    return MyList;
}

// 
static PyObject* xyzzy_args(PyObject* pSelf, PyObject* pArgs)
{
    int i;
    PyObject* ArgsList = NULL;

    printf("# Called xyzzy_args\n");
    
    if (!PyArg_ParseTuple(pArgs, "O", &ArgsList)) return NULL; // We want to get one list object.

    // Making sure its type is a list.
    if (!PyList_Check(ArgsList)) {
        // Raise an exception in case it's not.
        PyErr_SetString(PyExc_StandardError, "The only parameter \"xyzzy.args\" gets, should be a list!");
        return NULL;
    }

    // Scan all elements on that list, make sure they are int objects and print them:
    for (i = 0; i < PyList_Size(ArgsList); i++) {
        // This is a BORROWED REFERENCE!!! Don't DECREF it later!
       
        PyObject* tmpObj = PyList_GetItem(ArgsList, i);
        PyTypeObject* tmpType = PyObject_Type(tmpObj);

        PyObject* objectsRepresentation = PyObject_Repr(tmpObj);

        // Now that we know that we got an integer, let's print it.
        printf("%s: %s\n", PyString_AsString(PyObject_Str(tmpType)), PyString_AsString(objectsRepresentation));
    }

    // We are finished using the ArgsList, but we mustn't touch it, 
    // because we "told" PyArg_ParseTuple that we want to BORROW it!
    // So that's all folks. . .

    Py_INCREF(Py_None);
    return Py_None;
}


// Calling Python Functions from C
void call_time_time() {
    PyObject *pModule, *pFunc, *pValue;

    pModule = PyImport_ImportModule("time");
    pFunc = PyObject_GetAttrString(pModule, "time");
    pValue = PyObject_CallObject(pFunc, NULL);
    printf("*** time.time: %10.3f\n", PyFloat_AsDouble(pValue));
}

// Calling Python Functions from C (of a user module)
void call_module_func() {

    PyObject *module, *func, *args, *value;

    PySys_SetPath(".");
    module = PyImport_ImportModule("test");
    func = PyObject_GetAttrString(module, "test");

    args = PyTuple_New(1);
    PyTuple_SetItem(args, 0, PyInt_FromLong(42L));
    value = PyObject_CallObject(func, args);
    // OR:
    //value = PyObject_CallFunction(func, "i", 42);
    
    printf("*** test.test: %i\n", PyInt_AsLong(value));
}


static PyMethodDef xyzzy_methods[] = {
    {"foo",  xyzzy_foo,   METH_NOARGS,  "Return the meaning of everything."},
    {"bar",  xyzzy_bar,   METH_VARARGS, "An advanced dummy function which prints its argument."},
    {"baz",  xyzzy_baz,   METH_NOARGS,  "Return None."},
    {"qux",  xyzzy_qux,   METH_NOARGS,  "Return a tuple."},
    {"list", xyzzy_list,  METH_NOARGS,  "Return a list."},
    {"args", xyzzy_args,  METH_VARARGS, "Parses args."},
    {NULL, NULL,      0,            NULL}           /* sentinel */
};

void
initxyzzy(void)
{
    PyImport_AddModule("xyzzy");
    Py_InitModule("xyzzy", xyzzy_methods);
}
//test.py
//# test.test
//def test(value):
//    print '### test called'
//    return value * 42
//
