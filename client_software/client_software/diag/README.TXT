TestPcscProxy is a command line Java tool that you could try
if you have some problem with the PCSC proxy. It gives info
about what might be wrong.

Usage: simply run from the command line:

  java -cp . TestPcscProxy


Important note for Windows:
  You probably have to provide the path to the java.exe, e.g.
    "C:\Program Files\Java\jre7\bin\java.exe" -cp . TestPcscProxy
  To find out this path, first run "java -cp . TestPcscProxy" which will tell
  you the path to your Java home dir.
