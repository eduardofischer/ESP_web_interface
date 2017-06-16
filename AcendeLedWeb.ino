#include <ESP8266WiFi.h>
#define PIN1 15

const char* ssid = "########";
const char* password = "########";

// Network settings
IPAddress ip(10, 1, 1, 50);
IPAddress gateway(10, 1, 1, 1);
IPAddress subnet(255, 255, 255, 0);

// Create an instance of the server
// specify the port to listen on as an argument
WiFiServer server(80);

void setup() {
  Serial.begin(115200);
  delay(10);

  // prepare GPIO2
  pinMode(PIN1, OUTPUT);
  digitalWrite(PIN1, 0);

  // Connect to WiFi
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.config(ip, gateway, subnet);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");

  // Start the server
  server.begin();
  Serial.println("Server started");

  // Print the IP address
  Serial.println(WiFi.localIP());
}

void loop() {
  // Check if a client has connected
  WiFiClient client = server.available();
  if (!client) {
    return;
  }

  // Wait until the client sends some
  while(!client.available()){
    delay(1);
  }

  // Read the first line of the request
  String req = client.readStringUntil('\r');
  Serial.println(req);
  client.flush();

  // Match the request
  // Set GPIO2 according to the request
  if (req.indexOf("/gpio/0") != -1)
    digitalWrite(PIN1, 0);
  else if (req.indexOf("/gpio/1") != -1)
    digitalWrite(PIN1, 1);

  client.flush();

  //Gets the current state of the relay/device
  int leitura = 0;  //digitalRead(## PIN HERE ##);

  // Prepare the response
  String s = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n";
  s += "<!DOCTYPE HTML>";
  s += "<html>";
  s += "    <head>";
  s += "        <title>Ligue o LED!</title>";
  s += "        <meta charset='UTF-8'>";
  s += "        <link rel='stylesheet' href='https://maxcdn.bootstrapcdn.com/bootstrap/4.0.0-alpha.6/css/bootstrap.min.css'>";
  s += "    </head>";
  s += "    <body>";
  s += "        <script src='https://ajax.googleapis.com/ajax/libs/jquery/3.2.1/jquery.min.js'></script>";
  s += "        <script>";
  s += "            var status = 1;";
  s += "            $(document).ready(function() {";
  s += "                if (status == 0) {";
  s += "                    $('#btn').addClass('btn-danger').removeClass('btn-success').text('Desligar');";
  s += "                } else {";
  s += "                    $('#btn').addClass('btn-success').removeClass('btn-danger').text('Ligar');";
  s += "                }";
  s += "            });";
  s += "            function enviar(stat) {";
  s += "                var xhttp = new XMLHttpRequest;";
  s += "                xhttp.open('GET', '/gpio/' + stat, true);";
  s += "                xhttp.send();";
  s += "                if (status == 1) {";
  s += "                    status = 0;";
  s += "                    $('#btn').addClass('btn-danger').removeClass('btn-success').text('Desligar');";
  s += "                } else {";
  s += "                    status = 1;";
  s += "                    $('#btn').addClass('btn-success').removeClass('btn-danger').text('Ligar');";
  s += "                }";
  s += "            }";
  s += "        </script>";
  s += "        <div class='jumbotron text-center'>";
  s += "            <h1 class='display-2'>";
  s += "                Ligue o led!<br>";
  s += "            </h1>";
  s += "            <p class='lead'>Esta página serve apenas para ligar o led...</p>";
  s += "        </div>";
  s += "        <div class='container'>";
  s += "          <div class='row'>";
  s += "              <div class='col'>";
  s += "                  <button id='btn' onclick='enviar(status)' style='font-size: 40pt; padding: 30px;' class='btn btn-success btn-lg w-100'>Ligar </button>";
  s += "              </div>";
  s += "          </div>";
  s += "        </div>";
  s += "    </body>";
  s += "</html>";

  // Send the response to the client
  client.print(s);
  delay(1);

  // The client will actually be disconnected
  // when the function returns and 'client' object is detroyed
}
