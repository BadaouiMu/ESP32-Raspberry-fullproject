# ESP32-Raspberry-fullproject
This project involves the integration of ESP32 and Raspberry (refer to Project_in_Frensh.pdf). The installation process is as follows:

## ESP32 Installation
On my Windows machine, I utilized the Arduino IDE to install and upload the code to all four ESPs. (Arduino/)

## Raspberry Installation::  
```
wget https://github.com/BadaouiMu/Read-Markdown-Website/blob/master/Raspberry/flush.sh
sudo chmod 774 flush.sh
sh ./flush.sh
mysql -uroot -p  # Authenticate with the password chosen during installation  

# Execute the following commands (or modify them as needed):

CREATE DATABASE IF NOT EXISTS TD11;

USE TD11;

CREATE TABLE IF NOT EXISTS AP (
    id INT AUTO_INCREMENT PRIMARY KEY,
    IP_address VARCHAR(15) NOT NULL,
    MAC_address VARCHAR(17) NOT NULL,
    time TIMESTAMP
);

```


### In my machine Windows : 
I visit the website http://ip-raspberry:1880, import the file "Raspberry/flows.json," and then click on Deploy.

## Testing:
For testing purposes, use "DDOS_Paul" to assess the load balancer's functionality.

## Note : 
- In the "Result/" directory, you'll find a PDF presentation explaining the project's workings and its results.
- To maintain confidentiality, I removed the HTTPS certificate. To create new certificates, you can refer to the method outlined in: https://github.com/fhessel/esp32_https_servers