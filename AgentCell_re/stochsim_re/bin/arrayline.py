#!/usr/bin/python

import sys, glob, operator, string, array

from string import ljust

sys.argv = reduce(operator.add, map(glob.glob, sys.argv))

def remove_non_digit_from_string(x):
    str=''
    for c in x:
        if c.isdigit():
          str=str+c
    return str

def remove_non_0f_from_string_and_transform_into_bytes(x):
    str=''
    for c in x:
        if c == '0':
            str=str+'\000'
        elif c == 'f':
            str=str+'\001'
    return str


for file in sys.argv[1:]:
    lines = open(file).readlines()
    
    #finding line containing dimensions
    for i in range(len(lines)):           
        if lines[i].find('static char') != -1:
            break
    i+=1

    #getting dimensions
    words = string.split(lines[i])
    nx = remove_non_digit_from_string(words[0])
    ny = remove_non_digit_from_string(words[1])
    if ny != '1':
        print "You must run stochsim with an Line array that has ny=1 and nx>1"
    
    #remove all the lines that have a length smaller than max(nx,ny)
    #and convert those to a a line of bytes eliminating everything that
    #is not a 0 or a f
    dat=''
    for line in lines[i:]:
        if len(line) >= max(int(nx),int(ny)):
            dat=dat + remove_non_0f_from_string_and_transform_into_bytes(line)

    #write out in file
    #length of header in bytes
    headerlength=128
    fout = open(file+'.dat','wb')
    #writing header and data
    fout.write(ljust(nx+' '+str(len(dat)/int(nx)),headerlength))
    fout.write(dat)
    fout.close()
