#include <EEPROM.h>                            //Library EEPROM

//RTC
#include <RTClib.h>                            //Library RTC
RTC_DS3231 rtc;                                //Objek untuk RTC-DS3231

char dataHari[7][12] = {"Minggu", "Senin", "Selasa", "Rabu", "Kamis", "Jumat", "Sabtu"};                                                                                                                                                          
String hari;                                   //Menyimpan nilai hari
int tanggal, bulan, tahun, jam, menit, detik;  //Menyimpan nilai tanggal - detik
int jam1, menit1, jam2, menit2;                //Menyimpan nilai jam - detik
int jamMakan, menitMakan;                      //Menyimpan nilai jammakan,menitmakan,detikmakan
int makan,b,c;                                 //Menyimpan nilai makan
float suhu;                                    //Menyimpan nilai suhu

//Bot Telegram
#include "CTBot.h"                             //Library untuk bot telegram
CTBot myBot;                                   //Objek untuk CTBot

//LCD
#include <LiquidCrystal_I2C.h>                 //Library LCD I2C
LiquidCrystal_I2C lcd(0x27, 16, 2);            //Alamat I2C LCD 16x2

//Servo
#include<Servo.h>                              //Library servo
Servo mekanik;                                 //Objek untuk Servo

//Buzzer
#define BUZZER 14                              //Pasang di pin D5

//Koneksi WiFi & Bot Telegram
String ssid  = "zzzzzz";                                          //Username
String pass  = "yyyyyyyy";                                        //Kata sandi
String token = "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";  //Token bot telegram
const int id = vvvvvvvvvv;                                        //ID telegram

//Variabel penampung untuk parameter
String arrData[4];                                                //String("), gabungan beberapa char(karakter)

void setup () {
  Serial.begin(9600);                                             //Baut Rate

  myBot.wifiConnect(ssid,pass);                                   //Koneksi telegram melalui wifi
  myBot.setTelegramToken(token);                                  //Mengatur token telegram
  if(myBot.testConnection() )                                     //Uji koneksi ke telegram
    Serial.println("Koneksi Berhasil");
  else
    Serial.println("Koneksi Gagal");
  
//EEPROM
  EEPROM.begin(6);
  jam1       = EEPROM.read(0);                  //Baca EEPROM
  menit1     = EEPROM.read(1);
  jam2       = EEPROM.read(2);
  menit2     = EEPROM.read(3);
  jamMakan   = EEPROM.read(4);
  menitMakan = EEPROM.read(5);
  delay(100);

//LCD
  lcd.begin();

//BUZZER
  pinMode(BUZZER,OUTPUT);                      //Buzzer sebagai output

//Servo
  mekanik.attach(12);                          //Pasang di pin D6
  mekanik.write(0);                            //Atur posisi servo ke posisi 0 (derajat)

//RTC
  if (! rtc.begin()) {                         //Jika RTC tidak ditemukan
    Serial.println("RTC Tidak Ditemukan");     //Menampilkan pada serial monitor                                                        
    Serial.flush();                            //Pengosongan data pembacaan yang ditaruh pada buffer             
    abort();                                   //Mengakhiri/menghentikan proses sebelum perjalanan proses berlangsung sempurna                 
  }//if

//Atur Waktu (NOTE : setelah sekali atur langsung disable)
  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));                               //Atur otomatis
  //rtc.adjust(DateTime(2022, 01, 21, 17, 45, 0));                              //Atur manual
}//setup

void loop () {
  DateTime now = rtc.now();
  hari    = dataHari[now.dayOfTheWeek()];                                       //Data hari
  tanggal = now.day(), DEC;                                                     //Data tanggal
  bulan   = now.month(), DEC;                                                   //Data bulan
  tahun   = now.year(), DEC;                                                    //Data tahun
  jam     = now.hour(), DEC;                                                    //Data jam
  menit   = now.minute(), DEC;                                                  //Data menit
  detik   = now.second(), DEC;                                                  //Data detik
  suhu    = rtc.getTemperature();                                               //Data suhu

  Serial.println(String() + hari + "," + tanggal + "-" + bulan + "-" + tahun);  //Menampilkan pada serial monitor
  Serial.println(String() + jam + ":" + menit + ":" + detik);                   //Menampilkan
  //Serial.println(String() + "Suhu: " + suhu + " C");
  Serial.println(); 
  delay(1000);                                                            

//LCD
  lcd.setCursor(0,0);                                                           //Posisi 0,0
  lcd.print(String() + hari + "," + tanggal + "-" + bulan + "-" + tahun);       //Menampilkan pada LCD
  lcd.print(" ");                                                               //Spasi
  lcd.setCursor(0,1);                                                           //Posisi 0,1
  lcd.print(String() + jam + ":" + menit + ":" + detik);                        //Menampilkan
  lcd.print(" ");                                                               //Spasi
  //lcd.print(suhu);                                                            //Menampilkan suhu
  //lcd.print(" ");                                                             //Spasi

//Pembacaan Pesan Dari Telegram
TBMessage pesan;                                            //Variabel objek untuk pesan
if(myBot.getNewMessage(pesan) ) {                           //Uji jika berhasil menerima pesan dari telegram
  Serial.println("Pesan masuk : " + pesan.text);            //Tampilkan di serial monitor isi pesan
  String message = pesan.text;                              //Variabel penampung pesan

  //Parsing pesan berdasarkan tanda #
  //Parameter : Jam1#Menit1#Jam2#Menit2
  //Index        0     1      2     3
  int index = 0;                                           //Urutan
  arrData[0] = "";                                         //Reset Variabel
  arrData[1] = "";
  arrData[2] = "";
  arrData[3] = "";
  for(int i=0; i<message.length(); i++) {
    char delimiter = '#';                                  //Karakter(') 
    if(message[i] != delimiter)
      arrData[index] += message[i];                        //Gabungkan isi data array dengan karakter yg sedang dibaca    
    else
      index = index + 1;
      delay(10);
  }
  if(index == 3) {                                         //Parameter lengkap 
    jam1      = arrData[0].toInt();                        //Ambil Jam1, Menit, Jam2, Menit2
    menit1    = arrData[1].toInt();
    jam2      = arrData[2].toInt();
    menit2    = arrData[3].toInt();
    
    EEPROM.write(0, jam1);                                 //Simpan data ke EEPROM
    EEPROM.write(1, menit1);
    EEPROM.write(2, jam2);
    EEPROM.write(3, menit2);
    EEPROM.commit();
    
    Serial.println("Atur Jam Makan....");                  //Setting Jam Makan
    myBot.sendMessage(id, "Pembaharuan Berhasil");
    lcd.setCursor(0,0);                                    //Posisi 0,1
    lcd.print("Pembaharuan    ");                          //Menampilkan
    lcd.setCursor(0,1);                                    //Posisi 0,1
    lcd.print("Berhasil ");                                //Menampilkan
    delay(3000); lcd.clear();
  }
 
  if(message == "menu"){
    myBot.sendMessage(id, "Silakan Kirim : \n1 = Pemberian pakan sekarang \n2 = Pengaturan pemberian pakan \n3 = Melihat jam makan terbaru \n4 = Pemberian pakan terakhir");
  }

  if(message =="1"){
    makan = 1;
  }
    
  if(message =="2"){
    myBot.sendMessage(id, "Silakan masukkan jam & menit \nFormat : Jam1#Menit1#Jam2#Menit2");
  }
    
  if(message =="3") {
    myBot.sendMessage(id, String() + "Jam Makan1 = " + jam1 + ":" + menit1 + "\nJam Makan2 = " + jam2 + ":" + menit2);
    Serial.println(String() + "Jam Makan1=" + jam1 + ":" + menit1 + "\nJam Makan2= " + jam2 + ":" + menit2);
    lcd.setCursor(0,0);                                                           //Posisi 0,0
    lcd.print(String() + "Jam Makan1=" + jam1 + ":" + menit1);                    //Menampilkan pada LCD
    lcd.print(" ");                                                               //Spasi
    lcd.setCursor(0,1);                                                           //Posisi 0,1
    lcd.print(String() + "Jam Makan2=" + jam2 + ":" + menit2);                    //Menampilkan
    lcd.print(" ");                                                               //Spasi
    delay(3000); lcd.clear();
  }

  if(message =="4") {
    myBot.sendMessage(id, String() + "Terakhir Makan = " + jamMakan + ":" + menitMakan);
    Serial.println(String() + "Terakhir Makan = " + jamMakan + ":" + menitMakan);
    lcd.setCursor(0,0);                                                           //Posisi 0,1
    lcd.print("Terakhir Makan ");                                                 //Menampilkan
    lcd.setCursor(0,1);                                                           //Posisi 0,1
    lcd.print(String() + jamMakan + ":" + menitMakan + "    ");                   //Menampilkan
    delay(3000); lcd.clear();  
  }
}
 delay(1000); 

//Kondisi
  Serial.println(String() + jam1 + ":" + menit1);          //Menampilkan
  Serial.println(String() + jam2 + ":" + menit2);          //Menampilkan
  if(jam == jam1 & menit == menit1 & b!=1){
    kasih_pakan(1);                                        //panggil void kasih pakan
    digitalWrite(BUZZER,HIGH);                             //Buzzer on
    delay(300);                                            //Tunda 
    digitalWrite(BUZZER,LOW);                              //Buzzer off
    Serial.println("Pemberian Makan Berhasil (Jam Makan 1)");
    myBot.sendMessage(id, "Pemberian Makan Berhasil (Jam Makan 1)");
    lcd.setCursor(0,0);                                    //Posisi 0,0
    lcd.print("Pemberian Makan Berhasil ");                //Menampilkan pada LCD
    lcd.setCursor(0,1);                                    //Posisi 0,1
    lcd.print("Berhasil ");                                //Menampilkan
    delay(3000); lcd.clear();                              
    jamMakan = jam1; menitMakan = menit1; b=1; c=0;
    EEPROM.write(4, jamMakan);
    EEPROM.write(5, menitMakan);
    EEPROM.commit();
    delay(100);
  }//if
  if(jam == jam2 & menit == menit2 & c!=1){
    kasih_pakan(3);                                        //panggil void kasih pakan
    digitalWrite(BUZZER,HIGH);                             //Buzzer on
    delay(300);                                            //Tunda 
    digitalWrite(BUZZER,LOW);                              //Buzzer off
    Serial.println("Pemberian Makan Berhasil (Jam Makan 2)");
    myBot.sendMessage(id, "Pemberian Makan Berhasil (Jam Makan 2)");
    lcd.setCursor(0,0);                                    //Posisi 0,0
    lcd.print("Pemberian Makan ");                         //Menampilkan pada LCD
    lcd.setCursor(0,1);                                    //Posisi 0,1
    lcd.print("Berhasil ");                                //Menampilkan
    delay(3000); lcd.clear();                              //Spasi
    jamMakan = jam2; menitMakan = menit2; c=1 ; b=0;
    EEPROM.write(4, jamMakan);
    EEPROM.write(5, menitMakan);
    EEPROM.commit();
    delay(100);
  }//if
  if(makan == 1){
    kasih_pakan(3);                                        //panggil void kasih pakan
    digitalWrite(BUZZER,HIGH);                             //Buzzer on
    delay(300);                                            //Tunda 
    digitalWrite(BUZZER,LOW);                              //Buzzer off
    Serial.println("Pemberian Makan Berhasil");
    myBot.sendMessage(id, "Pemberian Makan Berhasil");
    lcd.setCursor(0,0);                                    //Posisi 0,0
    lcd.print("Pemberian Makan ");                         //Menampilkan pada LCD
    lcd.setCursor(0,1);                                    //Posisi 0,1
    lcd.print("Berhasil ");                                //Menampilkan
    delay(3000); lcd.clear();                                 
    jamMakan = jam; menitMakan = menit; makan = 0;
    EEPROM.write(4, jamMakan);
    EEPROM.write(5, menitMakan);
    EEPROM.commit();
    delay(100);
  }//if
}//loop                                                                                                                           

void kasih_pakan(int jumlah) {
  for(int a=1; a<=jumlah; a++) {                           //Perulangan
    mekanik.write(180);                                    //Bergerak ke posisi 180 derajat
    delay(100);                                            //Tunda
    mekanik.write(0);                                      //Bergerak ke posisi 0 derajat
    delay(100);                                            //Tunda
  }//for
}//void kasih pakan 
 
