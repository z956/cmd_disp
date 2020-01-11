Command Dispatcher
===========
A command dispatcher for user to add and dispatch command easily.

Usage
-----
Include header ``dispatcher.h``.

A command processing function can be a free function or a class member function.
Use ``DISP_DEFINE_CMD`` to define it as a command function that the dispatcher accepts.
.. code:: c++

    // A free function to process command
    void func1(int a, double b)
    {
        // ...
    }

    // The first argument is the function name
    // Remains are the parameter types
    DISP_DEFINE_CMD(func1, int, double);

    struct Class
    {
        // A class member function to process command
        void func2(char a, int b)
        {
            // ...
        }

        // Define func2 as a command function
        DISP_DEFINE_CMD(func2, int, double);
    };

Create an instance of the dispatcher and add functions into it.
.. code:: c++

    cmd_disp::Dispatcher disp;

    // Add func1 into disp with the name "func1"
    disp.add("func1", &func1);

    // Add func2 into disp with the name "func2"
    // Must provide the object pointer when adding a class member function
    Class c;
    disp.add("func2", &c, &Class::func2);

    // Use dispatch to dispatch commands to functions
    // Dispatcher will recognize the first arguments as the name

    // dispatch command to func1
    disp.dispatch("func1 5566 3.1415926");

    // dispatch command to func2
    disp.dispatch("func2 g 87");

By default, parameters are tokenized by spaces.
Users can customize the tokenizer.
.. code:: c++
    class ExampleTokenizer
    {
        std::vector<std::string> args;

    public:
        // The constructor should accept a string of command
        // and tokenize the command
        ExampleTokenizer(const std::string& command)
        {
            // Tokenize the command and store in args
            std::stringstream ss(cmd);
            std::string s;

            while (std::getline(ss, s, ' '))
            {
                args.push_back(s);
            }
        }

        // A tokenizer must provide "Iterator" type
        // for the dispatcher to iterate tokenized arguments
        using Iterator = std::vector<std::string>::const_iterator;

        // begin() provides an iterator pointing to the first argument
        // (usually it is the first argument after the name)
        Iterator begin() const
        {
            return args.begin() + 1;
        }

        // end() provides the end of the iterator
        Iterator end() const
        {
            return args.end();
        }

        // name() provides the names of this command
        std::string name() const
        {
            return args.at(0);
        }
    };

    // Use ExampleTokenizer as the tokenizer for the dispatcher
    cmd_disp::Dispatcher<ExampleTokenizer> disp;

LIMITATION
----
* The name of a command function must be a string.
* The translation function set is not flexible. Should provide customized translation set.
* The input of function dispatch() must be a string.
* Command functions cannot be overloadded.
.. code:: c++
    void func(int, int)
    {
    }
    DISP_DEFINE_CMD(func, int, int);

    void func(double, double)
    {
    }
    DISP_DEFINE_CMD(func, double, double);

    cmd_disp::Dispatcher disp;

    // Error, compiler cannot determine which func to use
    disp.add("func", &func);

    // OK, use the func that accept two double arguments
    disp.add("func", static_cast<void(*)(double, double)>(&func));

* The default values of a command functions cannot affect dispatching.
.. code:: c++
    void func(int a, int b = 5)
    {
    }
    DISP_DEFINE_CMD(func, int, int);

    cmd_disp::Dispatcher disp;
    disp.add("func", &func);

    // Users may expect that func is called with argument (a = 20, b = 5)
    // This will cause a runtime error because it cannot handle the default value
    disp.dispatch("func 20");

    // OK, no use the default value
    disp.dispatch("func 20 5");

TODO
----
* Provide customized type of name
* Provide customized translation set.
* Provide customized type of input to dispatch().
* Provide a well interface to handle errors.
