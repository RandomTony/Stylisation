# Stylisation project
This project aim to build NRI (Non Realistic Image) from photographies. It used different algorithms to create painting-like image that you can find in the source code.
## Building the project

### Prerequisites

1. [CMake](https://cmake.org), at least version 2.8.
2. [Git](https://git-scm.com/).
3. [A C++ compiler]()
4. The [OpenCV library]()

### Download the sources

~~~
    $ git clone https://github.com/RandomTony/Stylisation.git
    $ cd stylisation
~~~

### Build the source

We use CMake to generate a Makefile. For now it build the entire project. You can not generate the makefile from the root of the project to avoid a total mess.
We advice to use the ```cmake``` command from the *build* project :
~~~
    $ cd build
    $ cmake ..
    $ make
~~~
The project outputs will be generated in *bin* folder.
## Contributing
If you want to contribute to this project, you can. Before you should read the [contribution document](CONTRIBUTING.md) and you should know that this project is under [Apache 2.0 licence](LICENSE.md).
## Testing the project
Write about the functionnality of each executable file.
