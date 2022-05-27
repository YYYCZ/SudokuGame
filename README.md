# SudokuGame
A sudoku game, also can use it to fast solve sudoku.

## How to use
If you use IDE, you can just add all the files in "src" to your IDE, and then "run" or "debug" in your IDE.

If not, you shold open the terminal, change the directory to this project, and enter the follwing commands.
> Note: In first, you need to install cmake in Linux, or to install cmake and MinGw in Windows.
```bash
cmake -S . -B ./build

# also, if you use vs, you can skip following commands
# and find ".sln" in directory "build"
cd ./build
make
```
Finally, you can see the program "Sudoku" in directory "build".
