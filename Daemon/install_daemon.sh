#!/bin/bash
# Local Operations Menu
################################################################################
base_folder="/home/pi/scripts/lora"

# Have to create the logs directory or else the scripts will complain
mkdir -p "${base_folder}/logs"
mkdir -p "${base_folder}/pids"

# Install the Python Daemon Library
sudo apt-get update
sudo apt-get -y install python-daemon python-serial
#sudo apt-get install python-pip
#sudo pip install python-daemon

# Create the LoRa Daemon Configuration File
echo '#!/bin/sh

### BEGIN INIT INFO
# Provides: myservice
# Required-Start: $remote_fs $syslog
# Required-Stop: $remote_fs $syslog
# Default-Start: 2 3 4 5
# Default-Stop: 0 1 6
# Short-Description: Remote Control daemon
# Description: Periodically checks for remote commands via a web service callr
### END INIT INFO

# Change the next 3 lines to suit where you install your script and what you want to call it
DIR='"${base_folder}"'
DAEMON=$DIR/lora_daemon.py
DAEMON_NAME=lora_daemon

# This next line determines what user the script runs as.
# Root generally not recommended but necessary if you are using the Raspberry Pi GPIO from Python.
DAEMON_USER=root

# The process ID of the script when it runs is stored here:
PIDFILE='"${base_folder}"'/pids/$DAEMON_NAME.pid

. /lib/lsb/init-functions

do_start () {
log_daemon_msg "Starting system $DAEMON_NAME daemon"
start-stop-daemon --start --background --pidfile $PIDFILE --make-pidfile --user $DAEMON_USER --startas $DAEMON
log_end_msg $?
}
do_stop () {
log_daemon_msg "Stopping system $DAEMON_NAME daemon"
start-stop-daemon --stop --pidfile $PIDFILE --retry 10
log_end_msg $?
}

case "$1" in

start|stop)
do_${1}
;;

restart|reload|force-reload)
do_stop
do_start
;;

status)
status_of_proc "$DAEMON_NAME" "$DAEMON" && exit 0 || exit $?
;;
*)
echo "Usage: /etc/init.d/$DEAMON_NAME {start|stop|restart|status}"
exit 1
;;

esac
exit 0
' | sudo tee "/etc/init.d/lora_daemon"

dos2unix -f "/etc/init.d/lora_daemon"
chmod +x "/etc/init.d/lora_daemon"
chown -h root:root "/etc/init.d/lora_daemon"

# Add the LoRa Daemon service to the startup list, (99 = start last). This can also be started programmatically with the command: "service lora_daemon start"
#update-rc.d lora_daemon defaults 99

# Increase Priority of the "lora_daemon" Process
#sed -i -E -e 's/^(\s*start\-stop\-daemon\s+\-\-start\s+\-\-background\s+)(\-\-pidfile\s+.+)$/\1--nicelevel -10 \2/' "/etc/init.d/lora_daemon"
