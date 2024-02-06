
5. Customizing the *Tree* genotype
==================================

#. Types of terminals 
#. Defining constants 
#. Defining Ephemereal random constants (ERCs) 
#. Adding a user defined function primitive 
#. Adding custom type terminals and functions 

(This part of the documentation presumes you have already studied the
basic usage of Tree genotype, described in the :ref:`Symbolic regression
example <gp_tree>` in Tutorial)

--------------

1. Types of terminals (in-built)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Terminals in Tree are of type *double* by default, but may be any of the
following types: DOUBLE, INT, BOOL, CHAR, STRING. The type is determined
with type name before series of terminals, e.g.:

.. code-block:: xml

   <Entry key="terminalset">DOUBLE X Y INT a b c BOOL active</Entry>

**NOTE**: currently *no type control* is implemented in ECF
(strongly-typed GP is planned), so the terminals and functions used
should be of the same type (or the user-defined type, see example
below).

 

2. Defining constants
~~~~~~~~~~~~~~~~~~~~~

Terminals of constant values are defined simply by listing them in the
"terminalset" parameter. Here the following rule applies: *the
constant's name is its value*.

.. code-block:: xml

   <Entry key="terminalset">X Y 0 1 3.141592 BOOL true false</Entry>

Alternatively, the user can introduce a variable terminal (e.g. with the
name "PI"), define its value at the begining (EvaluationOp::initialize)
and simply not change it during the evolution.

 

3. Defining Ephemereal random constants (ERCs)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

ERCs represent a special type of terminals that assume a random value upon initialization, but the initial value can later be modified with mutation or specialized operators.
ERCs of in-built types may be defined in the configuration file in the
"terminalset" variable, e.g.:

.. code-block:: xml

   <Entry key="terminalset">DOUBLE {1.5 3.14159 42} [-3.14 3.14] INT {1 2 3 4 5} BOOL {true false}</Entry>

Explanation:

-  [<lower_bound> <upper_bound>] will work only for DOUBLE typed ERCs
   and will create a random double (of uniform distribution) in the
   defined **interval**

-  {<value1> <value2> ...} will create ERCs which can assume only values
   stated in the **set**

ERCs of types other than *double* assume a specific value when inserted
in a new tree and that value remains constant during the evolution.
Double-typed ERCs *can* change value if mutated by Gaussian mutation
operator ("mut.gauss" in Tree genotype parameters).

To distinguish ERCs from constants in output, any ERC will have a prefix
denoting its type, e.g. D_1.234, I_3

 

4. Adding a user defined function primitive
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Tree genotype may accept user defined functional primitives (for an
example of a functional primitive structure, see example below or
existing primitives in 'tree' folder). Adding a functional primitive is
performed before *State* initialization:

.. code-block:: cpp

   TreeP tree = (TreeP) new Tree;             // create Tree object
   PrimitiveP prim = (PrimitiveP) new Prim;   // create primitive object
   tree->addFunction(prim);                   // add primitive to Tree
   state->addGenotype(tree);                  // add Tree (replaces original Tree)

   state->initialize(argc, argv);
   state->run();

The primitive is now added to the existing functions, but will be used
*only if* its name is stated in the "primitiveset" variable in the
configuration file.

 

5. Adding custom-type terminals and functions
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The user may want to perform tree execution with a custom data type; in
that case, both function(s) and primitive(s) of that type must be
defined. An example:

.. code-block:: cpp

	// user-defined type example
	struct my_type
	{
		double v;
		bool b;
	};

	// terminal class of type my_type
	class MyTerminal : public Primitives::Primitive
	{
	public:
		my_type value_;

		MyTerminal()
		{
			nArguments_ = 0;
		}
		void execute(void* result, Tree& tree)
		{
			my_type& res = *(my_type*)result;
			res = value_;
		}
		void setValue(void* value)
		{
			value_ = *(my_type*)value;
		}
		~MyTerminal()
		{	 }
	};

	// example function with arguments of my_type
	class MyFunc : public Primitives::Primitive
	{
	public:
		MyFunc()
		{
			nArguments_ = 2;
			name_ = "func";
		}
		void execute(void* result, Tree& tree)
		{
			my_type first, second;
			my_type& func = *(my_type*)result;

			getNextArgument(&first, tree);
			getNextArgument(&second, tree);

			// example calculation
			func.b = first.b && second.b;
			func.v = first.v + second.v;
		}
		~MyFunc()
		{	 }
	};

After this, the defined function and terminal may be added to the Tree
genotype:

.. code-block:: cpp

   TreeP tree = (TreeP) new Tree;

   PrimitiveP myFunc = (PrimitiveP) new MyFunc;
   tree->addFunction(myFunc);

   PrimitiveP myTerm = (PrimitiveP) new MyTerminal;
   myTerm->setName("term");
   tree->addTerminal(myTerm);
   // for multiple terminals of this type, new objects (with different name) must be created

   state->addGenotype(tree);

   state->initialize(argc, argv);
   state->run();

Finally, the functions and terminals may be used by stating them in the
configuration file, e.g.:

.. code-block:: xml

   <Tree>
       <Entry key="functionset">func</Entry>
       <Entry key="terminalset">term</Entry>
   </Tree>

| 

