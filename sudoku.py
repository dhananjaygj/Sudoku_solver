import os
import re
import sets
from itertools import chain
import numpy as np


#function to solve 9X9 sudoku
def sudoku():
    print "Enter numbers in your (9X9) sudoku puzzle without spaces"
    print "Note: 9 numbers per row & 9 such rows(0 for missing numbers)"

    puzzle = []
    i = 0

    
    while (i != 9):
        inp = ( raw_input())
        puzzle.append(map(int,str(inp)))
        i = i + 1
    
    """
    #sample input for debugging    
    puzzle = [[0,2,0,9,4,5,0,7,8],
              [0,0,9,0,6,2,0,0,0],
              [0,7,0,1,0,0,2,0,5],
              [0,1,0,8,0,9,4,0,0],
              [0,0,0,0,5,0,0,0,0],
              [0,0,5,4,0,1,0,6,0],
              [1,0,7,0,0,8,0,2,0],
              [0,0,0,3,2,0,1,0,0],
              [8,6,0,5,1,4,0,3,0]]
    """
    
    #calling actual solver function first time
    done = sud(puzzle)

    #loop until solved
    while( False in done.keys() ):
        #print done[False]
        done = sud(done[False])

    #print solved result
    #print done[True]
    print "\nSolved result is \n"
    print np.matrix(done[True])



#actual sudoku solving function
def sud(puzzle):

    missingrow = []
    missingcol = []
    missingbox = []
    solution = []
    solution1 = []
    finished = True

    #traverse through the array and get the missing numbers by row,column,box wise
    for i in range(9):
        missingrow.append(checkrow(i,puzzle))
        missingcol.append(checkcol(i,puzzle))
        missingbox.append(checkbox(i,puzzle))

        
    #print missingrow
    #print '\n'
    #print missingcol
    #print '\n'
    #print missingbox
    #print '\n'

    #iterate through each cell of the 9X9 array and
    #solve using set intersection numbers posible on that cell
    for i in range(9):
        for j in range(9):

            if( puzzle[i][j] == 0):
                finished = False
                mr = set(tuple(missingrow[i]))
                mc = set(tuple(missingcol[j]))
                mb = set(tuple(missingbox[(i/3)*3 + (j/3)]))
                       
                tmp = (list((mr.intersection(mc)).intersection(mb)))
                if len(tmp) == 1:
                    solution.append(tmp[0])
                else:
                    solution.append(0)
            else:
                solution.append((puzzle[i][j]))

    #intermediate or final result
    for i in range(9):
        solution1.append(solution[9*i:9*i+9])

    return {finished:solution1}



#checks the numbers in a row    
def checkrow(r,puzzle):
    missing = []
    for i in range(9):
        if (i+1) not in puzzle[r]:
            missing.append(i+1)

    return missing
        
#checks the numbers in a column 
def checkcol(c,puzzle):
    missing = []
    for i in range(9):
        if (i+1) not in [row[c] for row in puzzle]:
            missing.append(i+1)

    return missing

#checks the numbers in a 3X3 box 
def checkbox(n,puzzle):
    missing = []
    box = []
    for j in range(3):
        box.extend((puzzle[((n/3)*3) + j][(3*(n%3)):((n%3)+1)*3]))
        
    for i in range(9):
            if i+1 not in box:
                missing.append(i+1)
            
    return missing
    
    

#trigger the sudoku function
sudoku()
    
    
