#! /bin/sh
#compile2
javac -cp ".:./lib/*:./ics/uci/edu/table:./ics/uci/edu/rmi:./ics/uci/edu/logmanager:./ics/uci/edu/lockmanager:./ics/uci/edu/resourcemanager:./lib/httpcore-4.0.1.jar:" -d . ./src/ics/uci/edu/rmi/*.java

#execute Client
java -cp ".:./lib/*:./ics/uci/edu/table:./ics/uci/edu/rmi:./lib/httpcore-4.0.1.jar:" ics.uci.edu.rmi.Client
