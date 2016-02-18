Initially, the winscard.cpp file was generated using the wrappit.exe tool found in
this project. See  http://www.codeproject.com/KB/DLL/CreateYourProxyDLLs.aspx

However, for 64 bit (Windows), the 'naked' keyword no longer works; so we
make a tool (http://10.2.250.30/svn/eidmw/trunk/misc/makeproxy) to automatically
generate the proxy code in the windows and linux subdirs.

This code has been modified to link it to the code in this dir that takes care
of the virtual readers/cards.

