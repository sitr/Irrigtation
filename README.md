# Irrigation controller
An Arduino controller for handling mulitple watering valves from [HomeSeer](http://www.homeseer.com).

## Parts list
* Wemos D1 Mini (ESP8266 with wifi)
* <img align="right" title="SSR relay" src="https://www.kjell.com/no/image/Product_560727no/full/1/luxorparts-halvlederrele-3-32-v-25-a?636434163479829839" alt="Drawing" style="width: 200px; "/>SSR relays, one for each valve. Since mine are controlled using 24 VAC, I chose a [relay type](https://www.kjell.com/no/produkter/elektro-og-verktoy/elektronikk/electromechanics/releer/luxorparts-halvlederrele-3-32-v-25-a-p90764) with 24 - 380 VAC output. SSR was also selected because of an input voltage of 3 - 30 VDC, which can be controlled directly from an ESP8266.
* Test board
* Circuit board connectors
* Cables and ring end connectors
* Housing (water tight if placed outside)
* 24 VAC power supply for the valves
* 5 VDC power supply for the ESP8266