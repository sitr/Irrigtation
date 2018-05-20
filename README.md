# Irrigation controller
An Arduino controller for handling mulitple watering valves from [HomeSeer](http://www.homeseer.com).

## Parts list
* Wemos D1 Mini (ESP8266 with wifi)
* <img align="right" title="SSR relay" width="100" src="https://www.kjell.com/no/image/Product_560727no/full/1/luxorparts-halvlederrele-3-32-v-25-a?636434163479829839" alt="Drawing" />SSR relays, one for each valve. Since mine are controlled using 24 VAC, I chose a [relay type](https://www.kjell.com/no/produkter/elektro-og-verktoy/elektronikk/electromechanics/releer/luxorparts-halvlederrele-3-32-v-25-a-p90764) with 24 - 380 VAC output. SSR was also selected because of an input voltage of 3 - 30 VDC, which can be controlled directly from an ESP8266.
* Test board
* Circuit board connectors
* Cables and ring end connectors
* Housing (water tight if placed outside)
* 24 VAC power supply for the valves
* 5 VDC power supply for the ESP8266

After compile and upload, the irrigation controller should start and be available at the IP-adress it got on your network.

Goto *http://[controller ip address]* in a web browser or use Curl, and you should get:

<code>
{
  "status": "Ok"
}
</code>

To control a valve, just do a HTTP POST to the controller:

*http://[controller ip address]/valve*

Add header: *Content-Type: application/json*

Add the following data to the request:

<code>{"valveId": "1", "command": "1"}</code>

The response will answer with your input command or any errors that may have occured.

In my example, valveId can be 1 - 4 and command is either 1 (on) or 0 (off)
