#include <Python.h>
#include <vector>
#include <algorithm>
#include <string>
#include <structmember.h>
#include <iostream>

#define MAX_NUMBER_OF_VERTEX 16
//#define DEBUG

using namespace std;

class IncidencyMatrix
{
public:
	IncidencyMatrix()
	{
		this->numberOfVertices = 0;
		this->numberOfEdges = 0;
		this->matrix = vector<vector<bool>>();
	}

	~IncidencyMatrix()
	{
		this->clearGraph();
	}

	void init(const int n)
	{
		this->clearGraph();
		this->numberOfVertices = n;
		this->initialVertices();
	}

	void addVertex()
	{
		vector<bool> temp(this->numberOfEdges, false);
		this->matrix.push_back(temp);
		++this->numberOfVertices;
	}

	void addVertex(const int vertex)
	{
		vector<bool> temp = vector<bool>(this->numberOfEdges, false);
		this->matrix.insert(this->matrix.begin() + vertex - 1, temp);
		++this->numberOfVertices;
	}

	void deleteVertex(const int vertex)
	{

		int edges = this->numberOfEdges - 1;
		for (int i = edges; i >= 0; i--)
		{
			if (this->matrix[vertex][i])
				this->deleteEdge(i);
		}
		this->matrix.erase(this->matrix.begin() + vertex);
		--this->numberOfVertices;
	}

	void addEdge(const int vertex1, const int vertex2)
	{
		for (vector<vector<bool>>::iterator it = this->matrix.begin(); it != this->matrix.end(); it++)
		{
			it->push_back(false);
		}
		this->matrix[vertex1][this->numberOfEdges] = true;
		this->matrix[vertex2][this->numberOfEdges] = true;
		++this->numberOfEdges;
	}

	bool isEdge(const int u, const int v)
	{
		if (this->findEdge(u, v) >= 0)
			return true;

		return false;
	}

	void deleteEdge(const int u, const int v)
	{
		int edge = this->findEdge(u, v);
		if (edge >= 0)
		{
			this->deleteEdge(edge);
			--this->numberOfEdges;
		}
	}

	bool operator ==(IncidencyMatrix* other)
	{
		if (this->order() != other->order())
			return false;
		if (this->numberOfEdges != other->numberOfEdges)
			return false;

		for (int i = 0; i < this->numberOfVertices; i++)
		{
			for (int j = 0; j < this->numberOfEdges; j++)
			{
				if (this->matrix[i][j] != other->matrix[i][j])
					return false;
			}
		}

		return true;
	}

	bool operator !=(IncidencyMatrix* other)
	{
		if (this->order() != other->order())
			return true;
		if (this->numberOfEdges != other->numberOfEdges)
			return true;

		for (int i = 0; i < this->numberOfVertices; i++)
		{
			for (int j = 0; j < this->numberOfEdges; j++)
			{
				if (this->matrix[i][j] != other->matrix[i][j])
					return true;
			}
		}

		return false;
	}

	int order()
	{
		return this->numberOfVertices;
	}

	void setNumbersOfVertices(const int n)
	{
		this->numberOfVertices = n;
	}

	int getNumberOfEdges()
	{
		return this->numberOfEdges;
	}

	void print()
	{
		cout << "E" << this->numberOfEdges << "\n";
		for (int i = 0; i < this->numberOfVertices; i++)
		{
			cout << "V" << i + 1;
			if (i < 9)
				cout << " ";
			cout << " ";

			for (int j = 0; j < this->numberOfEdges; j++)
			{
				cout << to_string(this->matrix[i][j]) << " ";
			}
			cout << "\n";
		}
	}

private:

	void deleteEdge(const int edge)
	{
		for (vector<vector<bool>>::iterator it = this->matrix.begin(); it != this->matrix.end(); it++)
		{
			it->erase(it->begin() + edge);
		}
	}

	int findEdge(const int u, const int v)
	{
		for (int i = 0; i < this->numberOfEdges; i++)
		{
			//both '1'
			if (this->matrix[u][i] && this->matrix[v][i])
				return i;
		}
		return -1;
	}

	void initialVertices()
	{
		for (int i = 0; i < this->numberOfVertices; i++)
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
		this->numberOfEdges = 0;
		this->numberOfVertices = 0;
	}

	vector<vector<bool>> matrix;
	int numberOfVertices, numberOfEdges;
};


static PyObject* G6Error;
static PyObject* NoVerticesError;
static PyObject* TooManyVerticesError;

typedef struct {
	PyObject_HEAD
		IncidencyMatrix* incidencyMatrix;
} SimpleGraph;

string toBinary(int number)
{
	auto res = string("");
	for (int i = 0; i < 6; i++) {
		res = to_string(number % 2) + res;
		number /= 2;
	}
	return res;
}

bool createGraph(SimpleGraph* self, string text)
{
	if (text.empty())
	{
		PyErr_SetString(G6Error, "text is empty");
		return false;
	}

	int tmp = (int)text[0] - 63;
	if (tmp > MAX_NUMBER_OF_VERTEX || tmp < 1)
	{
		PyErr_SetString(G6Error, "wrong number of vertices");
		return false;
	}

	string binaryForm = "";

	for (int i = 1; i < (int)text.size(); i++)
	{
		int asciiValue = (int)text[i] - 63;
		if (asciiValue > 63 || asciiValue < 0)
		{
			PyErr_SetString(G6Error, "invalid values in");
			return false;
		}
		binaryForm += toBinary(asciiValue);
	}

	int strPtr = 0;

	self->incidencyMatrix->init(tmp);
	for (int i = 0; i < tmp; i++)
	{
		for (int j = 0; j < i; j++)
		{
			if (binaryForm[strPtr++] == '1')
				self->incidencyMatrix->addEdge(i, j);
		}
	}
	return true;
}

static PyObject* SimpleGraph__new__(PyTypeObject * type, PyObject * args)
{
#ifdef DEBUG
	printf("New Begin\n");
#endif
	return type->tp_alloc(type, 0);
}

static int SimpleGraph__init__(SimpleGraph* self, PyObject* args)
{
#ifdef DEBUG
	printf("Init Begin n = ");
#endif
	char* arg = NULL;

	if (!PyArg_ParseTuple(args, "|z", &arg))
	{
#ifdef DEBUG
		printf("\n");
#endif
		return -1;
	}

	self->incidencyMatrix = new IncidencyMatrix();
	if (arg == NULL)
	{
#ifdef DEBUG
		printf("1\n");
#endif
		self->incidencyMatrix->init(1);
		return 0;
	}

	if (!createGraph(self, string(arg)))
	{
#ifdef DEBUG
		cout << string(arg) << "\n";
#endif
		return -1;
	}

#ifdef DEBUG
	printf("\n");
#endif

	return 0;
}

static void SimpleGraph__del__(SimpleGraph* self)
{
#ifdef DEBUG
	cout << "Delete Begin\n";
	self->incidencyMatrix->print();
#endif

	delete self->incidencyMatrix;
	Py_TYPE(self)->tp_free((PyObject*)self);
}

static PyObject* fromString(SimpleGraph* self, PyObject* args)
{
	char* arg;
	if (!PyArg_ParseTuple(args, "|s", &arg))
		return NULL;

	if (!createGraph(self, string(arg)))
		return NULL;

	return Py_None;
}

string edgesToBinary(SimpleGraph* self)
{
	string binaryForm = "";

	for (int i = 0; i < self->incidencyMatrix->order(); i++)
	{
		for (int j = 0; j < i; j++)
		{
			binaryForm += self->incidencyMatrix->isEdge(i, j) ? "1" : "0";
		}
	}

	while (binaryForm.size() % 6 != 0)
		binaryForm += "0";

	return binaryForm;
}

string toG6(SimpleGraph* self)
{
	char firstChar = self->incidencyMatrix->order() + 63;
	string result = "";

	result.push_back(firstChar);
	string binaryForm = edgesToBinary(self);
	for (int i = 0; i < (int)(binaryForm.size() / 6); i++)
	{
		char tmp = 63;
		for (int j = 5; j >= 0; j--)
		{
			if (binaryForm[i * 6 + j] == '1')
				tmp += (int)pow(2, 5 - j);
		}
		result.push_back(tmp);
	}
	return result;
}

static PyObject* SimpleGraph__str__(SimpleGraph* self)
{
	return PyUnicode_FromString(toG6(self).c_str());
}

static PyObject* SimpleGraph__comp__(SimpleGraph* self, PyObject* args, int op)
{
	SimpleGraph* other = (SimpleGraph*)args;
	bool result = (self->incidencyMatrix == other->incidencyMatrix);
	if (op == Py_EQ)
	{
		return PyBool_FromLong(result);
	}
	if (op == Py_NE)
	{
		return PyBool_FromLong(!result);
	}
	return PyBool_FromLong(0);
}

//METODY
static PyObject* order(SimpleGraph* self)
{
#ifdef DEBUG
	printf("Order\n");
#endif

	return PyLong_FromLong((long)self->incidencyMatrix->order());
}

static PyObject* deleteEdge(SimpleGraph* self, PyObject* args)
{
#ifdef DEBUG
	printf("Delete edge\n");
#endif

	int v1, v2;
	if (!PyArg_ParseTuple(args, "|ii", &v1, &v2))
		return NULL;

	self->incidencyMatrix->deleteEdge(v1, v2);

#ifdef DEBUG
	printf("V%d -> V%d\n", v1 + 1, v2 + 1);
	self->incidencyMatrix->print();
#endif

	return PyBool_FromLong(true);
}

static PyObject* addEdge(SimpleGraph* self, PyObject* args)
{
#ifdef DEBUG
	printf("Add edge\n");
#endif

	int v1, v2;
	if (!PyArg_ParseTuple(args, "|ii", &v1, &v2))
		return NULL;

	self->incidencyMatrix->addEdge(v1, v2);

#ifdef DEBUG
	printf("V%d -> V%d\n", v1 + 1, v2 + 1);
	self->incidencyMatrix->print();
#endif

	return PyBool_FromLong(true);
}

static PyObject* isEdge(SimpleGraph* self, PyObject* args)
{
#ifdef DEBUG
	//	printf("Check is edge\n");
#endif
	int v1, v2;
	if (!PyArg_ParseTuple(args, "|ii", &v1, &v2))
		return NULL;

	return PyBool_FromLong(self->incidencyMatrix->isEdge(v1, v2));
}

static PyObject* addVertex(SimpleGraph* self) 
{
#ifdef DEBUG
	printf("Add vertex\n");
#endif
	if (self->incidencyMatrix->order() < MAX_NUMBER_OF_VERTEX)
	{
		self->incidencyMatrix->addVertex();
	}
	else
	{
		PyErr_SetString(TooManyVerticesError, "");
		return NULL;
	}

#ifdef DEBUG
	self->incidencyMatrix->print();
#endif

	return PyBool_FromLong(true);
}

static PyObject* deleteVertex(SimpleGraph* self, PyObject* args) 
{
#ifdef DEBUG
	printf("Delete vertex\n\tOrder = %d\n", self->incidencyMatrix->order());
#endif

	int vertex;
	if (!PyArg_ParseTuple(args, "|i", &vertex))
		return NULL;
	
	if (self->incidencyMatrix->order() == 1)
	{
		PyErr_SetString(NoVerticesError, "");
		return NULL;
	}

	self->incidencyMatrix->deleteVertex(vertex);

#ifdef DEBUG
	self->incidencyMatrix->print();
#endif

	return PyBool_FromLong(true);
}

static PyMethodDef methods[] = 
{
	{ "order", (PyCFunction)order, METH_NOARGS, nullptr },
	{ "addVertex", (PyCFunction)addVertex, METH_NOARGS, nullptr},
	{ "deleteVertex", (PyCFunction)deleteVertex, METH_VARARGS, nullptr },
	{ "addEdge", (PyCFunction)addEdge, METH_VARARGS, nullptr },
	{ "deleteEdge", (PyCFunction)deleteEdge, METH_VARARGS, nullptr },
	{ "isEdge", (PyCFunction)isEdge, METH_VARARGS, nullptr },
	{ "fromString", (PyCFunction)fromString, METH_VARARGS, nullptr },
	{ NULL }
};

static PyMemberDef members[] = 
{
	{NULL}
};

static PyTypeObject SimpleGraphType = 
{
	PyVarObject_HEAD_INIT(NULL, 0)
	"simple_graphs.IncidencyMatrix",
	sizeof(SimpleGraph),
	0,
	(destructor)SimpleGraph__del__,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	(reprfunc)SimpleGraph__str__,
	0, 0, 0,
	Py_TPFLAGS_DEFAULT,
	"Graph represented with incidency matrix with maximum number of 16 vertices G6 representation",
	0, 0,
	(richcmpfunc)SimpleGraph__comp__,
	0, 0, 0,
	methods,
	members, 0, 0, 0, 0, 0, 0,
	(initproc)SimpleGraph__init__,
	0,
	(newfunc)SimpleGraph__new__
};

static PyModuleDef simple_graphs = 
{
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