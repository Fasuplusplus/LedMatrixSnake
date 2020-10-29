//El dibujado se hace de izquierda a derecha, en las columnas.
//Y es eje vertical, X es eje horizontal.
//SIEMPRE TENER UN LIMITADOR DE FPS YA SEA EN drawScreen O EN EL LOOP PRINCIPAL
//1,2,3,4,5,6, 10, 12, 15, 20, 30 y 60 (divisores de 60)
bool mode = 0; //0: Modo snake (solo control de dirección)|1:Modo manual (control de dirección y velocidad)
byte diff = 2; // Dificultad (aumento de velocidad gradual) 0: sin aumento; 1-4: aumento cada vez más frecuente y hasta un máximo más alto
byte inp = 2;//Serial Input
byte rcl = 3;//Register Clock/Refresh Outputs
byte scl = 4;//Serial Clock/Shift register
byte joyX = 0;//Pines joystick
byte joyY = 1;
byte x1 = 7;//Pines display
byte x2 = 10;
byte x3 = 11;
byte x4 = 9;
byte x5 = 12;
byte x6 = 8;
byte x7 = 6;
byte x8 = 5;
byte fila[] = {x1, x2, x3, x4, x5, x6, x7, x8};
byte res = 8; //cantidad de pixeles(matriz cuadrada)
byte speeds [] = {61, 60, 30, 20, 15, 12, 10, 6, 5, 4, 3, 2, 1,} ; //velocidades en relación al ciclo de 60 frames
int joyForceX = 0; //valores análogos del joystick
int joyForceY = 0;
byte image [] = { //modelo para el estado del display con bytes
  B00000000,
  B00000000,
  B00000000,
  B00000000,
  B00000000,
  B00000000,
  B00000000,
  B00000000,
};
class dot { //puntos
  public:
    int dotX;
    int dotY;
    byte dotSpeedX;
    byte dotSpeedY;
    void frameDot() {
      image[dotY] = (image[dotY] | (B10000000 >> dotX));
    }
};
dot head; //punto "cabeza"
byte tailLong; //longitud actual de la cola
const byte longLim = 63; //límite de longitud de la cola (cabeza == 0)
dot tail[longLim - 1]; //cola
dot apple;
long randX; //variables para números aleatorios
long randY;
byte usedX[longLim];
byte usedY[longLim];
int dotDirX = 0; // 1||-1
int dotDirY = 0; // ""
byte lastDirX;  //registro de la última dirección que lleva la cabeza, para evitar que regrese sobre su cuerpo
byte lastDirY;
byte frame = 0; //contador de frames
void startingVal() { //poner la viborita en el lugar
  tailLong = 2;
  head.dotX = 4;
  head.dotY = 4;
  for (byte i = head.dotX; i < (head.dotX + tailLong); i++) {
    tail[i - head.dotX].dotY = 4;
    tail[i - head.dotX].dotX = (i + 1);
  }
}
void write1() {
  digitalWrite(inp, HIGH);
  digitalWrite(scl, HIGH);
  digitalWrite(scl, LOW);
  digitalWrite(inp, LOW);
}
void write0() {
  digitalWrite(inp, LOW);
  digitalWrite(scl, HIGH);
  digitalWrite(scl, LOW);
}
void refresh() {
  digitalWrite(rcl, HIGH);
  digitalWrite(rcl, LOW);
}
void Xoff() {  //"apagar" todas las x
  digitalWrite(x1, HIGH);
  digitalWrite(x2, HIGH);
  digitalWrite(x3, HIGH);
  digitalWrite(x4, HIGH);
  digitalWrite(x5, HIGH);
  digitalWrite(x6, HIGH);
  digitalWrite(x7, HIGH);
  digitalWrite(x8, HIGH);
}
void drawScreen() {
  write1();
  refresh();
  for (byte currY = 0; currY < res; currY++) {
    for (byte currX = 0; currX < res; currX++) {
      if (((image[currX] << currY) & B10000000) == B10000000) {
        digitalWrite(fila[currX], LOW);
      }
    }
    delay(1.5);
    Xoff();
    write0();
    refresh();
  }
}
void borderWarp() {
  if (head.dotX == 8) {
    head.dotX = 0;
  }
  else if (head.dotX == -1) {
    head.dotX = 7;
  }
  if (head.dotY == 8) {
    head.dotY = 0;
  }
  else if (head.dotY == -1) {
    head.dotY = 7;
  }
}
void placeApple() {
generate:
  randX = random (0, 8);
  randY = random (0, 8);
  bool ocup = false;
  for (byte i = 0; i <= tailLong; i++) {
    if (randX == usedX[i] && randY == usedY[i]) {
      ocup = true;
    }
    if (ocup == true) {
      goto generate;
    }
    else {
      apple.dotX = randX;
      apple.dotY = randY;
    }
  }
}
void eatApple() {
  if (head.dotX == apple.dotX && head.dotY == apple.dotY) {
    tailLong++;
    tail[tailLong - 1].dotX = tail[tailLong - 2].dotX;
    tail[tailLong - 1].dotY = tail[tailLong - 2].dotY;
    track();
    placeApple();
  }
}
void blinkApple() {
  if (frame % 10 == 0) {
    apple.frameDot();
  }
}
void die () {
  while (true) {
    head.frameDot();
    frameTail(1);
    for (byte i = 0; i < 35; i++) {
      drawScreen();
    }
    delay(250);
  }
}
void collCheck() {
  track();
  for (byte i = 1; i <= tailLong; i++)
    if (head.dotX == usedX[i]) {
      if (head.dotY == usedY[i]) {
        die();
      }
    }
}
void taylTracing() { //jaja es gracioso porque se parece a RayTracing pero es para mover la cola
  for (int i = (tailLong - 1); i >= 0; i--) {
    if (i == 0) {
      tail[i].dotX = head.dotX;
      tail[i].dotY = head.dotY;
    }
    else {
      tail[i].dotX = tail[i - 1].dotX;
      tail[i].dotY = tail[i - 1].dotY;
    }
  }
}
void track() { //registrar posiciones ocupadas
  usedX[0] = head.dotX;
  usedY[0] = head.dotY;
  for (byte i = 1; i <= tailLong; i++) {
    usedX[i] = tail[i - 1].dotX;
    usedY[i] = tail[i - 1].dotY;
  }
}
void frameTail(byte blinkSpeed) {
  for (byte i = 0; i < tailLong; i++) {
    if (frame % blinkSpeed == 0) {
      tail[i].frameDot();
    }
  }
}
void clearFrame() {
  image [0] = B00000000;
  image [1] = B00000000;
  image [2] = B00000000;
  image [3] = B00000000;
  image [4] = B00000000;
  image [5] = B00000000;
  image [6] = B00000000;
  image [7] = B00000000;
}
void checkJoystick() { //Incluye controles manuales analógicos y control solo de dirección
  int iX = analogRead(joyX);
  int iY = analogRead(joyY);
  if (mode == 1) { //Modo control manual analógico v
    joyForceX = map(iX, 0, 1023, -6, 6);
    joyForceY = map(iY, 0, 1023, -6, 6);
    if (joyForceX < 0) {
      dotDirX = 1;
      joyForceX = abs(joyForceX); //para después asignar el valor a la velocidad
    }
    else if (joyForceX > 0) {
      dotDirX = -1;
    }
    if (joyForceY < 0) {
      dotDirY = -1;
      joyForceY = abs(joyForceY);
    }
    else if (joyForceY > 0) {
      dotDirY = 1;
    }
    head.dotSpeedX = speeds[joyForceX];
    head.dotSpeedY = speeds[joyForceY];
  } //Modo control manual analógico ^
  else { //Modo control "snake" (solo control de dirección) v
    joyForceX = map(iX, 0, 1023, 1, -1); //están invertidos porque me quedaron invertidas las direcciones porque soy tonto
    joyForceY = map(iY, 0, 1023, -1, 1);
    if (joyForceX != 0 && (head.dotX + joyForceX) != usedX[1] && -1 * joyForceX != dotDirX) { //chequeo redundante para minimizar bugs porque sobra potencia para hacerlo y asi no se mata cuando warpea en los bordes
      dotDirX = joyForceX;
      dotDirY = 0;
      switch (diff) { //dificultades; si se cambian para el eje X cambiarlas también para el eje Y y viceversa o se rompe el sistema de colisiones
        case 0:
          head.dotSpeedX = speeds[2];
          break;
        case 1:
          head.dotSpeedX = speeds[map (tailLong, 2, 15, 2, 3)];
          break;
        case 2:
          head.dotSpeedX = speeds[map (tailLong, 2, 15, 2, 4)];
          break;
        case 3:
          head.dotSpeedX = speeds[map (tailLong, 2, 15, 2, 5)];
          break;
        case 4:
          head.dotSpeedX = speeds[map (tailLong, 2, 15, 2, 6)];
          break;
      }
    }
    if (joyForceY != 0 && (head.dotY + joyForceY) != usedY[1] && -1 * joyForceY != dotDirY) {
      dotDirY = joyForceY;
      dotDirX = 0;
      switch (diff) { //dificultades; si se cambian para el eje X cambiarlas también para el eje Y y viceversa o se rompe el sistema de colisiones
        case 0:
          head.dotSpeedY = speeds[2];
          break;
        case 1:
          head.dotSpeedY = speeds[map (tailLong, 2, 15, 2, 3)];
          break;
        case 2:
          head.dotSpeedY = speeds[map (tailLong, 2, 15, 2, 4)];
          break;
        case 3:
          head.dotSpeedY = speeds[map (tailLong, 2, 15, 2, 5)];
          break;
        case 4:
          head.dotSpeedY = speeds[map (tailLong, 2, 15, 2, 6)];
          break;
      }
    }
    lastDirX = dotDirX;
    lastDirY = dotDirY;
  } //Modo control "snake" (solo control de dirección) ^
}
void setup() {
  Serial.begin(9600);
  randomSeed(analogRead(5));
  startingVal();
  pinMode(inp, OUTPUT);
  pinMode(rcl, OUTPUT);
  pinMode(scl, OUTPUT);
  pinMode(joyX, INPUT);
  pinMode(joyY, INPUT);
  pinMode(x1, OUTPUT);
  pinMode(x2, OUTPUT);
  pinMode(x3, OUTPUT);
  pinMode(x4, OUTPUT);
  pinMode(x5, OUTPUT);
  pinMode(x6, OUTPUT);
  pinMode(x7, OUTPUT);
  pinMode(x8, OUTPUT);
  Xoff();
  digitalWrite(rcl, LOW);
  track();
  placeApple();
}

void loop() {
  collCheck();
  eatApple();
  head.frameDot();
  blinkApple();
  frameTail(3);
  drawScreen();
  frame++;
  checkJoystick();
  borderWarp();
  bool ch = 0; // pa revisar si ya hice taylTracing y no hacerlo dos veces en el mismo frame.
  if ((frame % head.dotSpeedX) == 0) {
    if ((head.dotX + dotDirX) != usedX[1]) {
      taylTracing();
      ch = 1;
      head.dotX = (head.dotX + dotDirX);
      track();
    }
  }
  if ((frame % head.dotSpeedY) == 0) {
    if ((head.dotY + dotDirY) != usedY[1]) {
      if (ch == 0) {
        taylTracing();
      }
      head.dotY = (head.dotY + dotDirY);
      track();
    }
  }
  if (frame == 60) {
    frame = 0;
  }
  clearFrame();
}
