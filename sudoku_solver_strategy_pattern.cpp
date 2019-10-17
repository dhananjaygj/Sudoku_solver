/*
 * @brief Implementation of C++ based Sudoku Solver
 * 
 * Sudoku is a number placement puzzle. The goal is to fill a 9×9 grid
 * with digits so that each column, each row, and each of the nine 
 * 3×3 subgrids that compose the grid ("blocks") contain all of the 
 * digits from 1 to 9. This file contains the different implementation
 * strategies to solve this puzzle.
 * 
 * This implementation also uses Strategy Design pattern under the 
 * SudokuSolver class to allow for variation in different solving methods 
 * and input methods.
 * 
 * Common namings used in this file under sudoku context:
 * 1) "grid" is a 9x9 matrix which can be either input or output.
 * 2) "block" is a subgrid of sudoku matrix. There are 9 blocks.
 * 3) "possibility grid" is a 3 dimensional vector holding the all possible
 * digits that can be filled in an unfilled sudoku cell location and for 
 * all 81 locations.
 *
 * Note: sudoku solving logic is based on https://norvig.com/sudoku.html
 * 
 * Compile with "g++ sudoku_solver_strategy_pattern.cpp -o sudoku_solver"
 * 
 * @author Dhananjay G J <dhguja@gmail.com>
 */
 
#include <iostream>
#include <stdio.h>
#include <algorithm>
#include <map>
#include <vector>
#include <cassert>

using namespace std;

/* Sudoku contains a 9x9 grid */
#define N 9

/* Strategy interface class for different sudoku solving methods */
class SolvingStrategy {
    private:
    
    /* Helper function to remove element in c++ vector */
    void remove_elem(int val, vector<int> &input_vec) {
        input_vec.erase(std::remove(input_vec.begin(), input_vec.end(), val), input_vec.end());
    }
    
    /* Helper function to remove num from the given grid row */
    void elim_grid_row(int num, vector<vector<vector<int>>> &grid, int row) {
           for (int i = 0; i < N; i++) {
                if (grid[row][i].size() > 1) {
                    //remove num from this vector
                    remove_elem(num, grid[row][i]);
                    
                }
            }
    }
    
    /* Helper function to remove num from the given grid column */
    void elim_grid_col(int num, vector<vector<vector<int>>> &grid, int col) {
           for (int i = 0; i < N; i++) {
               if (grid[i][col].size() > 1) {
                    //remove num from this vector
                    remove_elem(num, grid[i][col]);
                }
            }
    }
    
    /* Helper function to remove num from the given grid block */
    void elim_grid_blk(int num, vector<vector<vector<int>>> &grid, int blk) {
       for (int i = 0; i < 3; i++) { 
           for (int j = 0; j < 3; j++) {
               int blk_row = 3*(blk / 3) + i;
               int blk_col = 3*(blk % 3) + j;
              if (grid[blk_row][blk_col].size() > 1) {
                    //remove num from this vector
                    remove_elem(num, grid[blk_row][blk_col]);
                }
            }
        }
    }
    
    public:
    
    /* Function to eliminate numbers from possibility_grid based on sudoku rules */
    void eliminate_elem(vector<vector<vector<int>>> &possibility_grid) {
        /* iterate over the full possibility_grid */
        for (int row = 0; row < N; row++) {
           for (int col = 0; col < N; col++) {
               
               /* 
                * Vector size of 1 means only this number possible at this grid location. 
                * According to sudoku rules there can be no duplicate number on row, 
                * column or block level. Eliminate this number from row, col, block.
                */
                if (possibility_grid[row][col].size() == 1) {
                    elim_grid_row(possibility_grid[row][col][0], possibility_grid, row);
                    elim_grid_col(possibility_grid[row][col][0], possibility_grid, col);
                    
                    int blk = (3*(row/3) + (col/3));
                    elim_grid_blk(possibility_grid[row][col][0], possibility_grid, blk);
                }
            }
        }
    }

    /* Function to create a possibility_grid */
    void form_possibility_grid(int input_grid[N][N], vector<vector<vector<int>>> &possibility_grid) {
        /* Initialize possibility_grid with digits 1-9 on all grid locations */
        vector< int > def_vec = {1,2,3,4,5,6,7,8,9};
        possibility_grid.resize(N, vector<vector<int>>(N, def_vec));
    
        for (int row = 0; row < N ; row++) {
            for (int col = 0; col < N ; col++) {
                /* 
                 * If a input grid location is 0 then its an unfilled location 
                 * that sudoku solver should solve. If an input grid location has 
                 * a value then update the possibility_grid with this value since 
                 * this is the only possible value at this location.
                 */
                if (input_grid[row][col] != 0) {
                        possibility_grid[row][col].clear();
                        possibility_grid[row][col].push_back(input_grid[row][col]);
                }
            }
        }
    }
    
    /* Function to print a possibility_grid */
    void print_possibility_grid(vector<vector<vector<int>>> &possibility_grid) {
        for (int row = 0; row < N; row++) {
            for (int col = 0; col < N; col++) {
                for (int k = 0; k < possibility_grid[row][col].size(); k++) {
                    cout << possibility_grid[row][col][k];
                }
                cout << " ";
            }
            cout << endl;
        }
    }
    
     
    /* Function to copy the possibility_grid with solved grid location to a grid */
    void copy_possibility_grid_output(int output_grid[N][N], vector<vector<vector<int>>> &possibility_grid) {
        for (int row = 0; row < N; row++) {
            for (int col = 0; col < N; col++) {
                /* copy only solved vectors by Elimination */
                if (possibility_grid[row][col].size() == 1) {
                    output_grid[row][col] = possibility_grid[row][col][0];
                }
            }
        }
    }
    
    /* Function to check if possibility_grid contains solved solution */
    bool is_possibility_grid_solved(vector<vector<vector<int>>> &possibility_grid) {
        for (int row = 0; row < N; row++) {
            for (int col = 0; col < N; col++) {
                for (int k = 0; k < possibility_grid[row][col].size(); k++) {
                    if (possibility_grid[row][col].size() > 1) {
                        /* grid is not solved if a location contains more than 1 possible value */
                        return false;
                    }
                }
            }
        }
        return true;
    }
    
    /* Abstract function that should be implemented by the deriving classes */
    virtual bool solve(int input_grid[N][N], int output_grid[N][N]) = 0;
};

/* 
 * Class implementing the backtracking method of solving sudoku.
 * Backtracking method can solve any diffulty level of sudkou puzzle.
 */
class BacktrackingStrategy : public SolvingStrategy {
    private:
    
    /* Function to check if number can be placed in the given row */
    bool check_row(int grid[N][N], int row, int num)  
    {  
        for (int col = 0; col < N; col++)  
            if (grid[row][col] == num)  
                return false;  
        return true;  
    }  
    
    /* Function to check if number can be placed in the given column */
    bool check_col(int grid[N][N], int col, int num)  
    {  
        for (int row = 0; row < N; row++)  
            if (grid[row][col] == num)  
                return false;  
        return true;  
    }  
    
    /* Function to check if number can be placed in the given block */
    bool check_blk(int grid[N][N], int boxStartRow, int boxStartCol, int num)  
    {  
        for (int row = 0; row < 3; row++)  
            for (int col = 0; col < 3; col++)  
                if (grid[row + boxStartRow] 
                        [col + boxStartCol] == num)  
                    return false;  
        return true;  
    }  
      
    /* Function to check if number can be placed in the given grid location */
    bool is_num_safe(int grid[N][N], int row, int col, int num)  
    {  
        /* Check if 'num' is not already placed in  
        current row, current column and current 3x3 box */
        return check_row(grid, row, num) &&  
               check_col(grid, col, num) &&  
               check_blk(grid, row - row % 3 ,  
                          col - col % 3, num) &&  
                grid[row][col] == 0;  
    }  
    
    /* Function to get next unfilled sudoku grid location */
    bool get_unsolved_location(int grid[N][N], int &row, int &col)  
    {  
        for (row = 0; row < N; row++)  
            for (col = 0; col < N; col++)  
                /* If location has value 0 then its unfilled */
                if (grid[row][col] == 0)  
                    return true;
        /* If all locations are filled then return false */
        return false;  
    } 
    
    /* Function containing the core backtracking logic */
    bool backtrack(int output_grid[N][N], vector<vector<vector <int>>> &possibility_grid)  
    {  
        int row, col;  
      
        if (!get_unsolved_location(output_grid, row, col)) {
            return true; 
        }
        
        assert(possibility_grid[row][col].size() > 1);
        
        for (int i = 0; i < possibility_grid[row][col].size(); i++)
        {  
            int num = possibility_grid[row][col][i];
            
            if (is_num_safe(output_grid, row, col, num))  
            {  
                /* make tentative assignment */
                output_grid[row][col] = num;  
      
                /* return, if success */
                if (backtrack(output_grid, possibility_grid))  
                    return true;  
      
                /* failure, unassign to try again */
                output_grid[row][col] = 0;  
            }  
        }  
        
        /* trigger backtracking */
        return false; 
    } 
    
    /* Function preparing the inputs for backtracking logic */
    bool sudoku_backtrack(int input_grid[N][N], int output_grid[N][N]) {
        
        vector<vector<vector <int>>> possibility_grid;
        
        form_possibility_grid(input_grid, possibility_grid);
        
        eliminate_elem(possibility_grid);
        
    	copy_possibility_grid_output(output_grid, possibility_grid);
    
        return backtrack(output_grid, possibility_grid);
    }
    
    /* Implementation of solving strategy interface function */
    bool solve(int input_grid[N][N], int output_grid[N][N]) {
        cout << "Backtracking Method" << endl;
        cout<<"---------------------" << endl;
        return sudoku_backtrack(input_grid, output_grid);
    }
};

/* 
 * Class implementing the naive method of solving sudoku.
 * Naive or brute force method can only solve any easy/medium level
 * of sudkou puzzle
 */
class NaiveStrategy : public SolvingStrategy {
    private: 
    /* Function containing the core naive logic */
    void naive(vector<vector<vector<int>>> &possibility_grid) {
        eliminate_elem(possibility_grid);
        
        if (is_possibility_grid_solved(possibility_grid) == true)  {
             return;
        } else {
            naive(possibility_grid);
        }
    }
    
    /* Function preparing the inputs for naive logic */
    bool sudoku_naive(int input_grid[N][N], int output_grid[N][N]) {
        vector<vector<vector <int>>> possibility_grid;
        form_possibility_grid(input_grid, possibility_grid);
        
    	naive(possibility_grid);
    	
    	copy_possibility_grid_output(output_grid, possibility_grid);
    
    	return true;
    }
    
    /* Implementation of solving strategy interface function */
    bool solve(int input_grid[N][N], int output_grid[N][N]) {
        cout << "Naive method" << endl;
        cout<<"---------------" << endl;
        return sudoku_naive(input_grid, output_grid);;
    }
};

/* Strategy interface class for getting sudoku input grid in different ways */
class InputStrategy {
    public:
    /* Abstract function that should be implemented by the deriving classes */
    virtual void get_input(int input_grid[N][N]) = 0;
};

class RawInputStrategy : public InputStrategy {
    /* Implementation of input strategy interface function */
    void get_input(int input_grid[N][N]) {
        /* TODO: get the raw input from user using cin */
        cout << "Raw input method" << endl;
        return;
    }
};

class ImageInputStrategy : public InputStrategy {
    /* Implementation of input strategy interface function */
    void get_input(int input_grid[N][N]) {
        /* TODO: get the input from sudoku puzzle image using image recognition */
        cout << "Image input method" << endl;
        return;
    }
};

/* 
 * Sudoku solver Class definition utilizing Strategy design pattern for 
 * varying the different ways of solving methods and getting the input grid.
 */
class SudokuSolver {
    private:
    /* variable to hold different strategies under this class */
    SolvingStrategy *solvingStrategy = NULL;
    InputStrategy *inputStrategy = NULL;
    
    public:
    /* Function to set a required strategy for solving sudoku */
    void set_solving_strategy(SolvingStrategy *strategy) {
        this->solvingStrategy = strategy;
    }
    
    /* Function to solve the sudoku puzzle using the requested solving strategy */
    bool do_solve(int input_grid[N][N], int output_grid[N][N]) {
        cout << "inside do solve" << endl;
        if (solvingStrategy == NULL) {
            cout << "choose a valid solving strategy" << endl;
            return false;
        }
        return solvingStrategy->solve(input_grid, output_grid);
    }
    
    /* Function to set a required strategy for getting sudoku input grid */
    void set_input_strategy(InputStrategy *strategy) {
        this->inputStrategy = strategy;
    }
    
    /* Function to get the sudoku puzzle grid using the requested input strategy */
    void do_get_input(int input_grid[N][N]) {
        cout << "inside do get input" << endl;
        if (inputStrategy == NULL) {
            cout << "choose a valid input strategy" << endl;
            return;
        }
        inputStrategy->get_input(input_grid);
        return;
    }
    
    /* print the sudoku grid */
    void print_grid(int grid[N][N]) {
        for (int row = 0; row < N; row++) {
            for (int col = 0; col < N; col++)  {
                cout << grid[row][col] << " ";  
                if (col % 3 == 2) {
                    cout << " | ";
                }
            }
            if (row % 3 == 2) {
                cout << "\n--------------------------";
            }
            cout << endl; 
        }
    }
};


int main()
{
    cout<<"Sudoku Solver Demo" << endl;
    cout<<"---------------------" << endl;
    
    /* Define the sudoku solver class */
    SudokuSolver solver;

    /* Allocate different available sudoku solving strategies */
    BacktrackingStrategy *sol_st_1 = new BacktrackingStrategy;
    NaiveStrategy *sol_st_2 = new NaiveStrategy;
    
    /* Allocate different available sudoku input strategies */
    RawInputStrategy *inp_st_1 = new RawInputStrategy;
    ImageInputStrategy *inp_st_2 = new ImageInputStrategy;
    
    /* define a sample sudoku input */
    int inp[N][N] = {
        {0, 2, 0, 9, 4, 5, 0, 7, 8},
        {0, 0, 9, 0, 6, 2, 0, 0, 0},
        {0, 7, 0, 1, 0, 0, 2, 0, 5},
        {0, 1, 0, 8, 0, 9, 4, 0, 0},
        {0, 0, 0, 0, 5, 0, 0, 0, 0},
        {0, 0, 5, 4, 0, 1, 0, 6, 0},
        {1, 0, 7, 0, 0, 8, 0, 2, 0},
        {0, 0, 0, 3, 2, 0, 1, 0, 0},
        {8, 6, 0, 5, 1, 4, 0, 3, 0}
    };

    /* define output grids to hold the sudoku solutions */
    int out_1[N][N] = { 0 };
    int out_2[N][N] = { 0 };
    
    /* set the backtracking strategy and raw input strategy on sudoku solver class */ 
    solver.set_solving_strategy(sol_st_1);
    solver.set_input_strategy(inp_st_1);
    
    /* solve the input using requested strategies */
    solver.do_get_input(inp); // currently this is dummy
    solver.do_solve(inp, out_1);
    solver.print_grid(out_1);

    /* set the naive strategy and image input strategy on sudoku solver class */
    solver.set_solving_strategy(sol_st_2);
    solver.set_input_strategy(inp_st_2);
    
    /* solve the input using requested strategies */
    solver.do_get_input(inp); // currently this is dummy
    solver.do_solve(inp, out_2);
    solver.print_grid(out_2);

    return 0;
}

