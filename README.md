# Inlupp 2
***
This is a README file for Assignement 2 of the IOOPM-course
## Running tests
***
Use the following terminal commands to run different tests:
1.  To run tests
```bash
# runs tests for backend functions
make backend_test 
# runs tests for frontend, with script in test.text
make frontend_test
# runs both of the above
make test
# to compile all files
make all
```
2. To run the above through valgrind
```bash
make valgrind_backend
make valgrind_frontend
make valgrind
```
3. To remove compiled files, ergo entire build directory
```bash
make clean
```
## Coverage
***
We used gcov to collect coverage data.\
Use this make command:
```bash
make coverage
```
Should get the following data:
| FILE          | Lines         |  Exec   | Cover   | Branch  | Taken   |
| ------------- | ------------- | ------- | ------- | ------- | -----   |
| cart.c        | 124           | 123     | 99.19%  |   58    | 79.31%  |
| merch.c       | 109           | 104     | 95.41%  |   42    | 100%    |
| shelf.c       | 12            | 11      | 91.67%  |   42    | 100%    |
| store.c       | 94            | 83      | 88.30%  |   22    | 63.64%  |
| main.c        | 5             | 5       | 100%    |  -----  | ------  |
| tui.c         | 329           | 279     | 84.80%  |   108   | 61.11%  |

## Running program
***
Use this command to run the program: (this will build the required files)
```
make run
```
