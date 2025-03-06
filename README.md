# Motor-controller-PID-tuner
Machine Learning PID tuner for Motor controller

## Before cloning
1. Make example ESP-IDF project
2. Copy `.vscode` and `.devcontainer` directories
3. Clone repo
4. Copy those directories into this repo
---
(See the README.md file in the upper level 'examples' directory for more information about examples.)

This is the simplest buildable example. The example is used by command `idf.py create-project`
that copies the project to user specified path and set it's name. For more information follow the [docs page](https://docs.espressif.com/projects/esp-idf/en/latest/api-guides/build-system.html#start-a-new-project)



## How to use example
We encourage the users to use the example as a template for the new projects.
A recommended way is to follow the instructions on a [docs page](https://docs.espressif.com/projects/esp-idf/en/latest/api-guides/build-system.html#start-a-new-project).

## Example folder contents

The project **sample_project** contains one source file in C language [main.c](main/main.c). The file is located in folder [main](main).

ESP-IDF projects are built using CMake. The project build configuration is contained in `CMakeLists.txt`
files that provide set of directives and instructions describing the project's source files and targets
(executable, library, or both). 

Below is short explanation of remaining files in the project folder.

```
├── CMakeLists.txt
├── main
│   ├── CMakeLists.txt
│   └── main.c
└── README.md                  This is the file you are currently reading
```
Additionally, the sample project contains Makefile and component.mk files, used for the legacy Make based build system. 
They are not used or needed when building with CMake and idf.py.




## Pinouts
These are the agreed upon pinouts:
- Pin a and b dictate direction (GPIO) Connect to in1 and in2 on H-Bridge
- Pin c is PWM signal connects to ENA on H-Bridge
- Negative 3.3-5v to black encoder wire
- Positive 3.3-5v to blue encoder wire
- Pin d to Yellow encoder wire
- Pin e to green encoder wire