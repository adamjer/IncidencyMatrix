#include <Python.h>
#include <vector>
#include <algorithm>
#include <string>
#include <structmember.h>
#include <iostream>

#define MAX_NUMBER_OF_VERTEX 16
#define DEBUG

using namespace std;

class IncidencyMatrix
{
public:
	IncidencyMatrix()
	{
		this->numberOfVertices = 0;
	}

	~IncidencyMatrix()
	{
		this->clearGraph();
	}

	void init(const int n)
	{
		this->numberOfVertices = n;
		this->clearGraph();
		this->initialVertices();
	}

	void addVertex()
	{
		vector<bool> temp;
		this->matrix.push_back(temp);
		++this->numberOfVertices;
	}

	void addVertex(const int vertex)
	{
		vector<bool> temp = vector<bool>(false, this->numberOfEdges());
		this->matrix.insert(this->matrix.begin() + vertex - 1, temp);
	}

	void addEdge(const int vertex1, const int vertex2)
	{
		for (vector<vector<bool>>::iterator it = this->matrix.begin(); it != this->matrix.end(); it++)
		{
			it->push_back(false);
		}
		*this->matrix[vertex1].end() = true;
		*this->matrix[vertex2].end() = true;
	}

	bool isEdge(const int u, const int v)
	{
		if (this->findEdge(u, v) >= 0)
			return true;

		return false;
	}

	int findEdge(const int u, const int v)
	{
		for (int i = 0; i < this->numberOfEdges(); i++)
		{
			//both '1'
			if (this->matrix[u][i] && this->matrix[v][i])
				return i;
		}
		return -1;
	}

	void deleteEdge(const int u, const int v)
	{
		int edge = this->findEdge(u, v);
		if (edge >= 0)
		{
			for (vector<vector<bool>>::iterator it = this->matrix.begin(); it != this->matrix.end(); it++)
			{
				it->erase(it->begin() + edge);
			}
		}
	}

	int isEqual(IncidencyMatrix& other)
	{
		if (this->order() != other.order())
			return false;
		if (this->numberOfEdges() != other.numberOfEdges())
			return false;

		for (vector<vector<bool>>::iterator it = this->matrix.begin(); it != this->matrix.end(); it++)
		{
			for()
		}

		return true;
	}

	int order()
	{
		return this->numberOfVertices;
	}

private:

	int numberOfEdges()
	{
		return this->matrix[0].size();
	}

	void initialVertices()
	{
		for (int i = 0; i < this->numberOfVertices; ++i)
		{
			vector<bool> temp = vector<bool>();
			this->matrix.push_back(temp);
		}
	}

	void clearGraph()
	{
		for (vector<vector<bool>>::iterator it = this->matrix.begin(); it != this->matrix.end(); it++)
		{
			it->clear();
		}
		this->matrix.clear();
	}

	vector<vector<bool>> matrix;
	int numberOfVertices;
};

static PyObject* G6Error;
static PyObject* NoVerticesError;
static PyObject* TooManyVerticesError;

typedef struct {
	PyObject_HEAD
		int numberOfVertices;
	IncidencyMatrix* incidencyMatrix;
} SimpleGraph;

static PyObject* SimpleGraph__new__(PyTypeObject * type, PyObject * args) {
	return type->tp_alloc(type, 0);
}

static void SimpleGraph__del__(SimpleGraph * self) {
	delete[] self->incidencyMatrix;
	Py_TYPE(self)->tp_free((PyObject*)self);
}

static PyObject* SimpleGraph__new__(PyTypeObject* type, PyObject* args) {
	return type->tp_alloc(type, 0);
}

static void SimpleGraph__del__(SimpleGraph* self) {
#ifdef DEBUG
	printf("Delete\n");
#endif // DEBUG

	delete[] self->incidencyMatrix;
	Py_TYPE(self)->tp_free((PyObject*)self);
}

static PyObject* fromString(SimpleGraph* self, PyObject* args) {
	char* arg;
	if (!PyArg_ParseTuple(args, "|s", &arg)) {
		return NULL;
	}
	if (!convertFromString(self, std::string(arg))) {
		return NULL;
	}
	return Py_None;
}

static int SimpleGraph__init__(SimpleGraph* self, PyObject* args) {
	char* arg = NULL;

	if (!PyArg_ParseTuple(args, "|z", &arg)) {
		return -1;
	}
	self->listOfAdjacency = new List[MAX_NUMBER_OF_VERTEX];
	if (arg == NULL) {
		self->numberOfVertex = 1;
		return 0;
	}
	if (!convertFromString(self, std::string(arg))) {
		return -1;
	}

	return 0;
}

static PyObject* SimpleGraph__str__(SimpleGraph* self) {
	return PyUnicode_FromString(convertToG6Format(self).c_str());
}

static PyObject* SimpleGraph__comp__(SimpleGraph* self, PyObject* args, int op) {
	SimpleGraph* other = (SimpleGraph*)args;
	bool result = compare(self, other);
	if (op == Py_EQ) {
		return PyBool_FromLong(result);
	}
	if (op == Py_NE) {
		return PyBool_FromLong(!result);
	}
	return PyBool_FromLong(0);
}

//METODY
static PyObject* order(SimpleGraph* self) {
#ifdef DEBUG
	//printf("Order\n");
	//printEdges(self);
#endif

	return PyLong_FromLong((long)self->numberOfVertex);
}

static PyObject* deleteEdge(SimpleGraph* self, PyObject* args) {
#ifdef DEBUG
	printf("Delete edge\n");
	printEdges(self);
#endif

	int v1, v2;
	if (!PyArg_ParseTuple(args, "|ii", &v1, &v2)) {
		return NULL;
	}
	removeEdge(self, v1, v2);
	return PyBool_FromLong(true);
}

static PyObject* addEdge(SimpleGraph* self, PyObject* args) {
#ifdef DEBUG
	printf("Add edge\n");
	printEdges(self);
#endif
	int v1, v2;
	if (!PyArg_ParseTuple(args, "|ii", &v1, &v2)) {
		return NULL;
	}
	createEdge(self, v1, v2);
	return PyBool_FromLong(true);
}

static PyObject* isEdge(SimpleGraph* self, PyObject* args) {
#ifdef DEBUG
	printf("Check is edge\n");
	printEdges(self);
#endif
	int v1, v2;
	if (!PyArg_ParseTuple(args, "|ii", &v1, &v2)) {
		return NULL;
	}
	return PyBool_FromLong(self->listOfAdjacency[v1].isExist(v2));
}

static PyObject* addVertex(SimpleGraph* self) {
#ifdef DEBUG
	printf("Add vertex\n");
#endif
	if (self->numberOfVertex < MAX_NUMBER_OF_VERTEX) {
		self->numberOfVertex++;
	}
	else {
		PyErr_SetString(TooManyVerticesError, "");
		return NULL;
	}
#ifdef DEBUG
	printf("End\n");
#endif
	return PyBool_FromLong(true);
}

static PyObject* deleteVertex(SimpleGraph* self, PyObject* args) {
#ifdef DEBUG
	printf("Delete vartex\n");
#endif
	int vertex;
	if (!PyArg_ParseTuple(args, "|i", &vertex)) {
		return NULL;
	}
	if (self->numberOfVertex == 1) {
		PyErr_SetString(NoVerticesError, "");
		return false;
	}
	int size = self->listOfAdjacency[vertex].getSize();
	for (int i = 0; i < size; i++) {
		removeEdge(self, vertex, self->listOfAdjacency[vertex][i]);
	}
	for (int i = vertex; i < self->numberOfVertex - 1; i++) {
		self->listOfAdjacency[i] = self->listOfAdjacency[i + 1];
		for (int j = 0; j < self->numberOfVertex; j++) {
			self->listOfAdjacency[j].changeValue(i + 1, i);
		}
	}
	self->listOfAdjacency[self->numberOfVertex - 1].clear();
	self->numberOfVertex--;
	return PyBool_FromLong(true);
}

static PyMethodDef methods[] = {
	{ "addEdge", (PyCFunction)addEdge, METH_VARARGS, nullptr },
	{ "addVertex", (PyCFunction)addVertex, METH_NOARGS, nullptr},
	{ "deleteEdge", (PyCFunction)deleteEdge, METH_VARARGS, nullptr },
	{ "deleteVertex", (PyCFunction)deleteVertex, METH_VARARGS, nullptr },
	{ "isEdge", (PyCFunction)isEdge, METH_VARARGS, nullptr },
	{ "fromString", (PyCFunction)fromString, METH_VARARGS, nullptr },
	{ "order", (PyCFunction)order, METH_NOARGS, nullptr },
	{ NULL }
};

static PyMemberDef members[] = {
	{NULL}
};

static PyTypeObject SimpleGraphType = {
	PyVarObject_HEAD_INIT(NULL, 0)
	"simple_graphs.IncidencyMatrix",
	sizeof(SimpleGraph),
	0,
	(destructor)SimpleGraph__del__,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	(reprfunc)SimpleGraph__str__,
	0, 0, 0,
	Py_TPFLAGS_DEFAULT,
	"Graph represented with incidency matrix with maximum number of 16 vertices",
	0, 0,
	(richcmpfunc)SimpleGraph__comp__,
	0, 0, 0,
	methods,
	members, 0, 0, 0, 0, 0, 0,
	(initproc)SimpleGraph__init__,
	0,
	(newfunc)SimpleGraph__new__
};

static PyModuleDef simple_graphs = {
	PyModuleDef_HEAD_INIT,
	"simple_graphs",
	"Macierz incydencji",
	-1,
	NULL, NULL, NULL, NULL, NULL
};

PyMODINIT_FUNC PyInit_simple_graphs()
{
	SimpleGraphType.tp_dict = PyDict_New();
	if (!SimpleGraphType.tp_dict)
	{
		return NULL;
	}

	G6Error = PyErr_NewException("IncidencyMatrix.G6Error", PyExc_BaseException, NULL);
	Py_INCREF(G6Error);
	PyDict_SetItemString(SimpleGraphType.tp_dict, "G6Error", G6Error);
	NoVerticesError = PyErr_NewException("IncidencyMatrix.NoVerticesError", PyExc_BaseException, NULL);
	Py_INCREF(NoVerticesError);
	PyDict_SetItemString(SimpleGraphType.tp_dict, "NoVerticesError", NoVerticesError);
	TooManyVerticesError = PyErr_NewException("IncidencyMatrix.TooManyVerticesError", PyExc_BaseException, NULL);
	Py_INCREF(TooManyVerticesError);
	PyDict_SetItemString(SimpleGraphType.tp_dict, "TooManyVerticesError", TooManyVerticesError);
	PyType_Modified((PyTypeObject*)& SimpleGraphType);

	if (PyType_Ready(&SimpleGraphType) < 0)
		return NULL;

	PyObject* m = PyModule_Create(&simple_graphs);

	if (m == NULL)
		return NULL;

	Py_INCREF(&SimpleGraphType);
	PyModule_AddObject(m, "IncidencyMatrix",
		(PyObject*)& SimpleGraphType);
	return m;
}