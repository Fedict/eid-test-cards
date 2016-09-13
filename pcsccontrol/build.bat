set JAVA_HOME=C:\Program Files\Java\jdk1.8.0_74

call ..\..\ThirdParty\ant\apache-ant-1.9.6\bin\ant -f build.xml clean
call ..\..\ThirdParty\ant\apache-ant-1.9.6\bin\ant -f build.xml
call ..\..\ThirdParty\ant\apache-ant-1.9.6\bin\ant -f makeJarFile.xml clean
call ..\..\ThirdParty\ant\apache-ant-1.9.6\bin\ant -f makeJarFile.xml
copy /Y pcsccontrol.jar saved\
