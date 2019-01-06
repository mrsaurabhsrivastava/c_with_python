#include<iostream>
#include<cstdio>
#include<Python.h>

using namespace std;

//void initxyzzy();

PyMODINIT_FUNC PyInit_xyzzy(void) ;

int main(int argc, char** argv) 
{
	wchar_t *lArgv[argc];
	for (int i = 0; i < argc; i++) 
	{
		lArgv[i]= Py_DecodeLocale(argv[i], NULL);
	}
    	if (lArgv[0] == NULL) {
        	fprintf(stderr, "Fatal error: cannot decode argv[0]\n");
        	exit(1);
    	}
	
 	Py_SetProgramName(lArgv[0]);
	PyImport_AppendInittab("xyzzy",PyInit_xyzzy);
	Py_Initialize();
	PySys_SetArgvEx(argc, lArgv, 0);

	//PyObject* pyIntObject = NULL;
	//pyIntObject = PyLong_FromLong(5);

	printf("Hello, brave new world\n\n");
	PyObject *obj = Py_BuildValue("s", "test.py");
 	FILE *file = _Py_fopen_obj(obj, "r+");
	if(file != NULL) {
     		PyRun_SimpleFile(file, "test.py");
 	}

	//PyRun_SimpleString("import sys");
	//PyRun_SimpleString("print (sys.argv)");
	//PyRun_SimpleString("import xyzzy\n");
	//PyRun_SimpleString("xyzzy.foo()\n");
	
}

static PyObject* xyzzy_foo(PyObject *self, PyObject* args)
{
    printf("# Called xyzzy_foo\n");
    return PyLong_FromLong(42L);
}

static PyObject* xyzzy_args(PyObject* pSelf, PyObject* pArgs)
{
	printf("# Called xyzzy_args\n");
	PyObject* ArgsList = NULL;
	char* data;
	if (!PyArg_ParseTuple(pArgs, "s", &data)) return NULL;
	
	cout<<data[0]<<endl;
	Py_INCREF(Py_None);
	return Py_None;		
}

static PyMethodDef xyzzy_methods[] = {
   {"foo",  xyzzy_foo,   METH_NOARGS,  "Return the meaning of everything."},
   {"args", xyzzy_args,  METH_VARARGS, "Parses args."},
   {NULL, NULL,      0,            NULL}           /* sentinel */
};

static struct PyModuleDef cModPyDem =
{
    PyModuleDef_HEAD_INIT,
    "xyzzy",
    "",
    -1,
    xyzzy_methods 		    
};

//
//static PyMethodDef xyzzy_methods[] = {
//    {"foo",  xyzzy_foo,   METH_NOARGS,  "Return the meaning of everything."},
//    {"bar",  xyzzy_bar,   METH_VARARGS, "An advanced dummy function which prints its argument."},
//    {"baz",  xyzzy_baz,   METH_NOARGS,  "Return None."},
//    {"qux",  xyzzy_qux,   METH_NOARGS,  "Return a tuple."},
//    {"list", xyzzy_list,  METH_NOARGS,  "Return a list."},
//    {"args", xyzzy_args,  METH_VARARGS, "Parses args."},
//    {NULL, NULL,      0,            NULL}           /* sentinel */
//};
//

PyMODINIT_FUNC PyInit_xyzzy(void) {
    return PyModule_Create(&cModPyDem);
}

