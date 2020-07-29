#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <EEPROM.h>
#include "ssid.h"
byte MaxBrightness=255;
const int MaxBrAddress=1;

ESP8266WebServer server ( 80 );

void handleRoot() {
  char temp[600];
  int sec = millis() / 1000;
  int min = sec / 60;
  int hr = min / 60;

  snprintf ( temp, 600,

"<html>\
  <head>\
    <meta http-equiv='refresh' content='5'/>\
    <title>Girlanda</title>\
    <style>\
      body { background-color: #cccccc; font-family: Arial, Helvetica, Sans-Serif; Color: #000088; font-size: 40pt; }\
      a { padding: 10px; }\
    </style>\
  </head>\
  <body>\
    <h1>Mick's star</h1>\
    <p>Uptime: %02d:%02d:%02d</p>\
    <p>Brightness: <a href='/bm2'>-</a><a href='/bm'>-</a>%03d<a href='/bp'>+</a><a href='/bp2'>+</a>\
    <p><a href='/boom'>BOOM</a>\
    <p><a href='/soliton'>SOLITON</a>\
  </body>\
</html>",

    hr, min % 60, sec % 60, MaxBrightness
  );
  //ReInit();
  server.send ( 200, "text/html", temp);
}

void handleNotFound() {
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += ( server.method() == HTTP_GET ) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";

  for ( uint8_t i = 0; i < server.args(); i++ ) {
    message += " " + server.argName ( i ) + ": " + server.arg ( i ) + "\n";
  }

  server.send ( 404, "text/plain", message );
};




// NeoPixel Ring simple sketch (c) 2013 Shae Erisson
// released under the GPLv3 license to match the rest of the AdaFruit NeoPixel library

#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

// Which pin on the Arduino is connected to the NeoPixels?
// On a Trinket or Gemma we suggest changing this to 1
#define PIN            D9

// How many NeoPixels are attached to the Arduino?
#define NUMPIXELS      144

// When we setup the NeoPixel library, we tell it how many pixels, and which pin to use to send signals.
// Note that for older NeoPixel strips you might need to change the third parameter--see the strandtest
// example for more information on possible values.
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

int delayval = 10; // delay for half a second

const int Nbeatles=7;

int Pos[Nbeatles];
int V[Nbeatles];
int R[Nbeatles];
int G[Nbeatles];
int B[Nbeatles];

int Colorsequence[3][8] = {
  {114, 32,  0,  0,  0,  0, 19,101 },
  {  3, 66,126, 66,  3,  0,  0,  0 },
  {  0,  0,  0,  1, 47,123, 85,  9 }
};

int Op[5][6] = {
   { 3, 66, 126,  66,  3,  0 },
   { 1, 47, 123,  85,  9,  0 },
   { 0, 32, 114, 101, 19,  0 },
   { 0, 19, 101, 114, 32,  0 }, 
   { 0,  9,  85, 123, 47,  1 } 
};

void PutPoint(int t,int r,int g,int b)
{
    int realpos=t/5 + 10;
    for(int dp=0;dp<6;dp++) {
      uint32_t C =pixels.getPixelColor((realpos+dp)%NUMPIXELS);
      byte* CB = (byte*) (&C);
      uint32_t C1=pixels.Color( (Op[t%5][dp]*r)/128,
                                (Op[t%5][dp]*g)/128,
                                (Op[t%5][dp]*b)/128 );
      byte* C1B= (byte*) (&C1);
      for(int j=0;j<4;j++) {
        int rC=CB[j]+C1B[j];
        if(rC>255) { CB[j] = 255; } else {CB[j]=rC; };                               
      };
      pixels.setPixelColor((realpos+dp)%NUMPIXELS,C);
    };
};

void Soliton() {
  for(int i=0;i<Nbeatles;i++) {
    Pos[i]= 10;
    G[i]=MaxBrightness;
    R[i]=0;
    B[i]=0;
  }
}

int prevshow=0;
int schedule_ReInit=0;

void ReInit() {
  int v=1;
  int r_round=random(MaxBrightness);
  int g_round=random(MaxBrightness);
  int b_round=random(MaxBrightness);
  for(int i=0;i<Nbeatles;i++) {
     Pos[i]=20*i;
     V[i]=v;
    // v=-v;//=random(-2,2);
    // if(V[i]==0) { V[i]=3;};
     R[i]=r_round;
     G[i]=g_round;
     B[i]=b_round;
  };
}

void showNumber(uint8_t b) {
  for(int i=0;i<30;i++) {
    pixels.setPixelColor(i,pixels.Color(0,0,0));
  };
  for(int i=0;i<b;i++) {
    for(int j=20;j<40; j++)
       pixels.setPixelColor(j, pixels.Color(0,40,0));
    pixels.show();
    delay(500);
    for(int j=20;j<40; j++)
       pixels.setPixelColor(j, pixels.Color(0,0,0));
    pixels.show();
    delay(  500);
  };
}

void setup() {
  // This is for Trinket 5V 16MHz, you can remove these three lines if you are not using a Trinket
#if defined (__AVR_ATtiny85__)
  if (F_CPU == 16000000) clock_prescale_set(clock_div_1);
#endif
  // End of trinket special code
  EEPROM.begin(512);
  MaxBrightness=EEPROM.read(MaxBrAddress);
  if(MaxBrightness==0) {
    MaxBrightness=100;
  };
  WiFi.begin ( ssid, password );

  pixels.begin(); // This initializes the NeoPixel library.
  randomSeed(analogRead(0));
  // Wait for connection
  int N=0;
  while ( WiFi.status() != WL_CONNECTED ) {
    delay ( 500 );
    pixels.setPixelColor(N,pixels.Color(20,0,0));
    if(N>0) {
       pixels.setPixelColor(N-1,pixels.Color(0,0,0));
    };
    pixels.show();
  };

  showNumber(WiFi.localIP()[3]);
  ReInit();
  /*for(int i=0;i<Nbeatles;i++) {
     Pos[i]=10*i;
     V[i]=random(-2,2);
     if(V[i]==0) { V[i]=3;};
     R[i]=random(MaxBrightness);
     G[i]=random(MaxBrightness);
     B[i]=random(MaxBrightness);
  };*/

  server.on ( "/", handleRoot );

  server.on ( "/boom", []() {
    Explode();
    server.sendHeader("Location", "/", true);
    server.send ( 302, "text/plain", "Redirecting..." ); 
  } );
  
  server.on ( "/soliton", []() {
    server.sendHeader("Location", "/", true);
    server.send ( 302, "text/plain", "Redirecting..." ); 
    Soliton();
  } );
  
  server.on ( "/bm", []() {
    MaxBrightness--;
    if(MaxBrightness<0 ) {MaxBrightness=0; };
    EEPROM.write(MaxBrAddress,MaxBrightness);
    ReInit();
    server.sendHeader("Location", "/", true);
    server.send ( 302, "text/plain", "Redirecting..." ); 
  } );
  
  server.on ( "/bm2", []() {
    MaxBrightness-=10;
    if(MaxBrightness<0 ) {MaxBrightness=0; };
    EEPROM.write(MaxBrAddress,MaxBrightness);
    ReInit();
    server.sendHeader("Location", "/", true); 
    server.send ( 302, "text/plain", "Redirecting..." ); 
  } );
  server.on ( "/bp", []() {
    MaxBrightness++;
    if(MaxBrightness<0 ) {MaxBrightness=0; };
    EEPROM.write(MaxBrAddress,MaxBrightness);
    ReInit();
    server.sendHeader("Location", "/", true); 
    server.send ( 302, "text/plain", "Redirecting..." ); 
  } );
  server.on ( "/bp2", []() {
    MaxBrightness+=10;
    if(MaxBrightness<0 ) {MaxBrightness=0; };
    EEPROM.write(MaxBrAddress,MaxBrightness);
    ReInit();
    server.sendHeader("Location", "/", true); 
    server.send ( 302, "text/plain", "Redirecting..." ); 
  } );
  server.onNotFound ( handleNotFound );
  server.begin();
  prevshow=millis() + 4000;
}

void Explode() {
    for(int i=0;i<Nbeatles;i++) {
      pixels.setPixelColor(Pos[i],pixels.Color(0,0,0));
      Pos[i]=((NUMPIXELS*5)/2-Nbeatles/2)+i;
      V[i]=(i-Nbeatles/2)/4;
      R[i]=MaxBrightness;
      G[i]=0;
      B[i]=0;
      
    };
    schedule_ReInit=millis()+5000;
};

int colorstep = 0;
int colorstep_increased = 0;
void loop() {
  int mls=millis();
  
  if(mls%20000 < 200) {
    if(colorstep_increased == 0) {
      colorstep = (colorstep + 1) % 8; 
      for(int i=0;i<Nbeatles;i++) {
        R[i] = Colorsequence[0][colorstep];
        G[i] = Colorsequence[1][colorstep];
        B[i] = Colorsequence[2][colorstep];

      }
    }
    colorstep_increased = 1;
  } else {
    colorstep_increased = 0;
  }
  if(schedule_ReInit != 0 && mls>schedule_ReInit) {
    ReInit();
    schedule_ReInit=0;
  }
  if(mls>prevshow+delayval) {
    for(int i=0;i<NUMPIXELS;i++) {
      pixels.setPixelColor(i,pixels.Color(0,0,0));
    };
  
    for(int i=0;i<Nbeatles;i++) {
      Pos[i]+=V[i];
      if(Pos[i]<0) { Pos[i] = 5*NUMPIXELS+Pos[i]; };
      //if(Pos[i]*5+6>=NUMPIXELS) { Pos[i] = Pos[i]- NUMPIXELS; };
      pixels.setPixelColor(Pos[i],pixels.Color(R[i],G[i],B[i]));
      PutPoint(Pos[i],R[i],G[i],B[i]);
      if(random(100)==1) {
        V[i]=random(-2,2);
        if(V[i]==0) { V[i]=3;};
      }
    };
    prevshow=mls;
    pixels.show(); // This sends the updated pixel color to the hardware.
    delay(100);
  };
  server.handleClient();
}
