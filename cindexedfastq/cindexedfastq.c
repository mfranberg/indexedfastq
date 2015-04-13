#include <Python.h>

#include <ifq.h>

/**
 * Wrapper object for a indexed fastq file. In python it will
 * act as a handle to the file.
 */
typedef struct
{
    PyObject_HEAD

    ifq_index_t index;
    ifq_record_t *record;
} c_indexed_fastq_t;

/**
 * Deallocates a Python CIndexedFastq object.
 * 
 * @param self Pointer to a c_indexed_fastq_t.
 */
void
c_indexed_fastq_dealloc(c_indexed_fastq_t *self)
{
    if( self->record != NULL )
    {
        ifq_destroy_record( self->record );
        ifq_destroy_index( &self->index );
        self->record = NULL;
        Py_TYPE( self )->tp_free( ( PyObject * ) self );
    }
}

#if PY_MAJOR_VERSION >= 3

/**
 * Python type of the above.
 */
static PyTypeObject c_indexed_fastq_prototype =
{
    PyVarObject_HEAD_INIT( NULL, 0 )
    "indexedfastq.CIndexedFastq",       /* tp_name */
    sizeof( c_indexed_fastq_t ),       /* tp_basicsize */
    0,                          /* tp_itemsize */
    (destructor) c_indexed_fastq_dealloc, /* tp_dealloc */
    0,                          /* tp_print */
    0,                          /* tp_getattr */
    0,                          /* tp_setattr */
    0,                          /* tp_compare */
    0,                          /* tp_repr */
    0,                          /* tp_as_number */
    0,                          /* tp_as_sequence */
    0,                          /* tp_as_mapping */
    0,                          /* tp_hash */
    0,                          /* tp_call */
    0,                          /* tp_str */
    0,                          /* tp_getattro */
    0,                          /* tp_setattro */
    0,                          /* tp_as_buffer */
    Py_TPFLAGS_DEFAULT,         /* tp_flags */
    "Contains the C part of the indexed fastq.",           /* tp_doc */
};

#else

/**
 * Python type of the above.
 */
static PyTypeObject c_indexed_fastq_prototype =
{
    PyObject_HEAD_INIT( NULL )
    0,
    "indexedfastq.CIndexedFastq",       /* tp_name */
    sizeof( c_indexed_fastq_t ),       /* tp_basicsize */
    0,                          /* tp_itemsize */
    (destructor) c_indexed_fastq_dealloc, /* tp_dealloc */
    0,                          /* tp_print */
    0,                          /* tp_getattr */
    0,                          /* tp_setattr */
    0,                          /* tp_compare */
    0,                          /* tp_repr */
    0,                          /* tp_as_number */
    0,                          /* tp_as_sequence */
    0,                          /* tp_as_mapping */
    0,                          /* tp_hash */
    0,                          /* tp_call */
    0,                          /* tp_str */
    0,                          /* tp_getattro */
    0,                          /* tp_setattro */
    0,                          /* tp_as_buffer */
    Py_TPFLAGS_DEFAULT,         /* tp_flags */
    "Contains the C part of the indexed fastq.",           /* tp_doc */
};

#endif

c_indexed_fastq_t * open_index(char *fastq_path, char *index_prefix)
{
    ifq_index_t index;
    c_indexed_fastq_t *cifq;
    ifq_codes_t status = ifq_open_index( fastq_path, index_prefix, &index );
    if( status != IFQ_OK )
    {
        if( status == IFQ_BAD_FASTQ )
        {
            PyErr_SetString( PyExc_IOError, "Error while opening fastq file." );
        }
        else if( status == IFQ_BAD_PREFIX )
        {
            PyErr_SetString( PyExc_IOError, "Error while opening index files." );
        }
        else
        {
            PyErr_SetString( PyExc_IOError, "Unknown error while reading the index." );
        }

        return NULL;
    }
    
    cifq = (c_indexed_fastq_t *) c_indexed_fastq_prototype.tp_alloc( &c_indexed_fastq_prototype, 0 );
    cifq->index = index;
    cifq->record = ifq_new_record( );

    return cifq;
}

static PyObject *py_create_indexed_fastq(PyObject *self, PyObject *args)
{
    char *fastq_path;
    char *index_prefix;

    if( !PyArg_ParseTuple( args, "ss", &fastq_path, &index_prefix ) )
    {
        return NULL;
    }
    
    ifq_codes_t status = ifq_create_index( fastq_path, index_prefix );
    if( status != IFQ_OK )
    {
        if( status == IFQ_BAD_FASTQ )
        {
            PyErr_SetString( PyExc_IOError, "Error while opening fastq file." );
        }
        else if( status == IFQ_BAD_PREFIX )
        {
            PyErr_SetString( PyExc_IOError, "Error while opening index files." );
        }
        else
        {
            PyErr_SetString( PyExc_IOError, "Error while creating the index." );
        }
        return NULL;
    }

    return (PyObject *) open_index( fastq_path, index_prefix );
}

static PyObject *py_open_indexed_fastq(PyObject *self, PyObject *args)
{
    char *fastq_path;
    char *index_prefix;

    if( !PyArg_ParseTuple( args, "ss", &fastq_path, &index_prefix ) )
    {
        return NULL;
    }

    return (PyObject *) open_index( fastq_path, index_prefix );
}

static PyObject *py_query_indexed_fastq(PyObject *self, PyObject *args)
{
    char *query;
    c_indexed_fastq_t *cifq;

    if( !PyArg_ParseTuple( args, "O!s", &c_indexed_fastq_prototype, &cifq, &query ) )
    {
        return NULL;
    }

    if( ifq_query_index( &cifq->index, query, cifq->record ) == IFQ_OK )
    {
        return Py_BuildValue( "sss", cifq->record->name, cifq->record->sequence, cifq->record->quality );
    }
    else
    {
        return Py_BuildValue( "sss", NULL, NULL, NULL );
    }
}

static PyObject *py_close_indexed_fastq(PyObject *self, PyObject *args)
{ 
    c_indexed_fastq_t *cifq;

    if( !PyArg_ParseTuple( args, "O!", &c_indexed_fastq_prototype, &cifq ) )
    {
        return NULL;
    }

    ifq_destroy_record( cifq->record );
    ifq_destroy_index( &cifq->index );
    cifq->record = NULL;

    Py_RETURN_NONE;
}

static PyMethodDef module_methods[] = {
    { "create_indexed_fastq", py_create_indexed_fastq, METH_VARARGS, "Create an index and return it." },
    { "open_indexed_fastq", py_open_indexed_fastq, METH_VARARGS, "Open an already indexed file." },
    { "query_indexed_fastq", py_query_indexed_fastq, METH_VARARGS, "Query and indexed fastq." },
    { "close_indexed_fastq", py_close_indexed_fastq, METH_VARARGS, "Close an opened index." },
    { NULL, NULL, 0, NULL }
};

#if PY_MAJOR_VERSION  >= 3

static PyModuleDef moduledef = {
    PyModuleDef_HEAD_INIT,
    "cindexedfastq",
    "Wrapper module for the cindexedfastq methods.",
    -1,
    module_methods,
    NULL,
    NULL,
    NULL,
    NULL
};

PyMODINIT_FUNC
PyInit_cindexedfastq(void)
{
    PyObject *module;
    
    c_indexed_fastq_prototype.tp_new = PyType_GenericNew;
    if( PyType_Ready( &c_indexed_fastq_prototype ) < 0 )
    {
        return NULL;
    }

    module = PyModule_Create( &moduledef );
    
    Py_INCREF( &c_indexed_fastq_prototype );
    PyModule_AddObject( module, "CIndexedFastq", (PyObject *) &c_indexed_fastq_prototype );

    return module;
}

#else

PyMODINIT_FUNC
initcindexedfastq(void)
{
    PyObject *module;

    c_indexed_fastq_prototype.tp_new = PyType_GenericNew;
    if( PyType_Ready( &c_indexed_fastq_prototype ) < 0 )
    {
        return;
    }

    module = Py_InitModule3( "cindexedfastq", module_methods, "Wrapper module for the cindexedfastq methods." );

    Py_INCREF( &c_indexed_fastq_prototype );
    PyModule_AddObject( module, "CIndexedFastq", (PyObject *) &c_indexed_fastq_prototype );
}

#endif
