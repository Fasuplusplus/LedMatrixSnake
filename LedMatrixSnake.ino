//El dibujado se hace de izquierda a derecha, en las columnas.
//Y columnas, X es filas
//SIEMPRE TENER UN LIMITADOR DE FPS YA SEA EN drawScreen O EN EL LOOP PRINCIPAL
//1,2,3,4,5,6, 10, 12, 15, 20, 30 y 60 (divisores de 60)
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
byte tailLong = 3; //longitud actual de la cola
const byte longLim = 10; //límite de longitud de la cola (cabeza == 0)
dot tail[longLim - 1]; //cola
dot apple;
long randX; //variables para números aleatorios
long randY;
byte usedX[longLim];
byte usedY[longLim];
int dotDirX = 1; // 1||-1
int dotDirY = -1; // ""
byte frame = 0; //contador de frames
void declararCaprichosas() { //no se pueden declarar valores para variables de clase fuera de un loop así que las meto acá así no joden
  head.dotX = 0;
  head.dotY = 4;
  for (byte i = 0; i < tailLong; i++) {
    tail[i].dotY = 4;
    tail[i].dotX = (i + 1);
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
  randX = random (0, 7);
  randY = random (0, 7);
  bool ocup = 0;
  for (byte i = 0; i <= tailLong; i++) {
    if (randX == usedX[i] && randY == usedY[i]) {
      ocup = 1;
      break;
    }
    if (ocup == 1) {
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
    placeApple();
    tailLong++;
    taylTracing();
  }
}
void blinkApple() {
  if (frame % 10 == 0) {
    apple.frameDot();
  }
}
void taylTracing() { //jaja es gracioso porque se parece a RayTracing pero es para mover la cola
  for (int i = (tailLong - 1); i >= 0; i--) { //tailLong == 4
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
void frameTail() {
  for (byte i = 0; i < tailLong; i++) {
    tail[i].frameDot();
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
void checkJoystick() {
  int iX = analogRead(joyX);
  int iY = analogRead(joyY);
  joyForceX = map(iX, 0, 1023, -6, 6);
  joyForceY = map(iY, 0, 1023, -6, 6);
  if (joyForceX < 0) {
    dotDirX = 1;
    joyForceX = abs(joyForceX);
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
}
void setup() {
  declararCaprichosas();
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
  eatApple();
  head.frameDot();
  blinkApple();
  frameTail();
  drawScreen();
  frame++;
  checkJoystick();
  borderWarp();
  if ((frame % head.dotSpeedX) == 0) {
    taylTracing();
    head.dotX = (head.dotX + dotDirX);
    track();
  }
  if ((frame % head.dotSpeedY) == 0) {
    taylTracing();
    head.dotY = (head.dotY + dotDirY);
    track();
  }
  if (frame == 60) {
    frame = 0;
  }
  clearFrame();
}
