# -*- coding: utf-8 -*-
import re,os,sys,io


doctringregex="(.+)(\"\@DocString)([\s\t]*\()(.+)(\")"
                
def getkey(matches):
    keytosearchfor=(matches.group(4).strip()).rstrip(")")
    if "(" in keytosearchfor:
        keytosearchfor+=")"
    return keytosearchfor


def getelementstocomment(filelist):
    matchlist = []
    for filename in filelist:
        tomatch={}
        with io.open(filename,encoding='windows-1251') as filestream:
            for line in filestream:
                matches=re.match(doctringregex,line)
                if matches:
                    keytosearchfor=getkey(matches)
                    keytoreplace="".join([value for value in  matches.groups()[1:]])
                    tomatch[keytosearchfor]=keytoreplace.strip()
        matchlist.append(tomatch)
    return matchlist

def getcommentselements(headerfiles):
    commentsmatch={}
    for filename in headerfiles:
        with io.open(filename,encoding='windows-1251') as filestream:
            docstring=""
            commentstring=""
            incomment=False
            for line in filestream:
                if not docstring:
                    matches=re.match("([\s\t]*)(// DocString:)(.+)",line)
                    if matches:
                        docstring=matches.group(3).strip()
                elif (docstring and (line.strip().startswith("/") or line.strip().startswith("*/"))):
                    singlecommentmatch=re.match("([\s\t]*)(///)([\s\t]*)(.+)",line)
                    multilinematch=re.match("([\s\t]*)(/\*\*)([\s\t]*)",line)
                    multilinematchclose=re.match("([\s\t]*)(\*/)([\s\t]*)",line)
                    if singlecommentmatch:
                        commentstring+=singlecommentmatch.group(4).strip()+"\n"
                    elif multilinematch:
                        incomment=True
                    elif multilinematchclose:
                        incomment=False
                elif docstring and incomment:
                    commentstring+=line.strip()+"\n"
                elif not incomment and docstring and commentstring:
                    commentsmatch[docstring]='"'+((commentstring.rstrip("\n")).replace('\n',r'\n')).replace('"',"")+'"'
                    docstring=""
                    commentstring=""
                    incomment=False
    return commentsmatch

def buildcommentsreplacement(commentsource,commentstoreplace):
    commentsfound = 0
    totalcomments = 0
    successfullreplacement = []
    for commentsdict in commentstoreplace:
        successfullreplacementdict={}
        for target,replacementkey in commentsdict.items():
            if target in commentsource:
                successfullreplacementdict[replacementkey]=commentsource[target]
                commentsfound+=1
            totalcomments+=1
        successfullreplacement.append(successfullreplacementdict)
    print("Found replacement for "+str(round((float(commentsfound)/float(totalcomments))*100))+" % of targeted objects")
    return successfullreplacement

def generatecomments(targetdirectory,originalfiles,generalmatches):
    for filepath,comments in zip(originalfiles,generalmatches):
        filename=os.path.basename(filepath)
        newfile = io.open(os.path.join(targetdirectory,filename),"w",encoding='windows-1251')
        with io.open(filepath,encoding='windows-1251') as filestream:
            for line in filestream:
                newline=line
                matches=re.match(doctringregex,line)
                if matches:
                    keytoreplace=("".join([value for value in  matches.groups()[1:]])).strip()
                    if keytoreplace in comments:
                        newline=line.replace(keytoreplace,comments[keytoreplace])
                newfile.write(newline)
        newfile.close()

def gatherfiles(directory,source):
    allfiles = []
    for file in os.listdir(directory):
        if (file.endswith(".hpp") and 
            (
                    (not source and (file.startswith("PY") or file.startswith("R"))) or 
                    (source and not (file.startswith("PY") or file.startswith("R")))
                    )
            ):
            allfiles.append(os.path.join(directory,file))
    return allfiles

if __name__=="__main__":
    pathtotarget = sys.argv[1]
    filestomodify = gatherfiles("../Include",False)
    headerfiles = gatherfiles("../Include",True)
    targetdirectory=pathtotarget#"../Source/include"
    os.mkdir(targetdirectory)
    commentsource=getcommentselements(headerfiles)
    commentstoreplace=getelementstocomment(filestomodify)
    replaced=buildcommentsreplacement(commentsource,commentstoreplace)
    generatecomments(targetdirectory,filestomodify,replaced)
 
    
