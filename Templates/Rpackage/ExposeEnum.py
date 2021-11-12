# -*- coding: utf-8 -*-
import re,os,sys

def createzzz(enumlist,outpath):
    with open(os.path.join(outpath,"zzz.R"),"w") as openfile:
        openfile.write("loadModule(\"FMT\", TRUE)\n\n")
        for enum in enumlist : 
            openfile.write(enum[0]+"=list("+enum[1]+")\n")
        openfile.write("\n")
        openfile.write(".onLoad <- function(lib, pkg)\n{\n\n}\n\n.onUnLoad <- function(lib)\n{\n\n}\n")

def findenum(directory):
    stripcomments = '/\*[^\*]*\*+(?:[^/*][^*]*\*+)*/|//[^\n]+|"(?:\\.|[^\\"])*"|\'(?:\\.|[^\\\'])*\''
    catchenum = "^\s*(enum(.|\n)*?;)"
    enums = [] #List to store enum definitions
    for file in os.listdir(directory):
        if (file.endswith(".hpp") and not (file.startswith("PY") or file.startswith("R"))):
            filetext = open(os.path.join(directory,file), "r").read()
            filetext = re.sub(stripcomments,"", filetext)
            for r in re.findall(catchenum,filetext,re.M):
                s = re.search("enum\s*(\w+)((.|\n)*?)({)((.|\n)*?)(?=})",r[0].replace("class",""))
                name = re.sub("\s","",s.group(1))
                values = re.sub("\s","",s.group(5))
                enums.append((name,values))
    return enums 

if __name__=="__main__":
    pathtotarget = sys.argv[1]
    enums = findenum("../../Source")
    createzzz(enums,pathtotarget)
    