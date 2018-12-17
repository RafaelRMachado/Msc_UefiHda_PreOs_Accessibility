## @file
#  
#   Copyright (c) 2018, Rafael Rodrigues Machado. All rights reserved.<BR>
#   This program and the accompanying materials
#   are licensed and made available under the terms and conditions of the BSD License
#   which accompanies this distribution. The full text of the license may be found at
#   http://opensource.org/licenses/bsd-license.
#
#   THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
#   WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
#
##

#!/usr/bin/env python

#This file is used to generate a .h based on a binary file's content
#so it can be accessed as a simple array

#Import the necessary libs
import sys, os.path
import binascii

#loop the files to be processed
for filename in sys.argv[1:]:
    
    #get the file size so the array can be created with the 
    #correct number of elements
    filesize = os.path.getsize(filename)
    
    #open the file in binary mode
    origfile =  open(filename, "rb")
    
    FileNameToUse = filename.replace(".", "_")
    FileNameToUse = FileNameToUse.replace("/", "__")
    
    #creates a string with the file name
    output = "const CHAR16* " + FileNameToUse + " = L" + '"' + FileNameToUse + '"' + ";\n"
    output += "const UINTN " + FileNameToUse + "_Size = %d;\n" % (filesize)
    
    
    #Creates the array with the correct name and size
    output += "static UINT8 SoundData" + FileNameToUse + "[%d] = {\n" % (filesize)
    
    #add each byte from the file to an string that will be 
    #used later to create the final file
    for i in range(0, filesize):
        stringValue = binascii.hexlify(origfile.read(1))
        output = output + " 0x%s," % stringValue
        if (i % 12) == 11:
            output = output + "\n"
    output = output + "\n};\n"
    
    #create the .h file with the binary file's content on 
    #an integer array
    f = file("%s.h" % filename , "w")
    f.write(output)
    f.close()

print "Done!"