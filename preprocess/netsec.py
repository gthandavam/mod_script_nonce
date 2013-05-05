import os
import re
import sys

try:
    readfile = sys.argv[1]
    ff = open(readfile,'r')
    s=ff.readline()

    if not os.path.exists("newfiles"):
        os.makedirs("newfiles")

except IOError:
    print "ERROR encountered while reading input file!!"
    quit()

    
while s:
    fname=s.strip()
    s=ff.readline()
    try:
        f = open(fname)
        path =os.path.join("newfiles",fname)
        fnew = open(path,'w')
    except IOError:
        print "ERROR encountered while reading input file!!"
        quit()
    
    line = f.readline()
    lineno=0
    while line:
        indxlist=[]
        lineno+=1
        for m in re.finditer( '<(s|S)(c|C)(r|R)(i|I)(p|P)(t|T)', line ):
            indxlist.append(m.end()-1)
        indxlist.reverse()
        if len(indxlist)>0:
            buff=""
            insrt=indxlist.pop()
            for i in range(len(line)):
                buff+=line[i]
                if i == insrt:
                    buff+=" nonce='xxxxxxxxxxxxxxxxx'"
                    if len(indxlist)>0:
                        insrt=indxlist.pop()
            fnew.write(buff)
        else:
            fnew.write(line)
        line = f.readline()


    f.close()
    fnew.close()
print "Done!"
ff.close()

