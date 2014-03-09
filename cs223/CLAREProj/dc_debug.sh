#! /bin/sh

#kill rmi processes
kill -9 `ps -ef | grep rmiregistry | awk '{print $2}'`
kill -9 `ps -ef | grep rmi\.server | awk '{print $2}'`

# Compile the source files and put the class files in destDir
#compile1
#javac -cp ".:./lib/*:" -d . ./src/ics/uci/edu/table/*.java
javac -cp ".:./lib/*:./ics/uci/edu/table:./ics/uci/edu/rmi:./ics/uci/edu/logmanager:./ics/uci/edu/lockmanager:./ics/uci/edu/resourcemanager:./lib/httpcore-4.0.1.jar:" -d . ./src/ics/uci/edu/logmanager/*.java
javac -cp ".:./lib/*:./ics/uci/edu/table:./ics/uci/edu/rmi:./ics/uci/edu/logmanager:./ics/uci/edu/lockmanager:./ics/uci/edu/resourcemanager:./lib/httpcore-4.0.1.jar:" -d . ./src/ics/uci/edu/lockmanager/*.java
javac -cp ".:./lib/*:./ics/uci/edu/table:./ics/uci/edu/rmi:./ics/uci/edu/logmanager:./ics/uci/edu/lockmanager:./ics/uci/edu/resourcemanager:./lib/httpcore-4.0.1.jar:" -d . ./src/ics/uci/edu/table/*.java

#compile2
javac -cp ".:./lib/*:./ics/uci/edu/table:./ics/uci/edu/rmi:./ics/uci/edu/logmanager:./ics/uci/edu/lockmanager:./ics/uci/edu/resourcemanager:./lib/httpcore-4.0.1.jar:" -d . ./src/ics/uci/edu/rmi/*.java

# Start the Java RMI registry(1100 - DC)
rmiregistry 1100 &

# Start the server(DC)
java -Xdebug -Xnoagent -Xrunjdwp:transport=dt_socket,server=y,address=8000,suspend=y -cp ".:./lib/*:./ics/uci/edu/table:./ics/uci/edu/rmi:./lib/httpcore-4.0.1.jar:" -Djava.rmi.server.codebase=file:./ ics.uci.edu.rmi.DCServer &
