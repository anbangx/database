kill -9 `ps -ef | grep rmiregistry | awk '{print $2}'`
kill -9 `ps -ef | grep rmi\.server | awk '{print $2}'`

