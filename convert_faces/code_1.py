path1 = input("type the obj with more than 3 verteces in a face:")
path2 = "n.obj"

def revert_face(c_line):
    elements = c_line.split('/')
    c_line = ""
    k=0
    for e in elements:
        e = int(e)
        if k<2:
            if e<0: 
                e = -e
                c_line += str(e) 
                c_line += "/"
            else:
                c_line += str(e) 
                c_line += "/"
            k+=1
        else:
            if e<0: 
                e = -e
                c_line += str(e) 
                c_line += " "
            else:
                c_line += str(e) 
                c_line += " "
            k=0
    return c_line
try:
    # Open the file for reading
    with open(path1, 'r') as f1:
        f2 = open(path2,"w")
        for line in f1:
            if line.startswith('f'):
                c_line = line.split(' ')
                if (c_line[-1]=='\n'):
                    c_line.pop()
                s_vertex = len(c_line)
                if (s_vertex>4):
                    for i in range(s_vertex-3):
                        sentence = "f "+c_line[1]+" "+c_line[i+2]+" "+c_line[i+3]+"\n"
                        f2.write(sentence)    
                else: f2.write(line)
            else: f2.write(line) 
            
except FileNotFoundError:
  # The file does not exist
  print('Error: The file does not exist')
except:
  # Any other exception
  print('Error: An unknown error occurred while trying to access the file')

                    
        