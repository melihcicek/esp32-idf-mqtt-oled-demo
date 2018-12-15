========================================
ESP32 MQTT 0.96 INCH OLED DISPLAY DEMO
========================================

Esp32'ye eklenen oled display için SSD1306 driver'ının yazılması için üreticinin sağladığı 
https://www.waveshare.com/wiki/0.96inch_OLED_(B) linkindeki kaynaklar kullanılmıştır.

Display ile haberleşme I2C protokolü ile gerçekleştirilmiştir. Gelen display default olarak
4 wire SPI kullanılacakmış gibi geliyor eğer farklı protokol kullanılacaksa ekranın arkasındaki
jumper dirençlerinin yerlerinin değiştirilmesi gerekiyor. Bu uygulamada I2C için gerekli düzenleme datahseet'te belirtildiği gibi yapıldı.

Mqtt haberleşmesi için iot.eclipse.org'un sağladığı Mqtt broker kullanıldı. Uygulama
basitçe gelen dataya göre bir pini ON yada OFF konumuna getirebiliyor. Ayrıca LM35  sıcaklık sensöründen aldığı sıcaklık bilgisinde belli aralıklarla broker'a gönderiyor. 
Örnek olması açısından telefon uygulaması için MQTT dashboard adlı programı şu linkten indirip kullanabilirsiniz

https://play.google.com/store/apps/details?id=net.routix.mqttdash&hl=tr

