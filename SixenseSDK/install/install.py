#!/usr/bin/python

import sys
import os.path
from optparse import OptionParser
from shutil import rmtree, copy, copytree
import subprocess

cmdOpts = ''
cmdArgs = ''
config = ''

menu = """
Please select your operating system:
1. Windows 32 bit
2. Windows 64 bit
3. Linux 32 bit
4. Linux 65 bit
5. OSX 32 bit
6. OSX 64 bit
"""

def isInt( s ):
   try:
      int(s)
      return True
   except ValueError:
      return False


class SDKPackager:
   def __init__( self ):
      self.versionMajor = 1
      self.versionMinor = 0
      self.versionMacro = 1
      
      self.currentDirectory = os.path.dirname(os.path.realpath(__file__))
      self.platform = ""
      
      self.linux_32 = []
      self.linux_64 = []
      self.osx = []
      self.osx64 = []
      
      print "\nSixense SDK Installation Script v%02d.%02d.%02d" % ( self.versionMajor, self.versionMinor, self.versionMacro )
      if( not cmdOpts.printVersionAndExit ):
         print "============================================"

   def cleanString( self, string ):
      string = string.lstrip()
      string = string.rstrip(" \t\r\n\0")
      return string

   def cleanList( self, lst):
      if "" in lst:
         lst.remove("")
      return lst
   
   def forceString( self, value ):
      if not isinstance(value, str):
         return value[0]
      else:
         return value
      
   def getValue( self, key ):
      returnValue = []

      file = open(cmdOpts.config)
      for line in file:
         line = self.cleanString(line)
         if len(line) == 0:
            continue
            
         if line[0] == '#':
            continue

         pairs = line.split("=")
      
         keyFile = self.cleanString(pairs[0])
         if keyFile == key:
            for element in pairs[1].split(","):
               returnValue.append(self.cleanString(element))
            return returnValue

      return returnValue
      
   def find_in_file(self, filename, searchstr):    # see whether a string appears in a file at all, and tell on which line
      libfd = open(filename,"r")
      contents = libfd.readlines()
      libfd.close()
      index = 0
      for fline in contents:
         if searchstr in fline:
            return True, index
         index = index + 1
      return False, index
   
   def append_to_file(self, filename, newstr):
      libfd = open(filename,"a")
      libfd.write(newstr)
      libfd.close()
             
   def parseConfig( self ):
   
      print "-----------------------------------------------"
      print "Parsing Config File %s" % cmdOpts.config
      print "-----------------------------------------------"
      
      self.parseItem()

   def parseItem( self ):
      print "Finding Items"
      file = open(cmdOpts.config)
      
      for line in file:
         line = self.cleanString(line)

         if len(line) == 0:
            continue
            
         if line[0] == '#':
            continue

         pairs = line.split("=")
      
         key = self.cleanString(pairs[0])
         value = []
      
         if key == "linux_32" and self.platform == "linux_32":
            sys.stdout.write("Found Linux 32 bit")
            sys.stdout.flush()
            for element in pairs[1].split(","):
               if len(element) > 0:
                  self.linux_32.append(self.cleanString(element))
            print "                              Done"
         if key == "linux_64" and self.platform == "linux_64":
            sys.stdout.write("Found Linux 64 bit")
            sys.stdout.flush()
            for element in pairs[1].split(","):
               if len(element) > 0:
                  self.linux_64.append(self.cleanString(element))
            print "                             Done"
         if key == "osx_32" and self.platform == "osx_32":
            sys.stdout.write("Found OSX 32 bit")
            sys.stdout.flush()
            for element in pairs[1].split(","):
               if len(element) > 0:
                  self.osx.append(self.cleanString(element))
            print "                                 Done"
         if key == "osx_64" and self.platform == "osx_64":
            sys.stdout.write("Found OSX 64 bit")
            sys.stdout.flush()
            for element in pairs[1].split(","):
               if len(element) > 0:
                  self.osx64.append(self.cleanString(element))
            print "                                 Done"
               
      file.close()
      print "Done\n"

   def chooseTargetPlatform( self ):
      response = ""
      print menu
 
      response = raw_input("Enter Selection: ")

      if response == "3":
         self.platform = "linux_32"
      elif response == "4":
         self.platform = "linux_64"
      elif response == "5":
         self.platform = "osx_32"
      elif response == "6":
         self.platform = "osx_64"
      else:
      	 if response == "1" or response == "2":
	    print "Currently Unsupported Target Operating System"
	 else:
            print "Invalid Selection"
         self.chooseTargetPlatform()
      
   def preInstall( self ):
      return
      
   def install( self ):
      self.copyFilesHelper( self.linux_32, "Linux 32 bit" )
      self.copyFilesHelper( self.linux_64, "Linux 64 bit" )
      self.copyFilesHelper( self.osx, "OSX 32 bit" )
      self.copyFilesHelper( self.osx64, "OSX 64 bit" )
      return

   def copyFilesHelper( self, folderList, userText ):
      if not cmdOpts.verbose: 
         self.firstErrorAfterHeader = True
      destination = ""
      source = ""
      files = []
      if len(folderList) > 0:
         sys.stdout.write("- for %s\r" % userText)   
         sys.stdout.flush()
         if cmdOpts.verbose:   
            print ""
         for element in folderList:
            destination = self.getValue(element+"_destination")
            source = self.getValue(element+"_source")
            files = self.getValue(element+"_file")
            source = self.cleanList(source)
            files = self.cleanList(files)
            for outfile in files:
               self.copyFileParser( destination, source, outfile )
         if not cmdOpts.verbose:
            sys.stdout.write("%46s\r" % "Done")
            sys.stdout.write("- for %s\n" % userText) 
            sys.stdout.flush()
         else:
            print "Done\n"

   def copyFileParser( self, dstPath, srcPath, srcFile ):
      dst = ""
      src = ""
      if not isinstance(dstPath, str):
         dst = dstPath[0]
      else:
         dst =  dstPath  
      if not isinstance(srcPath, str):
         src = srcPath[0]
      else:
         src =  srcPath

      pathsExist = True
      
      if not os.path.isdir(src):
         if self.firstErrorAfterHeader:
            print ""
            self.firstErrorAfterHeader = False
         print "Source Path Does Not Exist: %s" % src
         pathsExist = False
      if not os.path.isdir(dst):
         if self.firstErrorAfterHeader:
            print ""
            self.firstErrorAfterHeader = False
         print "Destination Path Does Not Exist: %s" % dst
         pathsExist = False

      if not pathsExist:
         return
         
      #copy all files
      if srcFile.split('.')[0] == '*' and srcFile.split('.')[1] == '*':
         for filename in os.listdir( os.path.join(".",src) ):
            self.copyFile( dst, src, filename )
      #copy all files by extention
      elif srcFile.split('.')[0] == '*' and srcFile.split('.')[1] != '*':
         for filename in os.listdir( os.path.join(".",src) ):
            if os.path.isfile(filename):
               if filename.split('.')[1] == srcFile.split('.')[1]:
                  self.copyFile( dst, src, filename )
      #copy all files starting with <>
      elif srcFile.split('.')[0] != '*' and srcFile.split('.')[1] == '*':
         for filename in os.listdir( os.path.join(".",src) ):
            if filename.split('.')[0] == srcFile.split('.')[0]:
               self.copyFile( dst, src, filename )
      #copy individual file
      else:
         self.copyFile( dst, src, srcFile )
   
   def copyFile( self, dstPath, srcPath, srcFile ):
      fileExists = True
         
      if not os.path.isfile(os.path.join(srcPath,srcFile)):
         if os.path.isdir(os.path.join(srcPath,srcFile)):
            if cmdOpts.verbose:
               print "Copying all files from %s to %s" % (os.path.join(srcPath,srcFile), dstPath )
            copytree(srcPath,os.path.join(dstPath,srcFile))
            return
         else:
            if self.firstErrorAfterHeader:
               print ""
               self.firstErrorAfterHeader = False
            print "Source File Does Not Exist: %s" % os.path.join(srcPath,srcFile)
            fileExists = False

      if not fileExists:
         return
         
      if cmdOpts.verbose:
         print "Copying File %s from %s to %s" % (srcFile, srcPath, dstPath )
      copy( os.path.join(srcPath,srcFile), dstPath)
      
   def postInstall( self ):
      if self.platform =="linux_32" or self.platform == "linux_64":
         configFile = self.forceString(self.getValue("linux_library_config_file"))
         libPath = self.forceString(self.getValue("linux_library_path"))
         if os.path.isfile( configFile ):
            found, index = self.find_in_file(configFile,libPath)    # is lib path already there?
            if found:
               print "Library path is already registered in %s, on line %d." % (configFile,index)
            else:
               print "Library path not registered yet. Adding library path to %s..." % configFile
               self.append_to_file(configFile,"\n"+libPath+"\n")
            lib_update_cmd = "ldconfig"
            p = subprocess.Popen(lib_update_cmd, shell=True, stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.PIPE, close_fds=True)
         else:
            print "Unable to Find ld.so.conf"
      
   def run( self ):
      self.chooseTargetPlatform()
      self.parseConfig()
      self.preInstall()
      self.install()
      self.postInstall()
      
# main program starts here
if __name__ == '__main__':
   #configFile = "sdk_hierarchy_default"
   
   parser = OptionParser(usage="%prog [options]")

   parser.add_option('',"--version",
                     action="store_true",
                     dest="printVersionAndExit",
                     default=False,
                     help="Prints the version and exits",)
                     
   parser.add_option('-c',"--config",
                     action="store",
                     dest="config",
                     default="install.cfg",
                     help="Config File to use",)

   parser.add_option('-v',"--verbose",
                     action="store_true",
                     dest="verbose",
                     default=False,
                     help="Print Extra Information",)

   parser.add_option('-w',"--warning",
                     action="store_true",
                     dest="warning",
                     default=False,
                     help="Print Only Warning Information",)
                                          
   (cmdOpts, cmdArgs) = parser.parse_args()
   
   package = SDKPackager()
   
   if( cmdOpts.printVersionAndExit ):
      exit()
   package.run()
