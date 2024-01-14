#!/bin/bash
echo "--------------------------------------"
echo "Mise à jour du système"
sudo apt -y update
sudo apt -y upgrade
echo "--------------------------------------"
echo "Installation du broker"
sudo apt -y install mosquitto
sudo systemctl enable mosquitto.service
echo "--------------------------------------"
echo "---Sécurisation par mot de passe"
# Demander un nom d'uilisateur du broker
echo "Entrer le nom d'utilisateur [Admin]: "
read varname
if [ -z $varname]
then
	varname="Admin"
fi
echo "Entrer un mot de passe pour l'utilisateur $varname : "
sudo mosquitto_passwd -c /etc/mosquitto/mqttpass $varname
echo "--------------------------------------"
echo "---Configuration du broker"
filename="/etc/mosquitto/mosquitto.conf"
echo "allow_anonymous false" >> $filename
echo "password_file /etc/mosquitto/mqttpass" >> $filename
cat /etc/mosquitto/mqttpass
echo "--------------------------------------"
echo "---Redemarrage du broker"
sudo systemctl restart mosquitto
sudo systemctl status mosquitto
echo "--------------------------------------"
sudo apt get install mosquitto_clients
Sudo apt install npm
bash <(curl -sL https://raw.githubusercontent.com/node-red/linux-installers/master/deb/update-nodejs-and-nodered) --node20
sudo systemctl enable nodered.service
node-red-start
sudo apt-get install mariadb-server
sudo mysql_secure_installation